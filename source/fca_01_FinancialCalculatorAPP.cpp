#include "fca_01_FinancialCalculatorAPP.hpp"
#include "language/tr_01_gerenciadorTraducao.hpp"
#include "ui/ui_23_history_container.hpp"
#include "utils/IconUtils.hpp"
#include "utils/LogManager.hpp"
#include "utils/TextFormat.hpp"
#include "utils/SessionManager.hpp"
#include "utils/DialogHelper.hpp"

#include <QCoreApplication>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTimer>

FinancialCalculatorApp::FinancialCalculatorApp(QWidget* parent)
    : QMainWindow(parent) {
    try {
        setGeometry(100, 100, 900, 600);

        QString icon_path = get_icon_path("economia.ico");
        if (!icon_path.isEmpty()) {
            setWindowIcon(QIcon(icon_path));
        }

        tabs = new QTabWidget(this);
        setCentralWidget(tabs);

        gerenciador = new GerenciadorTraducao(this);
        connect(gerenciador, &GerenciadorTraducao::idioma_alterado,
                this, &FinancialCalculatorApp::on_language_changed);
        gerenciador->aplicar_traducao();

        setWindowTitle(QCoreApplication::translate("App", "Calculadora de Economia de Engenharia - TT007"));
        rebuild_ui();

        QTimer::singleShot(100, this, [this]() {
            check_and_prompt_restore_session();
        });
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao inicializar FinancialCalculatorApp: %1").arg(e.what()), true);
    }
}

void FinancialCalculatorApp::on_language_changed(const QString& codigo_idioma) {
    try {
        retranslate_ui(codigo_idioma);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao mudar idioma: %1").arg(e.what()), true);
    }
}

