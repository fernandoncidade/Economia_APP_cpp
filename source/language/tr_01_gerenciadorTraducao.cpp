#include "tr_01_gerenciadorTraducao.hpp"
#include "../utils/CaminhoPersistenteUtils.hpp"
#include "../utils/ApplicationPathUtils.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QXmlStreamReader>
#include <algorithm>

QList<QPair<QString, QString>> GerenciadorTraducao::s_pt_to_en;
QList<QPair<QString, QString>> GerenciadorTraducao::s_en_to_pt;
bool GerenciadorTraducao::s_dicionario_carregado = false;
static QString s_idioma_ativo = "pt_BR";

QString GerenciadorTraducao::idioma_atual_global() {
    return s_idioma_ativo;
}

GerenciadorTraducao::GerenciadorTraducao(QObject* parent)
    : QObject(parent),
      m_idioma_atual("pt_BR"),
      m_idioma_padrao("en_US"),
      m_tradutor(nullptr) {
    try {
        m_idiomas_disponiveis["pt_BR"] = "Português (Brasil)";
        m_idiomas_disponiveis["en_US"] = "English (United States)";

        QString base_path = get_app_base_path();
        QStringList candidateDirs = {
            QDir(base_path).filePath("translations"),
            QDir(base_path).filePath("source/language/translations"),
            QDir(base_path).filePath("language/translations"),
            QDir(base_path).filePath("../source/language/translations"),
            QDir(base_path).filePath("../../source/language/translations"),
            "C:/Users/ferna/APLICATIVOS/CPP/Economia_APP_cpp/source/language/translations",
            ":/translations"
        };

        m_dir_traducoes = candidateDirs.first();
        for (const QString& d : candidateDirs) {
            if (QDir(d).exists() || d.startsWith(":/")) {
                m_dir_traducoes = d;
                break;
            }
        }

        carregar_configuracao_idioma();
        carregar_dicionario_se_necessario();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao inicializar GerenciadorTraducao: %1").arg(e.what()));
    }
}

GerenciadorTraducao::~GerenciadorTraducao() {
    remover_tradutor_instalado();
}

