#include "ui_05_create_gradient_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QLabel>
#include <QCoreApplication>

void FinancialCalculatorApp::create_gradient_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Gradientes"));

        // Modo de cálculo — store Portuguese source keys for retranslation
        grad_calc_mode = new QComboBox(widget);
        QStringList calc_source_keys = {
            "Calcular Valor Presente (P)",
            "Calcular k-ésimo Termo (X_k)",
            "Renda Perpétua",
            "Calcular Termo G_k do Gradiente Aritmético"
        };
        for (int i = 0; i < calc_source_keys.size(); ++i) {
            QString translated = tr_app(calc_source_keys[i].toUtf8().constData());
            grad_calc_mode->addItem(TextFormat::to_unicode_subscripts(translated));
            grad_calc_mode->setItemData(i, calc_source_keys[i], Qt::UserRole);
        }

        // Tipo de gradiente — store Portuguese source keys for retranslation
        grad_type = new QComboBox(widget);
        QStringList type_source_keys = {
            "Gradiente Aritmético (G)",
            "Gradiente Geométrico (g)"
        };
        for (int i = 0; i < type_source_keys.size(); ++i) {
            QString translated = tr_app(type_source_keys[i].toUtf8().constData());
            grad_type->addItem(TextFormat::to_unicode_subscripts(translated));
            grad_type->setItemData(i, type_source_keys[i], Qt::UserRole);
        }

        grad_p = new QLineEdit(widget);
        grad_a = new QLineEdit(widget);
        grad_g = new QLineEdit(widget);
        grad_i = new QLineEdit(widget);
        grad_n = new QLineEdit(widget);
        grad_k = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        grad_p->setValidator(val);
        grad_a->setValidator(val);
        grad_g->setValidator(val);
        grad_i->setValidator(val);
        grad_n->setValidator(val);
        grad_k->setValidator(val);

        grad_result = new HistoryContainer(widget);
        grad_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        grad_result->setFont(fixed_font);

        auto* calc_button = new QPushButton(tr_app("Calcular"), widget);
        connect(calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_gradient);

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
            export_to_pdf(grad_result, "gradiente.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            grad_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (grad_result->is_editing()) {
                grad_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
                grad_p->setFocus();
            } else {
                bool ok = grad_result->edit_selected();
                if (ok) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(tr_app("Modo de Cálculo:"), grad_calc_mode);
        layout->addRow(tr_app("Tipo de Gradiente:"), grad_type);
        layout->addRow(tr_app("Valor Presente (P):"), grad_p);
        layout->addRow(tr_app("Renda Periódica (A):"), grad_a);
        layout->addRow(tr_app("Gradiente (G ou g %):"), grad_g);
        layout->addRow(tr_app("Taxa de Juros (i % ao período):"), grad_i);
        layout->addRow(tr_app("Número de Períodos (n):"), grad_n);
        layout->addRow(tr_app("Termo desejado (k):"), grad_k);
        layout->addRow(calc_button);
        layout->addRow(btn_widget);
        right_layout->addWidget(grad_result, 1);

        auto toggle_fields = [this]() {
            int calc_mode = grad_calc_mode->currentIndex();
            // 0 = Calcular P, 1 = Calcular X_k, 2 = Renda Perpétua, 3 = Calcular G_k

            if (calc_mode == 2) { // Renda Perpétua
                grad_p->setEnabled(false);
                grad_p->clear();
                grad_a->setEnabled(true);
                grad_g->setEnabled(false);
                grad_g->clear();
                grad_i->setEnabled(true);
                grad_n->setEnabled(false);
                grad_n->clear();
                grad_k->setEnabled(false);
                grad_k->clear();
                grad_type->setEnabled(false);
            } else if (calc_mode == 1) { // Calcular X_k (só para geométrico)
                grad_p->setEnabled(true);
                grad_a->setEnabled(false);
                grad_a->clear();
                grad_g->setEnabled(true);
                grad_i->setEnabled(true);
                grad_n->setEnabled(true);
                grad_k->setEnabled(true);
                grad_type->setEnabled(true);
                if (grad_type->currentIndex() != 1) {
                    grad_type->setCurrentIndex(1);
                }
            } else if (calc_mode == 3) { // Calcular Termo G_k do Gradiente Aritmético
                grad_p->setEnabled(true);
                grad_a->setEnabled(false);
                grad_a->clear();
                grad_g->setEnabled(false);
                grad_g->clear();
                grad_i->setEnabled(true);
                grad_n->setEnabled(true);
                grad_k->setEnabled(true);
                grad_type->setEnabled(false);
                if (grad_type->currentIndex() != 0) {
                    grad_type->setCurrentIndex(0);
                }
            } else { // Calcular P (modo 0)
                grad_p->setEnabled(false);
                grad_p->clear();
                grad_a->setEnabled(false);
                grad_a->clear();
                grad_g->setEnabled(true);
                grad_i->setEnabled(true);
                grad_n->setEnabled(true);
                grad_k->setEnabled(false);
                grad_k->clear();
                grad_type->setEnabled(true);
            }
        };

        connect(grad_calc_mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
        connect(grad_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
        toggle_fields();

        auto clear_inputs = [this]() {
            grad_p->clear();
            grad_a->clear();
            grad_g->clear();
            grad_i->clear();
            grad_n->clear();
            grad_k->clear();
            grad_calc_mode->setCurrentIndex(0);
            grad_type->setCurrentIndex(0);
        };

        auto clear_output = [this]() {
            grad_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de gradientes: %1").arg(e.what()), true);
        throw;
    }
}