void FinancialCalculatorApp::retranslate_ui(const QString& codigo_idioma) {
    try {
        auto tr_app = [](const char* text) {
            return QCoreApplication::translate("App", text);
        };

        setWindowTitle(tr_app("Calculadora de Economia de Engenharia - TT007"));

        // 1. Atualizar títulos das abas
        if (tabs) {
            QStringList tab_titles = {
                tr_app("Juros Simples e Compostos"),
                tr_app("Anuidades"),
                tr_app("Gradientes"),
                tr_app("Conversão de Taxas"),
                tr_app("Amortização"),
                tr_app("Análise de Investimentos"),
                tr_app("Depreciação"),
                tr_app("Taxa Efetiva / TIR / Taxa Global"),
                tr_app("Retorno Mínimo (TMA)"),
                tr_app("Equação de Fisher"),
                tr_app("VPL com Tributos"),
                tr_app("CAUE - Vida Econômica")
            };
            for (int i = 0; i < tab_titles.size() && i < tabs->count(); ++i) {
                tabs->setTabText(i, tab_titles[i]);
            }
        }

        // 2. Atualizar barra de menu
        create_menu_bar();

        // 3. Traduzir rótulos e controles em tempo de execução sem apagar entradas do usuário
        QList<QLabel*> labels = findChildren<QLabel*>();
        for (auto* lbl : labels) {
            if (!lbl->text().isEmpty()) {
                lbl->setText(GerenciadorTraducao::traduzir_texto(lbl->text(), codigo_idioma));
            }
        }

        QList<QPushButton*> buttons = findChildren<QPushButton*>();
        for (auto* btn : buttons) {
            if (!btn->text().isEmpty()) {
                btn->setText(GerenciadorTraducao::traduzir_texto(btn->text(), codigo_idioma));
            }
        }

        QList<QCheckBox*> checkboxes = findChildren<QCheckBox*>();
        for (auto* chk : checkboxes) {
            if (!chk->text().isEmpty()) {
                chk->setText(GerenciadorTraducao::traduzir_texto(chk->text(), codigo_idioma));
            }
            if (!chk->toolTip().isEmpty()) {
                chk->setToolTip(GerenciadorTraducao::traduzir_texto(chk->toolTip(), codigo_idioma));
            }
        }

        QList<QComboBox*> combos = findChildren<QComboBox*>();
        for (auto* cb : combos) {
            for (int i = 0; i < cb->count(); ++i) {
                QString source_key = cb->itemData(i, Qt::UserRole).toString();
                if (!source_key.isEmpty()) {
                    // Use stored source key for precise translation via QTranslator
                    QString translated = QCoreApplication::translate("App", source_key.toUtf8().constData());
                    cb->setItemText(i, TextFormat::to_unicode_subscripts(translated));
                } else {
                    // Fallback for comboboxes without stored source keys
                    cb->setItemText(i, GerenciadorTraducao::traduzir_texto(cb->itemText(i), codigo_idioma));
                }
            }
        }

        // Retraduzir controles específicos da aba Análise de Investimentos
        if (invest_calc_button && invest_analysis_type) {
            int at = invest_analysis_type->currentIndex();
            if (at == 0) invest_calc_button->setText(tr_app("Calcular VPL e VAUE"));
            else if (at == 1) invest_calc_button->setText(tr_app("Calcular VPL Detalhado"));
            else if (at == 2) invest_calc_button->setText(tr_app("Calcular Payback Descontado"));
            else if (at == 3) invest_calc_button->setText(tr_app("Calcular Análise de Sensibilidade"));
            else invest_calc_button->setText(tr_app("Calcular"));
        }
        if (label_cashflow) label_cashflow->setText(tr_app("Fluxo de Caixa Líquido Periódico (Benefícios - Custos):"));
        if (label_revenue) label_revenue->setText(tr_app("Receita Anual:"));
        if (label_cost) label_cost->setText(tr_app("Custo/Desembolso Anual:"));
        if (label_sensitivity) label_sensitivity->setText(tr_app("Variação Percentual na Receita (%):"));

        // Retraduzir controles específicos da aba Depreciação
        if (label_deprec_method) label_deprec_method->setText(tr_app("Método de Depreciação:"));
        if (label_deprec_p) label_deprec_p->setText(tr_app("Valor de Aquisição do Ativo (P):"));
        if (label_deprec_vre) label_deprec_vre->setText(tr_app("Valor Residual Estimado (VRE):"));
        if (label_deprec_n) label_deprec_n->setText(tr_app("Vida Útil (N anos):"));
        if (label_deprec_k) label_deprec_k->setText(tr_app("Analisar ano específico (k):"));
        if (deprec_k) deprec_k->setPlaceholderText(tr_app("Opcional: para cálculo específico do ano k"));
        if (deprec_calc_button) deprec_calc_button->setText(tr_app("Calcular"));

        // Retraduzir controles específicos da aba Taxa Efetiva / TIR / Taxa Global
        for (const auto& pair : eff_rate_labels) {
            if (pair.first) {
                pair.first->setText(tr_app(pair.second));
            }
        }
        if (eff_rate_calc_button) {
            eff_rate_calc_button->setText(tr_app("Calcular"));
        }

        // Retraduzir controles específicos da aba Retorno Mínimo (TMA)
        if (label_min_return_investment) label_min_return_investment->setText(tr_app("Investimento Inicial (R$):"));
        if (label_min_return_tma) label_min_return_tma->setText(tr_app("TMA (% ao ano):"));
        if (label_min_return_periods) label_min_return_periods->setText(tr_app("Número de Períodos (anos):"));
        if (min_return_calc_button) min_return_calc_button->setText(tr_app("Calcular Retorno Mínimo"));

        // Retraduzir controles específicos da aba Equação de Fisher
        if (label_fisher_calc_type) label_fisher_calc_type->setText(tr_app("Tipo de Cálculo:"));
        if (label_tma_real) label_tma_real->setText(tr_app("TMA Real (i_r % ao ano):"));
        if (label_tma_nominal) label_tma_nominal->setText(tr_app("TMA Nominal (i_a % ao ano):"));
        if (label_fisher_inflation) label_fisher_inflation->setText(tr_app("Taxa de Inflação (θ % ao ano):"));
        if (fisher_calc_button) fisher_calc_button->setText(tr_app("Calcular"));

        // Retraduzir controles específicos da aba VPL com Tributos
        if (label_vpl_tax_investment) label_vpl_tax_investment->setText(tr_app("Investimento Inicial (R$):"));
        if (label_vpl_tax_annual_profit) label_vpl_tax_annual_profit->setText(tr_app("Lucro Antes do IR/CSLL anual (R$):"));
        if (label_vpl_tax_useful_life) label_vpl_tax_useful_life->setText(tr_app("Vida Útil / Depreciação (anos):"));
        if (label_vpl_tax_irpj) label_vpl_tax_irpj->setText(tr_app("Alíquota IRPJ (%):"));
        if (label_vpl_tax_csll) label_vpl_tax_csll->setText(tr_app("Alíquota CSLL (%):"));
        if (label_vpl_tax_tma) label_vpl_tax_tma->setText(tr_app("Taxa Mínima de Atratividade - TMA (% ao ano):"));
        if (vpl_tax_financed) vpl_tax_financed->setText(tr_app("Investimento Financiado (SAC)"));
        if (label_vpl_tax_finance_rate) label_vpl_tax_finance_rate->setText(tr_app("Taxa de Financiamento (% ao ano):"));
        if (label_vpl_tax_finance_periods) label_vpl_tax_finance_periods->setText(tr_app("Prazo do Financiamento (anos):"));
        if (label_vpl_tax_residual_value) label_vpl_tax_residual_value->setText(tr_app("Valor Residual Estimado (R$):"));
        if (label_vpl_tax_sale_year) label_vpl_tax_sale_year->setText(tr_app("Ano de Venda do Ativo (ano):"));
        if (label_vpl_tax_sale_value) label_vpl_tax_sale_value->setText(tr_app("Valor de Venda do Ativo (R$):"));
        if (vpl_tax_calc_button) vpl_tax_calc_button->setText(tr_app("Calcular VPL com Tributos"));
        if (vpl_tax_residual_value) vpl_tax_residual_value->setPlaceholderText(tr_app("Padrão: 0"));
        if (vpl_tax_sale_year) vpl_tax_sale_year->setPlaceholderText(tr_app("Padrão: último ano"));
        if (vpl_tax_sale_value) vpl_tax_sale_value->setPlaceholderText(tr_app("Padrão: 0"));
        if (vpl_tax_finance_rate) vpl_tax_finance_rate->setPlaceholderText(tr_app("Taxa de juros (%)"));
        if (vpl_tax_finance_periods) vpl_tax_finance_periods->setPlaceholderText(tr_app("Número de parcelas"));

        // Retraduzir controles específicos da aba CAUE
        if (label_caue_asset_data) label_caue_asset_data->setText(TextFormat::to_html_subscripts(tr_app("<b>Dados do Ativo</b>")));
        if (label_caue_initial_cost) label_caue_initial_cost->setText(tr_app("Custo de Aquisição (P) R$:"));
        if (label_caue_tma) label_caue_tma->setText(tr_app("TMA (% ao ano):"));
        if (label_caue_max_years) label_caue_max_years->setText(tr_app("Número Máximo de Anos:"));
        if (caue_btn_generate_table) caue_btn_generate_table->setText(tr_app("Gerar Tabela de Entrada"));
        if (label_caue_resale_costs) label_caue_resale_costs->setText(TextFormat::to_html_subscripts(tr_app("<b>Valores de Revenda e Custos</b>")));
        if (caue_calc_button) caue_calc_button->setText(tr_app("Calcular CAUE e Vida Econômica"));

        // 4. Retraduzir dinamicamente cabeçalhos das tabelas (Aba Amortização, CAUE, etc.)
        if (amort_table) {
            QStringList headers = {
                tr_app("Período (k)"),
                tr_app("Prestação"),
                tr_app("Juros"),
                tr_app("Amortização"),
                tr_app("Saldo Devedor")
            };
            amort_table->setHorizontalHeaderLabels(headers);
        }

        if (caue_input_table) {
            QStringList in_keys = {
                "Ano (n)",
                "VR_n (R$)",
                "Com_n (R$)"
            };
            for (int c = 0; c < in_keys.size(); ++c) {
                auto* hi = caue_input_table->horizontalHeaderItem(c);
                if (hi) {
                    hi->setText(tr_app(in_keys[c].toUtf8().constData()));
                } else {
                    auto* item = new QTableWidgetItem(tr_app(in_keys[c].toUtf8().constData()));
                    item->setData(Qt::UserRole, in_keys[c]);
                    caue_input_table->setHorizontalHeaderItem(c, item);
                }
            }
        }

        if (caue_output_table) {
            QStringList out_keys = {
                "Ano (n)",
                "VR_n (R$)",
                "Com_n (R$)",
                "CAUE_n (R$)"
            };
            for (int c = 0; c < out_keys.size(); ++c) {
                auto* hi = caue_output_table->horizontalHeaderItem(c);
                if (hi) {
                    hi->setText(tr_app(out_keys[c].toUtf8().constData()));
                } else {
                    auto* item = new QTableWidgetItem(tr_app(out_keys[c].toUtf8().constData()));
                    item->setData(Qt::UserRole, out_keys[c]);
                    caue_output_table->setHorizontalHeaderItem(c, item);
                }
            }
        }

        // Retradução para outras tabelas dinâmicas
        QList<QTableWidget*> all_tables = findChildren<QTableWidget*>();
        for (auto* tbl : all_tables) {
            if (tbl != amort_table && tbl != caue_input_table && tbl != caue_output_table) {
                for (int c = 0; c < tbl->columnCount(); ++c) {
                    auto* hi = tbl->horizontalHeaderItem(c);
                    if (hi) {
                        QString src_key = hi->data(Qt::UserRole).toString();
                        if (src_key.isEmpty()) {
                            src_key = hi->text();
                            hi->setData(Qt::UserRole, src_key);
                        }
                        QString translated = QCoreApplication::translate("App", src_key.toUtf8().constData());
                        if (translated == src_key) {
                            translated = GerenciadorTraducao::traduzir_texto(src_key, codigo_idioma);
                        }
                        hi->setText(translated);
                    }
                }
            }
        }

        // 5. Retraduzir todas as respostas existentes nos HistoryContainers mantendo o histórico
        QList<HistoryContainer*> containers = findChildren<HistoryContainer*>();
        for (auto* hc : containers) {
            hc->retranslate_entries(codigo_idioma);
        }

        LogManager::info(QString("UI e histórico de respostas retraduzidos com sucesso para: %1").arg(codigo_idioma));
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao retraduzir UI: %1").arg(e.what()), true);
    }
}