void GerenciadorTraducao::carregar_dicionario_se_necessario() {
    if (s_dicionario_carregado) return;

    try {
        QString base_path = get_app_base_path();
        QStringList candidateDirs = {
            QDir(base_path).filePath("translations"),
            QDir(base_path).filePath("source/language/translations"),
            QDir(base_path).filePath("language/translations"),
            QDir(base_path).filePath("../source/language/translations"),
            QDir(base_path).filePath("../../source/language/translations"),
            "C:/Users/ferna/APLICATIVOS/CPP/Economia_APP_cpp/source/language/translations"
        };

        QString ts_path;
        for (const QString& d : candidateDirs) {
            QString p = QDir(d).filePath("economia_en_US.ts");
            if (QFile::exists(p)) {
                ts_path = p;
                break;
            }
        }

        if (ts_path.isEmpty() || !QFile::exists(ts_path)) {
            LogManager::warning("Arquivo TS não encontrado para montar dicionário dinâmico.");
            s_dicionario_carregado = true;
            return;
        }

        QFile file(ts_path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            LogManager::error("Falha ao abrir economia_en_US.ts para dicionário dinâmico.");
            s_dicionario_carregado = true;
            return;
        }

        QXmlStreamReader xml(&file);
        QString current_source;
        QString current_trans;

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                if (xml.name() == QLatin1String("source")) {
                    current_source = xml.readElementText();
                } else if (xml.name() == QLatin1String("translation")) {
                    current_trans = xml.readElementText();
                    if (!current_source.isEmpty() && !current_trans.isEmpty() && current_source != current_trans) {
                        QString s = current_source;
                        s.replace("{n}", "%1");
                        QString t = current_trans;
                        t.replace("{n}", "%1");
                        s_pt_to_en.append({s, t});
                        s_en_to_pt.append({s, t});

                        QString s_fmt = TextFormat::format_sub_superscripts(s);
                        QString t_fmt = TextFormat::format_sub_superscripts(t);
                        if (s_fmt != s || t_fmt != t) {
                            s_pt_to_en.append({s_fmt, t_fmt});
                            s_en_to_pt.append({s_fmt, t_fmt});
                        }
                    }
                    current_source.clear();
                    current_trans.clear();
                }
            }
        }

        // Adicionar explicitamente mapeamento de fórmulas e índices matemáticos
        static const QList<QPair<QString, QString>> math_indices = {
            {"(1 + i_{período})^m = 1 + i_{anual}", "(1 + i_{period})^m = 1 + i_{annual}"},
            {"(1 + i_período)^m = 1 + i_anual", "(1 + i_period)^m = 1 + i_annual"},
            {"(1 + iₚₑᵣᵢₒᴅₒ)ᵐ = 1 + iₐₙᵤₐₗ", "(1 + iₚₑᵣᵢₒᵈ)ᵐ = 1 + iₐₙₙᵤₐₗ"},
            {"(1 + iₚₑᵣᵢₒdₒ)ᵐ = 1 + iₐₙᵤₐₗ", "(1 + iₚₑᵣᵢₒᵈ)ᵐ = 1 + iₐₙₙᵤₐₗ"},
            {"(1 + iₚₑᵣᵢₒᴅₒ)ᵐ = 1 + iₐₙᵤₐₗ", "(1 + iₚₑᵣᵢₒᴅ)ᵐ = 1 + iₐₙₙᵤₐₗ"},
            {"i_{período} = (1 + i_{anual})^{(1/m)} - 1", "i_{period} = (1 + i_{annual})^{(1/m)} - 1"},
            {"i_período = (1 + i_anual)^(1/m) - 1", "i_period = (1 + i_annual)^(1/m) - 1"},
            {"iₚₑᵣᵢₒᴅₒ = (1 + iₐₙᵤₐₗ)⁽¹/ᵐ⁾ - 1", "iₚₑᵣᵢₒᵈ = (1 + iₐₙₙᵤₐₗ)⁽¹/ᵐ⁾ - 1"},
            {"iₚₑᵣᵢₒdₒ = (1 + iₐₙᵤₐₗ)⁽¹/ᵐ⁾ - 1", "iₚₑᵣᵢₒᵈ = (1 + iₐₙₙᵤₐₗ)⁽¹/ᵐ⁾ - 1"},
            {"iₚₑᵣᵢₒᴅₒ = (1 + iₐₙᵤₐₗ)⁽¹/ᵐ⁾ - 1", "iₚₑᵣᵢₒᴅ = (1 + iₐₙₙᵤₐₗ)⁽¹/ᵐ⁾ - 1"},
            {"i_{período}", "i_{period}"},
            {"i_{periodo}", "i_{period}"},
            {"i_{perioDo}", "i_{period}"},
            {"i_período", "i_period"},
            {"i_periodo", "i_period"},
            {"i_perioDo", "i_period"},
            {"iperioDo", "iperiod"},
            {"iperíodo", "iperiod"},
            {"iperiodo", "iperiod"},
            {"i_{anual}", "i_{annual}"},
            {"i_anual", "i_annual"},
            {"ianual", "iannual"},
            {"iₚₑᵣᵢₒᴅₒ", "iₚₑᵣᵢₒᵈ"},
            {"iₚₑᵣᵢₒdₒ", "iₚₑᵣᵢₒᵈ"},
            {"iₚₑᵣᵢₒᴅₒ", "iₚₑᵣᵢₒᴅ"},
            {"iₚₑᵣᵢₒdₒ", "iₚₑᵣᵢₒᴅ"},
            {"iₚₑᵣᵢₒᴅ", "iₚₑᵣᵢₒᵈ"},
            {"iₐₙᵤₐₗ", "iₐₙₙᵤₐₗ"},
            {"período", "period"},
            {"períodos", "periods"},
            {"Período", "Period"},
            {"Períodos", "Periods"},
            {"anual", "annual"},
            {"anualmente", "annually"},
            {"Anual", "Annual"},
            {"i_r", "i_r"},
            {"i_a", "i_a"},
            {"1 + i = (1 + r) × (1 + θ)", "1 + i = (1 + r) × (1 + θ)"},
            {"1 + i = (1 + r) * (1 + θ)", "1 + i = (1 + r) * (1 + θ)"},
            {"1 + r = (1 + i) / (1 + θ)", "1 + r = (1 + i) / (1 + θ)"},
            {"TMA Real", "Real MARR"},
            {"TMA Nominal", "Nominal MARR"},
            {"Taxa Real", "Real Rate"},
            {"Taxa Nominal", "Nominal Rate"},
            {"Relação de Fisher", "Fisher Relation"},
            {"RELAÇÃO DE FISHER", "FISHER RELATION"},
            {"Relação de Fisher (rearranjada)", "Fisher Relation (rearranged)"},
            {"RELAÇÃO DE FISHER (REARRANJADA)", "FISHER RELATION (REARRANGED)"},
            {"CAUE - Vida Econômica", "EUAW - Economic Life"},
            {"CAUE - VIDA ECONÔMICA", "EUAW - ECONOMIC LIFE"},
            {"Calcular CAUE e Vida Econômica", "Calculate EUAW and Economic Life"},
            {"CALCULAR CAUE E VIDA ECONÔMICA", "CALCULATE EUAW AND ECONOMIC LIFE"},
            {"CÁLCULO DE CAUE E VIDA ECONÔMICA DO ATIVO", "CALCULATION OF EUAW AND ASSET ECONOMIC LIFE"},
            {"CÁLCULO DO CAUE", "CALCULATION OF EUAW"},
            {"Fórmula do CAUE:", "EUAW Formula:"},
            {"FÓRMULA DO CAUE:", "EUAW FORMULA:"},
            {"Fórmula do CAUE", "EUAW Formula"},
            {"MENOR CAUE", "LOWEST EUAW"},
            {"Menor CAUE", "Lowest EUAW"},
            {"Tabela de Resultados (CAUE)", "Results Table (EUAW)"},
            {"TABELA DE RESULTADOS (CAUE)", "RESULTS TABLE (EUAW)"},
            {"Valores de Revenda e Custos", "Resale Values and Costs"},
            {"VALORES DE REVENDA E CUSTOS", "RESALE VALUES AND COSTS"},
            {"Valores de Revenda (VRₙ) e Custos de Operação (Comₙ):", "Salvage Values (SVₙ) and Operating Costs (O&Mₙ):"},
            {"Valores de Revenda (VR_n) e Custos de Operação (Com_n):", "Salvage Values (SV_n) and Operating Costs (O&M_n):"},
            {"VP DOS CUSTOS DE OPERAÇÃO (ACUMULADO)", "PV OF OPERATING COSTS (CUMULATIVE)"},
            {"VP DO VALOR DE REVENDA", "PV OF SALVAGE VALUE"},
            {"VP TOTAL", "TOTAL PV"},
            {"Dados do Ativo", "Asset Data"},
            {"DADOS DO ATIVO", "ASSET DATA"},
            {"Custo de Aquisição (P) R$:", "Acquisition Cost (P) $:"},
            {"Custo de Aquisição (P):", "Acquisition Cost (P):"},
            {"Custo de Aquisição (P)", "Acquisition Cost (P)"},
            {"Custo de Aquisição", "Acquisition Cost"},
            {"Número Máximo de Anos:", "Maximum Number of Years:"},
            {"Número Máximo de Anos", "Maximum Number of Years"},
            {"Gerar Tabela de Entrada", "Generate Input Table"},
            {"Vida Econômica", "Economic Life"},
            {"Vida Econômica do Ativo", "Asset Economic Life"},
            {"VIDA ECONÔMICA", "ECONOMIC LIFE"},
            {"CAUEₙ (R$)", "EUAWₙ ($)"},
            {"CAUE_n (R$)", "EUAW_n ($)"},
            {"Comₙ (R$)", "O&Mₙ ($)"},
            {"Com_n (R$)", "O&M_n ($)"},
            {"VRₙ (R$)", "SVₙ ($)"},
            {"VR_n (R$)", "SV_n ($)"},
            {"CAUEₙ", "EUAWₙ"},
            {"CAUE_n", "EUAW_n"},
            {"Comₙ", "O&Mₙ"},
            {"Com_n", "O&M_n"},
            {"VRₙ", "SVₙ"},
            {"VR_n", "SV_n"},
            {"CAUE", "EUAW"},
            {"VP(Custos)", "PV(Costs)"},
            {"VP(Revenda)", "PV(Salvage)"}
        };
        for (const auto& pair : math_indices) {
            s_pt_to_en.append(pair);
            s_en_to_pt.append(pair);
        }

        // Sort by length descending to replace longest phrases first
        std::sort(s_pt_to_en.begin(), s_pt_to_en.end(), [](const QPair<QString, QString>& a, const QPair<QString, QString>& b) {
            return a.first.length() > b.first.length();
        });

        std::sort(s_en_to_pt.begin(), s_en_to_pt.end(), [](const QPair<QString, QString>& a, const QPair<QString, QString>& b) {
            return a.second.length() > b.second.length();
        });

        s_dicionario_carregado = true;
        LogManager::info(QString("Dicionário de tradução dinâmica carregado com %1 pares.").arg(s_pt_to_en.size()));
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao carregar dicionário de tradução dinâmica: %1").arg(e.what()));
        s_dicionario_carregado = true;
    }
}

