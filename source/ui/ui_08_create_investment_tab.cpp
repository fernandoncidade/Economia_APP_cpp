#include "ui_08_create_investment_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_investment_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Análise de Investimentos"));

        invest_analysis_type = new QComboBox(widget);
        const QStringList analysis_keys = {
            "VPL e VAUE (Fluxo Uniforme)",
            "VPL Detalhado (Receitas e Custos)",
            "Payback Descontado",
            "Análise de Sensibilidade do VPL"
        };
        for (const QString& key : analysis_keys) {
            invest_analysis_type->addItem(tr_app(key.toUtf8().constData()), key);
            invest_analysis_type->setItemData(invest_analysis_type->count() - 1, key, Qt::UserRole);
        }

        invest_initial = new QLineEdit(widget);
        invest_cashflow = new QLineEdit(widget);
        invest_n = new QLineEdit(widget);
        invest_tma = new QLineEdit(widget);
        invest_annual_revenue = new QLineEdit(widget);
        invest_annual_cost = new QLineEdit(widget);
        invest_sensitivity_variation = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        invest_initial->setValidator(val);
        invest_cashflow->setValidator(val);
        invest_n->setValidator(val);
        invest_tma->setValidator(val);
        invest_annual_revenue->setValidator(val);
        invest_annual_cost->setValidator(val);
        invest_sensitivity_variation->setValidator(val);

        invest_result = new HistoryContainer(widget);
        invest_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        invest_result->setFont(fixed_font);

        invest_calc_button = new QPushButton(tr_app("Calcular"), widget);
        connect(invest_calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_investment);

        label_cashflow = new QLabel(tr_app("Fluxo de Caixa Líquido Periódico (Benefícios - Custos):"), widget);
        label_revenue = new QLabel(tr_app("Receita Anual:"), widget);
        label_cost = new QLabel(tr_app("Custo/Desembolso Anual:"), widget);
        label_sensitivity = new QLabel(tr_app("Variação Percentual na Receita (%):"), widget);

        layout->addRow(tr_app("Tipo de Análise:"), invest_analysis_type);
        layout->addRow(tr_app("Investimento Inicial:"), invest_initial);

        layout->addRow(label_cashflow, invest_cashflow);
        layout->addRow(label_revenue, invest_annual_revenue);
        layout->addRow(label_cost, invest_annual_cost);
        layout->addRow(label_sensitivity, invest_sensitivity_variation);

        layout->addRow(tr_app("Número de Períodos (n):"), invest_n);
        layout->addRow(tr_app("Taxa Mínima de Atratividade (TMA %):"), invest_tma);
        layout->addRow(invest_calc_button);

        auto toggle_fields = [this, tr_app]() {
            int analysis_type = invest_analysis_type->currentIndex();
            if (analysis_type == 0) { // VPL/VAUE Uniforme
                label_cashflow->setVisible(true);
                invest_cashflow->setVisible(true);
                label_revenue->setVisible(false);
                invest_annual_revenue->setVisible(false);
                label_cost->setVisible(false);
                invest_annual_cost->setVisible(false);
                label_sensitivity->setVisible(false);
                invest_sensitivity_variation->setVisible(false);
                if (invest_calc_button) invest_calc_button->setText(tr_app("Calcular VPL e VAUE"));
            } else if (analysis_type == 1) { // VPL Detalhado
                label_cashflow->setVisible(false);
                invest_cashflow->setVisible(false);
                label_revenue->setVisible(true);
                invest_annual_revenue->setVisible(true);
                label_cost->setVisible(true);
                invest_annual_cost->setVisible(true);
                label_sensitivity->setVisible(false);
                invest_sensitivity_variation->setVisible(false);
                if (invest_calc_button) invest_calc_button->setText(tr_app("Calcular VPL Detalhado"));
            } else if (analysis_type == 2) { // Payback Descontado
                label_cashflow->setVisible(true);
                invest_cashflow->setVisible(true);
                label_revenue->setVisible(false);
                invest_annual_revenue->setVisible(false);
                label_cost->setVisible(false);
                invest_annual_cost->setVisible(false);
                label_sensitivity->setVisible(false);
                invest_sensitivity_variation->setVisible(false);
                if (invest_calc_button) invest_calc_button->setText(tr_app("Calcular Payback Descontado"));
            } else { // Análise de Sensibilidade
                label_cashflow->setVisible(false);
                invest_cashflow->setVisible(false);
                label_revenue->setVisible(true);
                invest_annual_revenue->setVisible(true);
                label_cost->setVisible(true);
                invest_annual_cost->setVisible(true);
                label_sensitivity->setVisible(true);
                invest_sensitivity_variation->setVisible(true);
                if (invest_calc_button) invest_calc_button->setText(tr_app("Calcular Análise de Sensibilidade"));
            }
        };

        connect(invest_analysis_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
        toggle_fields();

        auto* btn_widget = new QWidget(widget);
        auto* btn_vlayout = new QVBoxLayout(btn_widget);
        btn_vlayout->setContentsMargins(0, 0, 0, 0);

        auto* top_row = new QWidget(btn_widget);
        auto* top_layout = new QHBoxLayout(top_row);
        top_layout->setContentsMargins(0, 0, 0, 0);
        auto* btn_clear_inputs = new QPushButton(tr_app("Limpar Entrada"), top_row);
        auto* btn_clear_output = new QPushButton(tr_app("Limpar Saída"), top_row);
        auto* btn_clear_all = new QPushButton(tr_app("Limpar Tudo"), top_row);
        top_layout->addWidget(btn_clear_inputs);
        top_layout->addWidget(btn_clear_output);
        top_layout->addWidget(btn_clear_all);
        btn_vlayout->addWidget(top_row);

        auto* bottom_row = new QWidget(btn_widget);
        auto* bottom_layout = new QHBoxLayout(bottom_row);
        bottom_layout->setContentsMargins(0, 0, 0, 0);
        auto* btn_edit = new QPushButton(tr_app("Editar Cálculo"), bottom_row);
        auto* btn_delete = new QPushButton(tr_app("Excluir Seleção"), bottom_row);
        auto* btn_export = new QPushButton(tr_app("Exportar PDF"), bottom_row);
        bottom_layout->addWidget(btn_edit);
        bottom_layout->addWidget(btn_delete);
        bottom_layout->addWidget(btn_export);
        btn_vlayout->addWidget(bottom_row);

        connect(btn_export, &QPushButton::clicked, this, [this]() {
            export_to_pdf(invest_result, "investimento.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            invest_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (invest_result->is_editing()) {
                invest_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (invest_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            invest_initial->clear();
            invest_cashflow->clear();
            invest_annual_revenue->clear();
            invest_annual_cost->clear();
            invest_sensitivity_variation->clear();
            invest_n->clear();
            invest_tma->clear();
            invest_analysis_type->setCurrentIndex(0);
        };

        auto clear_output = [this]() {
            invest_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(invest_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de investimento: %1").arg(e.what()), true);
        throw;
    }
}
