#include "ui_06_create_rates_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>
#include <QTimer>

void FinancialCalculatorApp::create_rates_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Conversão de Taxas"));

        layout->addRow(new QLabel(tr_app("<b>Equivalência de Taxas Efetivas</b>"), widget));
        rate_equiv_i = new QLineEdit(widget);
        rate_equiv_current_n = new QLineEdit(widget);
        rate_equiv_target_n = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        rate_equiv_i->setValidator(val);
        rate_equiv_current_n->setValidator(val);
        rate_equiv_target_n->setValidator(val);

        auto* calc_equiv_button = new QPushButton(tr_app("Calcular Taxa Equivalente"), widget);
        connect(calc_equiv_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_rate_equivalence);

        layout->addRow(tr_app("Taxa Atual (%):"), rate_equiv_i);
        layout->addRow(tr_app("Período da Taxa Atual (em unidades de tempo):"), rate_equiv_current_n);
        layout->addRow(tr_app("Período da Taxa Desejada (em unidades de tempo):"), rate_equiv_target_n);
        layout->addRow(calc_equiv_button);

        // Equivalência buttons
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
        auto* btn_edit_equiv = new QPushButton(tr_app("Editar Cálculo"), bottom_row);
        auto* btn_delete_equiv = new QPushButton(tr_app("Excluir Seleção"), bottom_row);
        auto* btn_export_equiv = new QPushButton(tr_app("Exportar PDF"), bottom_row);
        bottom_layout->addWidget(btn_edit_equiv);
        bottom_layout->addWidget(btn_delete_equiv);
        bottom_layout->addWidget(btn_export_equiv);
        btn_vlayout->addWidget(bottom_row);
        layout->addRow(btn_widget);

        // Seção Taxa Real
        layout->addRow(new QLabel(QString("<b>%1</b>").arg(tr_app("Taxa Real e Aparente (Inflação)")), widget));
        rate_real_calc_type = new QComboBox(widget);
        rate_real_calc_type->addItems({
            tr_app("Calcular Taxa Aparente (i)"),
            tr_app("Calcular Taxa Real (r)")
        });

        rate_real_r = new QLineEdit(widget);
        rate_real_i = new QLineEdit(widget);
        rate_real_inflation = new QLineEdit(widget);

        rate_real_r->setValidator(val);
        rate_real_i->setValidator(val);
        rate_real_inflation->setValidator(val);

        auto* calc_real_button = new QPushButton(tr_app("Calcular"), widget);
        connect(calc_real_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_real_rate);

        layout->addRow(rate_real_calc_type);
        layout->addRow(tr_app("Taxa Real (r %):"), rate_real_r);
        layout->addRow(tr_app("Taxa Aparente (i %):"), rate_real_i);
        layout->addRow(tr_app("Taxa de Inflação (θ %):"), rate_real_inflation);
        layout->addRow(calc_real_button);

        auto* btn_widget_real = new QWidget(widget);
        auto* btn_vlayout_real = new QVBoxLayout(btn_widget_real);
        btn_vlayout_real->setContentsMargins(0, 0, 0, 0);

        auto* top_row_r = new QWidget(btn_widget_real);
        auto* top_layout_r = new QHBoxLayout(top_row_r);
        top_layout_r->setContentsMargins(0, 0, 0, 0);
        auto* btn_clear_inputs_r = new QPushButton(tr_app("Limpar Entrada"), top_row_r);
        auto* btn_clear_output_r = new QPushButton(tr_app("Limpar Saída"), top_row_r);
        auto* btn_clear_all_r = new QPushButton(tr_app("Limpar Tudo"), top_row_r);
        top_layout_r->addWidget(btn_clear_inputs_r);
        top_layout_r->addWidget(btn_clear_output_r);
        top_layout_r->addWidget(btn_clear_all_r);
        btn_vlayout_real->addWidget(top_row_r);

        auto* bottom_row_r = new QWidget(btn_widget_real);
        auto* bottom_layout_r = new QHBoxLayout(bottom_row_r);
        bottom_layout_r->setContentsMargins(0, 0, 0, 0);
        auto* btn_edit_real = new QPushButton(tr_app("Editar Cálculo"), bottom_row_r);
        auto* btn_delete_real = new QPushButton(tr_app("Excluir Seleção"), bottom_row_r);
        auto* btn_export_real = new QPushButton(tr_app("Exportar PDF"), bottom_row_r);
        bottom_layout_r->addWidget(btn_edit_real);
        bottom_layout_r->addWidget(btn_delete_real);
        bottom_layout_r->addWidget(btn_export_real);
        btn_vlayout_real->addWidget(bottom_row_r);
        layout->addRow(btn_widget_real);

        // Disposição
        rate_layout_mode = new QComboBox(widget);
        rate_layout_mode->addItems({
            tr_app("Empilhadas (acima e abaixo)"),
            tr_app("Lado a lado")
        });
        layout->addRow(tr_app("Disposição da visualização:"), rate_layout_mode);

        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

        // Equivalência container
        rate_equiv_container = new QWidget(widget);
        auto* equiv_layout = new QVBoxLayout(rate_equiv_container);
        equiv_layout->setContentsMargins(0, 0, 0, 0);
        equiv_layout->setSpacing(2);

        rate_equiv_label = new QLabel(tr_app("<b>Resultado — Equivalência de Taxas</b>"), rate_equiv_container);
        rate_equiv_label->setAlignment(Qt::AlignCenter);
        equiv_layout->addWidget(rate_equiv_label);

        rate_equiv_result = new HistoryContainer(rate_equiv_container);
        rate_equiv_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        rate_equiv_result->setFont(fixed_font);
        equiv_layout->addWidget(rate_equiv_result);

        // Real container
        rate_real_container = new QWidget(widget);
        auto* real_layout = new QVBoxLayout(rate_real_container);
        real_layout->setContentsMargins(0, 0, 0, 0);
        real_layout->setSpacing(2);

        rate_real_label = new QLabel(tr_app("<b>Resultado — Taxa Real / Aparente</b>"), rate_real_container);
        rate_real_label->setAlignment(Qt::AlignCenter);
        real_layout->addWidget(rate_real_label);

        rate_real_result = new HistoryContainer(rate_real_container);
        rate_real_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        rate_real_result->setFont(fixed_font);
        real_layout->addWidget(rate_real_result);

        // Splitter
        rate_splitter = new QSplitter(Qt::Vertical, widget);
        rate_splitter->setChildrenCollapsible(false);
        rate_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        rate_splitter->addWidget(rate_equiv_container);
        rate_splitter->addWidget(rate_real_container);
        rate_splitter->setStretchFactor(0, 1);
        rate_splitter->setStretchFactor(1, 1);

        connect(btn_export_equiv, &QPushButton::clicked, this, [this]() {
            export_to_pdf(rate_equiv_result, "equivalencia_taxa.pdf");
        });
        connect(btn_delete_equiv, &QPushButton::clicked, this, [this]() {
            rate_equiv_result->delete_selected();
        });
        connect(btn_edit_equiv, &QPushButton::clicked, this, [this, btn_edit_equiv, tr_app]() {
            if (rate_equiv_result->is_editing()) {
                rate_equiv_result->commit_edit();
                btn_edit_equiv->setText(tr_app("Editar Cálculo"));
            } else {
                if (rate_equiv_result->edit_selected()) {
                    btn_edit_equiv->setText(tr_app("Salvar Edição"));
                }
            }
        });

        connect(btn_export_real, &QPushButton::clicked, this, [this]() {
            export_to_pdf(rate_real_result, "taxa_real_aparente.pdf");
        });
        connect(btn_delete_real, &QPushButton::clicked, this, [this]() {
            rate_real_result->delete_selected();
        });
        connect(btn_edit_real, &QPushButton::clicked, this, [this, btn_edit_real, tr_app]() {
            if (rate_real_result->is_editing()) {
                rate_real_result->commit_edit();
                btn_edit_real->setText(tr_app("Editar Cálculo"));
            } else {
                if (rate_real_result->edit_selected()) {
                    btn_edit_real->setText(tr_app("Salvar Edição"));
                }
            }
        });

        auto clear_inputs = [this]() {
            rate_equiv_i->clear();
            rate_equiv_current_n->clear();
            rate_equiv_target_n->clear();
            rate_real_calc_type->setCurrentIndex(0);
            rate_real_r->clear();
            rate_real_i->clear();
            rate_real_inflation->clear();
        };

        auto clear_output = [this]() {
            rate_equiv_result->clear();
            rate_real_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        connect(btn_clear_inputs_r, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output_r, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all_r, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        auto set_rate_orientation = [this](int index) {
            Qt::Orientation orientation = (index == 0) ? Qt::Vertical : Qt::Horizontal;
            rate_splitter->setOrientation(orientation);

            QTimer::singleShot(0, this, [this, orientation]() {
                QSize total_size = rate_splitter->size();
                if (orientation == Qt::Horizontal) {
                    int width = std::max(total_size.width(), 2);
                    int half = width / 2;
                    rate_splitter->setSizes({half, width - half});
                } else {
                    int height = std::max(total_size.height(), 2);
                    int half = height / 2;
                    rate_splitter->setSizes({half, height - half});
                }
            });
        };

        connect(rate_layout_mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, set_rate_orientation);
        set_rate_orientation(rate_layout_mode->currentIndex());

        right_layout->addWidget(rate_splitter, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de conversão de taxas: %1").arg(e.what()), true);
        throw;
    }
}