QString GerenciadorTraducao::traduzir_texto(const QString& texto, const QString& idioma_destino) {
    if (texto.isEmpty()) return texto;
    carregar_dicionario_se_necessario();

    // Two-pass replacement to prevent cascade: replaced text must not be re-matched
    // by shorter dictionary entries (e.g., "Gradient" inside "Gradiente" → "Gradientee")
    QString res = texto;
    QStringList targets;  // stores the final translations in order

    if (idioma_destino == "en_US") {
        for (const auto& pair : s_pt_to_en) {
            if (res.contains(pair.first)) {
                // Replace matched Portuguese text with a unique placeholder
                QString ph = QString("\x01\x02%1\x03\x04").arg(targets.size());
                res.replace(pair.first, ph);
                targets.append(pair.second);
            }
        }
    } else {
        for (const auto& pair : s_en_to_pt) {
            if (res.contains(pair.second)) {
                // Replace matched English text with a unique placeholder
                QString ph = QString("\x01\x02%1\x03\x04").arg(targets.size());
                res.replace(pair.second, ph);
                targets.append(pair.first);
            }
        }
    }

    // Second pass: replace each placeholder with the actual translation
    for (int i = 0; i < targets.size(); ++i) {
        QString ph = QString("\x01\x02%1\x03\x04").arg(i);
        res.replace(ph, targets[i]);
    }

    return TextFormat::format_sub_superscripts(res);
}

