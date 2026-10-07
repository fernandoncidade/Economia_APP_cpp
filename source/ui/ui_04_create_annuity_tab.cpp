#include "ui_04_create_annuity_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_annuity_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Anuidades"));

        annuity_calc_type = new QComboBox(widget);
        annuity_calc_type->addItems({
            tr_app("Calcular Prestação (A)"),
            tr_app("Calcular Valor Presente (P)")
        });

        annuity_type = new QComboBox(widget);
        annuity_type->addItems({
            tr_app("Postecipada"),
            tr_app("Antecipada")
        });

        annuity_p = new QLineEdit(widget);
        annuity_a = new QLineEdit(widget);
        annuity_i = new QLineEdit(widget);
        annuity_n = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        annuity_p->setValidator(val);
        annuity_a->setValidator(val);
        annuity_i->setValidator(val);
        annuity_n->setValidator(val);

        annuity_result = new HistoryContainer(widget);
        annuity_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        annuity_result->setFont(fixed_font);

        auto* calc_button = new QPushButton(tr_app("Calcular"), widget);
        connect(calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_annuity);

        layout->addRow(annuity_calc_type);
        layout->addRow(tr_app("Tipo de Série:"), annuity_type);
        layout->addRow(tr_app("Valor Presente (P):"), annuity_p);
        layout->addRow(tr_app("Valor da Prestação (A):"), annuity_a);
        layout->addRow(tr_app("Taxa de Juros (i % ao período):"), annuity_i);
        layout->addRow(tr_app("Número de Períodos (n):"), annuity_n);
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
            export_to_pdf(annuity_result, "anuidades.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            annuity_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (annuity_result->is_editing()) {
                annuity_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
                annuity_p->setFocus();
            } else {
                bool ok = annuity_result->edit_selected();
                if (ok) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            annuity_p->clear();
            annuity_a->clear();
            annuity_i->clear();
            annuity_n->clear();
            annuity_calc_type->setCurrentIndex(0);
            annuity_type->setCurrentIndex(0);
        };

        auto clear_output = [this]() {
            annuity_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(annuity_result, 1);

        auto toggle_fields = [this]() {
            int current_index = annuity_calc_type->currentIndex();
            if (current_index == 0) { // Calcular Prestação (A)
                annuity_p->setEnabled(true);
                annuity_a->setEnabled(false);
                annuity_a->clear();
            } else { // Calcular Valor Presente (P)
                annuity_p->setEnabled(false);
                annuity_a->setEnabled(true);
                annuity_p->clear();
            }
        };

        connect(annuity_calc_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
        toggle_fields();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de anuidades: %1").arg(e.what()), true);
        throw;
    }
}