void FinancialCalculatorApp::rebuild_ui() {
    try {
        tabs->clear();
        create_interest_tab();
        create_annuity_tab();
        create_gradient_tab();
        create_rates_tab();
        create_amortization_tab();
        create_investment_tab();
        create_depreciation_tab();
        create_effective_rate_tab();
        create_minimum_return_tab();
        create_fisher_tab();
        create_vpl_tax_tab();
        create_caue_tab();
        create_menu_bar();

        // Conectar sinal de alteração de histórico de todos os containers ao salvamento automático
        QList<HistoryContainer*> all_containers = findChildren<HistoryContainer*>();
        for (auto* hc : all_containers) {
            connect(hc, &HistoryContainer::historyChanged, this, &FinancialCalculatorApp::auto_save_session);
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao reconstruir UI: %1").arg(e.what()), true);
    }
}

void FinancialCalculatorApp::auto_save_session() {
    if (m_isLoadingSession) return;
    SessionManager::save_session(this);
}

void FinancialCalculatorApp::check_and_prompt_restore_session() {
    if (!SessionManager::has_saved_session()) {
        return;
    }

    m_isLoadingSession = true;
    bool loaded = SessionManager::load_session(this);
    m_isLoadingSession = false;

    if (!loaded) return;

    QMessageBox::StandardButton reply = DialogHelper::question(
        this,
        QCoreApplication::translate("App", "Restaurar Sessão"),
        QCoreApplication::translate("App", "Os resultados da sessão anterior foram carregados. Deseja manter estes resultados?"),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
    );

    if (reply == QMessageBox::Yes) {
        LogManager::info("Sessão anterior confirmada e mantida pelo usuário.");
    } else {
        clear_all_history();
        SessionManager::clear_session();
        LogManager::info("Resultados da sessão anterior descartados a pedido do usuário.");
    }
}

void FinancialCalculatorApp::clear_all_history() {
    bool prevLoading = m_isLoadingSession;
    m_isLoadingSession = true;

    if (interest_result) interest_result->clear();
    if (annuity_result) annuity_result->clear();
    if (grad_result) grad_result->clear();
    if (rate_equiv_result) rate_equiv_result->clear();
    if (rate_real_result) rate_real_result->clear();
    if (amort_result) amort_result->clear();
    if (amort_table) {
        amort_table->clearContents();
        amort_table->setRowCount(0);
    }
    if (invest_result) invest_result->clear();
    if (deprec_result) deprec_result->clear();
    if (eff_rate_result) eff_rate_result->clear();
    if (min_return_result) min_return_result->clear();
    if (fisher_result) fisher_result->clear();
    if (vpl_tax_result) vpl_tax_result->clear();
    if (caue_result) caue_result->clear();
    if (caue_output_table) {
        caue_output_table->clearContents();
        caue_output_table->setRowCount(0);
    }

    QList<HistoryContainer*> all_containers = findChildren<HistoryContainer*>();
    for (auto* hc : all_containers) {
        hc->clear();
    }

    m_isLoadingSession = prevLoading;
}

void FinancialCalculatorApp::closeEvent(QCloseEvent* event) {
    if (!m_isLoadingSession) {
        SessionManager::save_session(this);
    }
    QMainWindow::closeEvent(event);
}