QString GerenciadorTraducao::obter_caminho_configuracao() const {
    try {
        QString dir = obter_caminho_persistente();
        return QDir(dir).filePath("language.json");
    } catch (...) {
        return QString();
    }
}

void GerenciadorTraducao::carregar_configuracao_idioma() {
    QString config_path = obter_caminho_configuracao();
    try {
        if (!config_path.isEmpty() && QFile::exists(config_path)) {
            QFile file(config_path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("idioma")) {
                        m_idioma_atual = obj["idioma"].toString();
                        s_idioma_ativo = m_idioma_atual;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao carregar configuração de idioma: %1").arg(e.what()));
    }
}

void GerenciadorTraducao::salvar_configuracao_idioma() {
    QString config_path = obter_caminho_configuracao();
    try {
        if (!config_path.isEmpty()) {
            QFile file(config_path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QJsonObject obj;
                obj["idioma"] = m_idioma_atual;
                QJsonDocument doc(obj);
                file.write(doc.toJson(QJsonDocument::Indented));
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao salvar configuração de idioma: %1").arg(e.what()));
    }
}

void GerenciadorTraducao::remover_tradutor_instalado() {
    try {
        if (m_tradutor) {
            QCoreApplication::removeTranslator(m_tradutor.get());
            m_tradutor.reset();
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao remover tradutor instalado: %1").arg(e.what()));
    }
}

bool GerenciadorTraducao::aplicar_traducao() {
    try {
        remover_tradutor_instalado();

        QString arquivo_traducao = QString("economia_%1.qm").arg(m_idioma_atual);
        QStringList possible_paths = {
            QDir(m_dir_traducoes).filePath(arquivo_traducao),
            QDir(get_app_base_path()).filePath("translations/" + arquivo_traducao),
            QDir(get_app_base_path()).filePath(arquivo_traducao),
            QDir(get_app_base_path()).filePath("source/language/translations/" + arquivo_traducao),
            QDir(get_app_base_path()).filePath("language/translations/" + arquivo_traducao),
            "C:/Users/ferna/APLICATIVOS/CPP/Economia_APP_cpp/source/language/translations/" + arquivo_traducao,
            ":/translations/" + arquivo_traducao
        };

        QString caminho_encontrado;
        for (const QString& p : possible_paths) {
            if (QFile::exists(p)) {
                caminho_encontrado = p;
                break;
            }
        }

        if (!caminho_encontrado.isEmpty()) {
            m_tradutor = std::make_unique<QTranslator>();
            if (m_tradutor->load(caminho_encontrado)) {
                QCoreApplication::installTranslator(m_tradutor.get());
                LogManager::info(QString("Tradução carregada com sucesso de: %1").arg(caminho_encontrado));
                return true;
            } else {
                LogManager::error(QString("Erro ao carregar arquivo de tradução: %1").arg(caminho_encontrado));
                m_tradutor.reset();
                return false;
            }
        }

        if (m_idioma_atual == m_idioma_padrao) {
            return true;
        }

        LogManager::warning(QString("Arquivo de tradução não encontrado: %1").arg(arquivo_traducao));
        return false;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao aplicar tradução: %1").arg(e.what()));
        return false;
    }
}

bool GerenciadorTraducao::definir_idioma(const QString& codigo_idioma) {
    try {
        if (m_idiomas_disponiveis.contains(codigo_idioma)) {
            m_idioma_atual = codigo_idioma;
            s_idioma_ativo = codigo_idioma;
            salvar_configuracao_idioma();
            bool resultado = aplicar_traducao();
            emit idioma_alterado(codigo_idioma);
            return resultado;
        }
        return false;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao definir idioma: %1").arg(e.what()));
        return false;
    }
}

QString GerenciadorTraducao::obter_idioma_atual() const {
    return m_idioma_atual;
}

void GerenciadorTraducao::traduzir_botoes_padrao(QDialog* dialogo) {
    if (!dialogo) return;
    try {
        QList<QDialogButtonBox*> boxes = dialogo->findChildren<QDialogButtonBox*>();
        for (QDialogButtonBox* box : boxes) {
            static const QMap<QDialogButtonBox::StandardButton, QString> botoes = {
                {QDialogButtonBox::Ok, "OK"},
                {QDialogButtonBox::Cancel, "Cancelar"},
                {QDialogButtonBox::Yes, "Sim"},
                {QDialogButtonBox::No, "Não"},
                {QDialogButtonBox::Abort, "Abortar"},
                {QDialogButtonBox::Retry, "Tentar Novamente"},
                {QDialogButtonBox::Ignore, "Ignorar"},
                {QDialogButtonBox::Close, "Fechar"},
                {QDialogButtonBox::Help, "Ajuda"},
                {QDialogButtonBox::Apply, "Aplicar"},
                {QDialogButtonBox::Reset, "Redefinir"},
                {QDialogButtonBox::RestoreDefaults, "Restaurar Padrões"},
                {QDialogButtonBox::Save, "Salvar"},
                {QDialogButtonBox::SaveAll, "Salvar Tudo"},
                {QDialogButtonBox::Open, "Abrir"}
            };

            for (auto it = botoes.constBegin(); it != botoes.constEnd(); ++it) {
                QPushButton* btn = box->button(it.key());
                if (btn) {
                    btn->setText(QCoreApplication::translate("Dialog", it.value().toUtf8().constData()));
                }
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao traduzir botões padrão: %1").arg(e.what()));
    }
}
