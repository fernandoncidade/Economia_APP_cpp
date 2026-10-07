#include "ui_03_create_interest_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_interest_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Juros Simples e Compostos"));

        interest_calc_type = new QComboBox(widget);
        interest_calc_type->addItems({
            tr_app("Calcular Montante (F)"),
            tr_app("Calcular Principal (P)"),
            tr_app("Comparar JS vs JC")
        });

        interest_regime = new QComboBox(widget);
        interest_regime->addItems({
            tr_app("Juros Compostos"),
            tr_app("Juros Simples")
        });

        interest_p = new QLineEdit(widget);
        interest_f = new QLineEdit(widget);
        interest_i = new QLineEdit(widget);
        interest_n = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        interest_p->setValidator(val);
        interest_f->setValidator(val);
        interest_i->setValidator(val);
        interest_n->setValidator(val);

        interest_result = new HistoryContainer(widget);
        interest_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        interest_result->setFont(fixed_font);

        auto* calc_button = new QPushButton(tr_app("Calcular"), widget);
        connect(calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_interest);

        layout->addRow(interest_calc_type);
        layout->addRow(interest_regime);
        layout->addRow(tr_app("Valor Principal (P):"), interest_p);
        layout->addRow(tr_app("Valor do Montante (F):"), interest_f);
        layout->addRow(tr_app("Taxa de Juros (i % ao período):"), interest_i);
        layout->addRow(tr_app("Número de Períodos (n):"), interest_n);
        layout->addRow(calc_button);

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
            export_to_pdf(interest_result, "juros.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            interest_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (interest_result->is_editing()) {
                interest_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
                interest_p->setFocus();
            } else {
                bool ok = interest_result->edit_selected();
                if (ok) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            interest_p->clear();
            interest_f->clear();
            interest_i->clear();
            interest_n->clear();
            interest_calc_type->setCurrentIndex(0);
            interest_regime->setCurrentIndex(0);
        };

        auto clear_output = [this]() {
            interest_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(interest_result, 1);

        auto toggle_fields = [this]() {
            int current_index = interest_calc_type->currentIndex();
            if (current_index == 0) { // Calcular Montante (F)
                interest_p->setEnabled(true);
                interest_f->setEnabled(false);
                interest_f->clear();
                interest_n->setEnabled(true);
            } else if (current_index == 1) { // Calcular Principal (P)
                interest_p->setEnabled(false);
                interest_f->setEnabled(true);
                interest_p->clear();
                interest_n->setEnabled(true);
            } else { // Comparar JS vs JC
                interest_p->setEnabled(true);
                interest_f->setEnabled(false);
                interest_f->clear();
                interest_n->setEnabled(true);
            }
        };

        connect(interest_calc_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
        toggle_fields();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de juros: %1").arg(e.what()), true);
        throw;
    }
}
