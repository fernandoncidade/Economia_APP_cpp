#include "ui_11_create_minimum_return_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_minimum_return_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Retorno Mínimo (TMA)"));

        min_return_investment = new QLineEdit(widget);
        min_return_tma = new QLineEdit(widget);
        min_return_periods = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        min_return_investment->setValidator(val);
        min_return_tma->setValidator(val);
        min_return_periods->setValidator(val);

        min_return_result = new HistoryContainer(widget);
        min_return_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        min_return_result->setFont(fixed_font);

        min_return_calc_button = new QPushButton(tr_app("Calcular Retorno Mínimo"), widget);
        connect(min_return_calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_minimum_return);

        label_min_return_investment = new QLabel(tr_app("Investimento Inicial (R$):"), widget);
        label_min_return_tma = new QLabel(tr_app("TMA (% ao ano):"), widget);
        label_min_return_periods = new QLabel(tr_app("Número de Períodos (anos):"), widget);

        layout->addRow(label_min_return_investment, min_return_investment);
        layout->addRow(label_min_return_tma, min_return_tma);
        layout->addRow(label_min_return_periods, min_return_periods);
        layout->addRow(min_return_calc_button);

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
            export_to_pdf(min_return_result, "tma_minima.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            min_return_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (min_return_result->is_editing()) {
                min_return_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (min_return_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            min_return_investment->clear();
            min_return_tma->clear();
            min_return_periods->clear();
        };

        auto clear_output = [this]() {
            min_return_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(min_return_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de retorno mínimo: %1").arg(e.what()), true);
        throw;
    }
}
