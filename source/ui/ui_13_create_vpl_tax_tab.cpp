#include "ui_13_create_vpl_tax_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_vpl_tax_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("VPL com Tributos"));

        vpl_tax_investment = new QLineEdit(widget);
        vpl_tax_residual_value = new QLineEdit(widget);
        vpl_tax_useful_life = new QLineEdit(widget);
        vpl_tax_annual_profit = new QLineEdit(widget);
        vpl_tax_sale_year = new QLineEdit(widget);
        vpl_tax_sale_value = new QLineEdit(widget);
        vpl_tax_irpj = new QLineEdit(widget);
        vpl_tax_csll = new QLineEdit(widget);
        vpl_tax_financed = new QCheckBox(tr_app("Investimento Financiado (SAC)"), widget);
        vpl_tax_finance_rate = new QLineEdit(widget);
        vpl_tax_finance_periods = new QLineEdit(widget);
        vpl_tax_tma = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);

        QList<QLineEdit*> all_edits = {
            vpl_tax_investment, vpl_tax_residual_value, vpl_tax_useful_life,
            vpl_tax_annual_profit, vpl_tax_sale_year, vpl_tax_sale_value,
            vpl_tax_irpj, vpl_tax_csll,
            vpl_tax_finance_rate, vpl_tax_finance_periods, vpl_tax_tma
        };
        for (auto* le : all_edits) {
            le->setValidator(val);
        }

        vpl_tax_residual_value->setPlaceholderText(tr_app("Padrão: 0"));
        vpl_tax_sale_year->setPlaceholderText(tr_app("Padrão: último ano"));
        vpl_tax_sale_value->setPlaceholderText(tr_app("Padrão: 0"));
        vpl_tax_finance_rate->setPlaceholderText(tr_app("Taxa de juros (%)"));
        vpl_tax_finance_periods->setPlaceholderText(tr_app("Número de parcelas"));
        vpl_tax_irpj->setText("25");
        vpl_tax_csll->setText("9");

        auto toggle_finance_fields = [this]() {
            bool enabled = vpl_tax_financed && vpl_tax_financed->isChecked();
            if (vpl_tax_finance_rate) vpl_tax_finance_rate->setEnabled(enabled);
            if (vpl_tax_finance_periods) vpl_tax_finance_periods->setEnabled(enabled);
        };
        connect(vpl_tax_financed, &QCheckBox::toggled, this, toggle_finance_fields);
        toggle_finance_fields();

        vpl_tax_result = new HistoryContainer(widget);
        vpl_tax_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        vpl_tax_result->setFont(fixed_font);

        vpl_tax_calc_button = new QPushButton(tr_app("Calcular VPL com Tributos"), widget);
        connect(vpl_tax_calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_vpl_with_taxes);

        label_vpl_tax_investment = new QLabel(tr_app("Investimento Inicial (R$):"), widget);
        label_vpl_tax_annual_profit = new QLabel(tr_app("Lucro Antes do IR/CSLL anual (R$):"), widget);
        label_vpl_tax_useful_life = new QLabel(tr_app("Vida Útil / Depreciação (anos):"), widget);
        label_vpl_tax_irpj = new QLabel(tr_app("Alíquota IRPJ (%):"), widget);
        label_vpl_tax_csll = new QLabel(tr_app("Alíquota CSLL (%):"), widget);
        label_vpl_tax_tma = new QLabel(tr_app("Taxa Mínima de Atratividade - TMA (% ao ano):"), widget);

        label_vpl_tax_finance_rate = new QLabel(tr_app("Taxa de Financiamento (% ao ano):"), widget);
        label_vpl_tax_finance_periods = new QLabel(tr_app("Prazo do Financiamento (anos):"), widget);

        label_vpl_tax_residual_value = new QLabel(tr_app("Valor Residual Estimado (R$):"), widget);
        label_vpl_tax_sale_year = new QLabel(tr_app("Ano de Venda do Ativo (ano):"), widget);
        label_vpl_tax_sale_value = new QLabel(tr_app("Valor de Venda do Ativo (R$):"), widget);

        layout->addRow(label_vpl_tax_investment, vpl_tax_investment);
        layout->addRow(label_vpl_tax_annual_profit, vpl_tax_annual_profit);
        layout->addRow(label_vpl_tax_useful_life, vpl_tax_useful_life);
        layout->addRow(label_vpl_tax_irpj, vpl_tax_irpj);
        layout->addRow(label_vpl_tax_csll, vpl_tax_csll);
        layout->addRow(label_vpl_tax_tma, vpl_tax_tma);

        layout->addRow(vpl_tax_financed);
        layout->addRow(label_vpl_tax_finance_rate, vpl_tax_finance_rate);
        layout->addRow(label_vpl_tax_finance_periods, vpl_tax_finance_periods);

        layout->addRow(label_vpl_tax_residual_value, vpl_tax_residual_value);
        layout->addRow(label_vpl_tax_sale_year, vpl_tax_sale_year);
        layout->addRow(label_vpl_tax_sale_value, vpl_tax_sale_value);
        layout->addRow(vpl_tax_calc_button);

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
            export_to_pdf(vpl_tax_result, "vpl_tributos.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            vpl_tax_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (vpl_tax_result->is_editing()) {
                vpl_tax_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (vpl_tax_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this, all_edits]() {
            for (auto* le : all_edits) {
                le->clear();
            }
            if (vpl_tax_financed) vpl_tax_financed->setChecked(false);
            if (vpl_tax_irpj) vpl_tax_irpj->setText("25");
            if (vpl_tax_csll) vpl_tax_csll->setText("9");
        };

        auto clear_output = [this]() {
            vpl_tax_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(vpl_tax_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba VPL com impostos: %1").arg(e.what()), true);
        throw;
    }
}
