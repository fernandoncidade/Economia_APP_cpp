#include "ui_14_create_caue_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QHeaderView>
#include <QCoreApplication>

void FinancialCalculatorApp::create_caue_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("CAUE - Vida Econômica"));

        caue_initial_cost = new QLineEdit(widget);
        caue_tma = new QLineEdit(widget);
        caue_max_years = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        caue_initial_cost->setValidator(val);
        caue_tma->setValidator(val);
        caue_max_years->setValidator(val);

        caue_initial_cost->setText("50000");
        caue_tma->setText("12");
        caue_max_years->setText("5");

        caue_input_table = new QTableWidget(widget);
        caue_input_table->setColumnCount(3);
        QStringList in_keys = {
            "Ano (n)",
            "VR_n (R$)",
            "Com_n (R$)"
        };
        for (int c = 0; c < in_keys.size(); ++c) {
            auto* item = new QTableWidgetItem(tr_app(in_keys[c].toUtf8().constData()));
            item->setData(Qt::UserRole, in_keys[c]);
            caue_input_table->setHorizontalHeaderItem(c, item);
        }
        caue_input_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        caue_input_table->setMinimumHeight(200);

        if (caue_output_table) {
            delete caue_output_table;
            caue_output_table = nullptr;
        }
        caue_output_table = new QTableWidget();
        caue_output_table->setVisible(false);
        caue_output_table->setColumnCount(4);
        QStringList out_keys = {
            "Ano (n)",
            "VR_n (R$)",
            "Com_n (R$)",
            "CAUE_n (R$)"
        };
        for (int c = 0; c < out_keys.size(); ++c) {
            auto* item = new QTableWidgetItem(tr_app(out_keys[c].toUtf8().constData()));
            item->setData(Qt::UserRole, out_keys[c]);
            caue_output_table->setHorizontalHeaderItem(c, item);
        }
        caue_output_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        caue_output_table->setMinimumHeight(200);

        caue_result = new HistoryContainer(widget);
        caue_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        caue_result->setFont(fixed_font);
        caue_result->setMinimumSize(0, 0);

        caue_btn_generate_table = new QPushButton(tr_app("Gerar Tabela de Entrada"), widget);
        connect(caue_btn_generate_table, &QPushButton::clicked, this, &FinancialCalculatorApp::generate_caue_input_table);

        caue_calc_button = new QPushButton(tr_app("Calcular CAUE e Vida Econômica"), widget);
        connect(caue_calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_caue);

        label_caue_asset_data = new QLabel(TextFormat::to_html_subscripts(tr_app("<b>Dados do Ativo</b>")), widget);
        label_caue_initial_cost = new QLabel(tr_app("Custo de Aquisição (P) R$:"), widget);
        label_caue_tma = new QLabel(tr_app("TMA (% ao ano):"), widget);
        label_caue_max_years = new QLabel(tr_app("Número Máximo de Anos:"), widget);
        label_caue_resale_costs = new QLabel(TextFormat::to_html_subscripts(tr_app("<b>Valores de Revenda e Custos</b>")), widget);

        layout->addRow(label_caue_asset_data);
        layout->addRow(label_caue_initial_cost, caue_initial_cost);
        layout->addRow(label_caue_tma, caue_tma);
        layout->addRow(label_caue_max_years, caue_max_years);
        layout->addRow(caue_btn_generate_table);

        layout->addRow(label_caue_resale_costs);
        layout->addRow(caue_input_table);

        layout->addRow(caue_calc_button);

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
            export_to_pdf(caue_result, "caue_vida_economica.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            caue_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (caue_result->is_editing()) {
                caue_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (caue_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        auto clear_inputs = [this]() {
            caue_initial_cost->clear();
            caue_tma->clear();
            caue_max_years->setText("5");
            caue_input_table->clearContents();
            caue_input_table->setRowCount(0);
            caue_output_table->clearContents();
            caue_output_table->setRowCount(0);
        };

        auto clear_output = [this]() {
            caue_result->clear();
            caue_output_table->clearContents();
            caue_output_table->setRowCount(0);
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        layout->addRow(btn_widget);
        right_layout->addWidget(caue_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba CAUE: %1").arg(e.what()), true);
        throw;
    }
}
