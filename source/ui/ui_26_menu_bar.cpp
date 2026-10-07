#include "ui_26_menu_bar.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_24_font_config_dialog.hpp"
#include "ui_25_export_pdf.hpp"
#include "ui_28_exibir_sobre.hpp"
#include "ui_33_exibir_manual.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/FontManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../utils/MathRenderer.hpp"
#include "../utils/SessionManager.hpp"
#include "../utils/DialogHelper.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QCoreApplication>
#include <QTextEdit>
#include <QRegularExpression>
#include <QStringList>
#include <QMessageBox>

void setup_menu_bar(FinancialCalculatorApp* app) {
    if (app) {
        app->create_menu_bar();
    }
}

void FinancialCalculatorApp::create_menu_bar() {
    try {
        QMenuBar* menubar = new QMenuBar(this);

        // Menu Arquivos
        QMenu* file_menu = menubar->addMenu(QCoreApplication::translate("App", "Arquivos"));
        QAction* export_current_action = new QAction(QCoreApplication::translate("App", "Exportar Atual"), this);
        QAction* export_all_action = new QAction(QCoreApplication::translate("App", "Exportar Todos"), this);
        file_menu->addAction(export_current_action);
        file_menu->addAction(export_all_action);

        file_menu->addSeparator();
        QAction* restore_session_action = new QAction(QCoreApplication::translate("App", "Restaurar Sessão Anterior"), this);
        QAction* clear_session_action = new QAction(QCoreApplication::translate("App", "Limpar Sessão Salva"), this);
        file_menu->addAction(restore_session_action);
        file_menu->addAction(clear_session_action);

        connect(restore_session_action, &QAction::triggered, this, [this]() {
            if (!SessionManager::has_saved_session()) {
                DialogHelper::information(this,
                    QCoreApplication::translate("App", "Restaurar Sessão"),
                    QCoreApplication::translate("App", "Nenhuma sessão anterior encontrada para restaurar."));
                return;
            }
            m_isLoadingSession = true;
            bool ok = SessionManager::load_session(this);
            m_isLoadingSession = false;
            if (ok) {
                DialogHelper::information(this,
                    QCoreApplication::translate("App", "Restaurar Sessão"),
                    QCoreApplication::translate("App", "Sessão anterior restaurada com sucesso."));
            }
        });

        connect(clear_session_action, &QAction::triggered, this, [this]() {
            QMessageBox::StandardButton reply = DialogHelper::question(
                this,
                QCoreApplication::translate("App", "Limpar Sessão"),
                QCoreApplication::translate("App", "Deseja realmente limpar a sessão salva e todos os resultados?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );

            if (reply == QMessageBox::Yes) {
                clear_all_history();
                SessionManager::clear_session();
                DialogHelper::information(
                    this,
                    QCoreApplication::translate("App", "Limpar Sessão"),
                    QCoreApplication::translate("App", "Sessão salva limpa com sucesso.")
                );
            }
        });

        // Menu Configuração
        QMenu* config_menu = menubar->addMenu(QCoreApplication::translate("App", "Configuração"));
        QMenu* idiomas_menu = config_menu->addMenu(QCoreApplication::translate("App", "Idiomas"));

        QAction* action_pt = new QAction(QCoreApplication::translate("App", "Português (Brasil)"), this);
        QAction* action_en = new QAction(QCoreApplication::translate("App", "English (United States)"), this);
        action_pt->setCheckable(true);
        action_en->setCheckable(true);

        QActionGroup* ag = new QActionGroup(this);
        ag->setExclusive(true);
        ag->addAction(action_pt);
        ag->addAction(action_en);

        idiomas_menu->addAction(action_pt);
        idiomas_menu->addAction(action_en);

        auto update_language_checks = [action_pt, action_en](const QString& codigo) {
            action_pt->setChecked(codigo == "pt_BR");
            action_en->setChecked(codigo == "en_US");
        };

        if (gerenciador) {
            update_language_checks(gerenciador->obter_idioma_atual());
            connect(gerenciador, &GerenciadorTraducao::idioma_alterado, this, [action_pt, action_en](const QString& codigo) {
                action_pt->setChecked(codigo == "pt_BR");
                action_en->setChecked(codigo == "en_US");
            });
        }

        connect(action_pt, &QAction::triggered, this, [this, update_language_checks]() {
            if (gerenciador) {
                gerenciador->definir_idioma("pt_BR");
                update_language_checks("pt_BR");
            }
        });

        connect(action_en, &QAction::triggered, this, [this, update_language_checks]() {
            if (gerenciador) {
                gerenciador->definir_idioma("en_US");
                update_language_checks("en_US");
            }
        });

        config_menu->addSeparator();

        QAction* font_config_action = new QAction(QCoreApplication::translate("App", "Configurar Fontes"), this);
        connect(font_config_action, &QAction::triggered, this, [this]() {
            try {
                FontConfigDialog dialog(this);
                dialog.exec();
            } catch (const std::exception& e) {
                LogManager::instance().log(QString("Erro ao abrir configuração de fontes: %1").arg(e.what()), LogManager::LogLevel::ERR);
            }
        });
        config_menu->addAction(font_config_action);

        // Menu Opções
        QMenu* options_menu = menubar->addMenu(QCoreApplication::translate("App", "Opções"));
        QAction* about_action = new QAction(QCoreApplication::translate("App", "Sobre"), this);
        about_action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+A")));
        connect(about_action, &QAction::triggered, this, [this]() {
            exibir_sobre(this);
        });
        options_menu->addAction(about_action);

        QAction* manual_action = new QAction(QCoreApplication::translate("App", "Manual"), this);
        manual_action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+M")));
        connect(manual_action, &QAction::triggered, this, [this]() {
            exibir_manual(this);
        });
        options_menu->addAction(manual_action);

        // Export All
        auto export_all = [this](const QString& custom_name = QString()) {
            bool is_en = (GerenciadorTraducao::idioma_atual_global() == "en_US");
            QString suggested_name = custom_name.isEmpty() ? (is_en ? "all_calculations.pdf" : "todos_os_calculos.pdf") : custom_name;
            suggested_name = PdfExport::localize_filename(suggested_name);

            struct Section {
                QString title;
                HistoryContainer* widget;
            };
            QList<Section> sections;

            auto add_section = [&sections](const QString& title, HistoryContainer* widget) {
                if (!widget || widget->isEmpty()) return;
                sections.append({title, widget});
            };

            add_section(QCoreApplication::translate("App", "Juros (Simples/Compostos)"), interest_result);
            add_section(QCoreApplication::translate("App", "Anuidades"), annuity_result);
            add_section(QCoreApplication::translate("App", "Gradientes"), grad_result);
            add_section(QCoreApplication::translate("App", "Equivalência de Taxas"), rate_equiv_result);
            add_section(QCoreApplication::translate("App", "Taxa Real / Aparente"), rate_real_result);
            add_section(QCoreApplication::translate("App", "Taxa Efetiva / TIR / Taxa Global"), eff_rate_result);
            add_section(QCoreApplication::translate("App", "Amortização"), amort_result);
            add_section(QCoreApplication::translate("App", "Análise de Investimentos"), invest_result);
            add_section(QCoreApplication::translate("App", "Depreciação"), deprec_result);
            add_section(QCoreApplication::translate("App", "Retorno Mínimo (TMA)"), min_return_result);
            add_section(QCoreApplication::translate("App", "Equação de Fisher"), fisher_result);
            add_section(QCoreApplication::translate("App", "VPL com Tributos"), vpl_tax_result);
            add_section(QCoreApplication::translate("App", "CAUE - Vida Econômica"), caue_result);

            QString font_css = FontManager::get_html_style();
            QStringList html_parts;
            html_parts << "<html><head><meta charset='utf-8'>" << font_css;
            html_parts << "<style>body { margin: 12px; } h1 { margin: 12px 0 10px 0; } h2 { margin: 12px 0 6px 0; } pre.calc, pre { white-space: pre-wrap; overflow-wrap: break-word; word-break: break-word; } sub { font-size: 75%; vertical-align: sub; } sup { font-size: 75%; vertical-align: super; } table { border-collapse: collapse; width: 100%; font-size: 10pt; margin: 10px 0; table-layout: fixed; } th, td { border: 1px solid #444; padding: 4px 6px; text-align: left; vertical-align: top; word-break: break-word; overflow-wrap: break-word; } thead tr { background: #f0f0f0; }</style></head><body>";
            html_parts << QString("<h1>%1</h1>").arg(QCoreApplication::translate("App", "Todos os Cálculos").toHtmlEscaped());

            for (const auto& sec : sections) {
                QString content = sec.widget->toExportHtml();
                if (!content.trimmed().isEmpty()) {
                    html_parts << QString("<h2>%1</h2>\n%2").arg(sec.title.toHtmlEscaped(), content);
                }
            }

            html_parts << "</body></html>";
            QString full_html = html_parts.join("\n");

            QTextEdit combined;
            combined.setReadOnly(true);
            combined.setHtml(full_html);
            MathRenderer::apply_radicals(combined.document());
            this->export_to_pdf(&combined, suggested_name);
        };

        // Export Current
        auto export_current = [this, export_all]() {
            int idx = tabs->currentIndex();
            QString tab_name = tabs->tabText(idx);
            bool is_en = (GerenciadorTraducao::idioma_atual_global() == "en_US");

            auto normalize = [](const QString& s) -> QString {
                QString res = s.toLower();
                res.replace(QRegularExpression("[^0-9a-z]+"), " ");
                return res.trimmed();
            };

            QString tab_norm = normalize(tab_name);

            if (idx == 4 || tab_norm == normalize(QCoreApplication::translate("App", "Amortização"))) {
                QString pdf_name = is_en ? "amortization.pdf" : "amortizacao.pdf";
                this->export_amortization_pdf(pdf_name);
                return;
            }

            if (idx == 3 || tab_norm == normalize(QCoreApplication::translate("App", "Conversão de Taxas"))) {
                if (rate_real_result && !rate_real_result->isEmpty()) {
                    QString pdf_name = is_en ? "real_nominal_rate.pdf" : "taxa_real_aparente.pdf";
                    this->export_to_pdf(rate_real_result, pdf_name, QCoreApplication::translate("App", "Taxa Real / Aparente"));
                    return;
                }
                if (rate_equiv_result && !rate_equiv_result->isEmpty()) {
                    QString pdf_name = is_en ? "rate_equivalence.pdf" : "equivalencia_taxas.pdf";
                    this->export_to_pdf(rate_equiv_result, pdf_name, QCoreApplication::translate("App", "Equivalência de Taxas"));
                    return;
                }
                if (rate_equiv_result) {
                    QString pdf_name = is_en ? "rate_equivalence.pdf" : "equivalencia_taxas.pdf";
                    this->export_to_pdf(rate_equiv_result, pdf_name, QCoreApplication::translate("App", "Equivalência de Taxas"));
                    return;
                }
            }

            struct TabMap {
                int index;
                QString title;
                HistoryContainer* widget;
                QString pdf_pt;
                QString pdf_en;
            };

            QList<TabMap> mapping = {
                {0, QCoreApplication::translate("App", "Juros Simples e Compostos"), interest_result, "juros.pdf", "interest.pdf"},
                {1, QCoreApplication::translate("App", "Anuidades"), annuity_result, "anuidades.pdf", "annuities.pdf"},
                {2, QCoreApplication::translate("App", "Gradientes"), grad_result, "gradientes.pdf", "gradients.pdf"},
                {5, QCoreApplication::translate("App", "Análise de Investimentos"), invest_result, "analise_investimentos.pdf", "investment_analysis.pdf"},
                {6, QCoreApplication::translate("App", "Depreciação"), deprec_result, "depreciacao.pdf", "depreciation.pdf"},
                {7, QCoreApplication::translate("App", "Taxa Efetiva / TIR / Taxa Global"), eff_rate_result, "taxa_efetiva_tir.pdf", "effective_rate_irr.pdf"},
                {8, QCoreApplication::translate("App", "Retorno Mínimo (TMA)"), min_return_result, "retorno_minimo_tma.pdf", "minimum_attractive_rate.pdf"},
                {9, QCoreApplication::translate("App", "Equação de Fisher"), fisher_result, "equacao_fisher.pdf", "fisher_equation.pdf"},
                {10, QCoreApplication::translate("App", "VPL com Tributos"), vpl_tax_result, "vpl_tributos.pdf", "npv_with_taxes.pdf"},
                {11, QCoreApplication::translate("App", "CAUE - Vida Econômica"), caue_result, "caue_vida_economica.pdf", "euac_economic_life.pdf"}
            };

            for (const auto& item : mapping) {
                if (idx == item.index || normalize(item.title) == tab_norm) {
                    if (item.widget) {
                        QString pdf_file = is_en ? item.pdf_en : item.pdf_pt;
                        this->export_to_pdf(item.widget, pdf_file, item.title);
                        return;
                    }
                }
            }

            QString safe_name = is_en ? "all_calculations.pdf" : "todos_os_calculos.pdf";
            export_all(safe_name);
        };

        connect(export_current_action, &QAction::triggered, this, export_current);
        connect(export_all_action, &QAction::triggered, this, [export_all]() { export_all(); });

        this->setMenuBar(menubar);

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar menu: %1").arg(e.what()));
        throw;
    }
}
