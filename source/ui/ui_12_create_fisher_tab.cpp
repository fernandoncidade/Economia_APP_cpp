#include "ui_12_create_fisher_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_fisher_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Equação de Fisher"));

        label_fisher_calc_type = new QLabel(tr_app("Tipo de Cálculo:"), widget);
        fisher_calc_type = new QComboBox(widget);
        fisher_calc_type->addItem(tr_app("Calcular TMA Nominal a partir da Real"), "Calcular TMA Nominal a partir da Real");
        fisher_calc_type->setItemData(0, "Calcular TMA Nominal a partir da Real", Qt::UserRole);
        fisher_calc_type->addItem(tr_app("Calcular TMA Real a partir da Nominal"), "Calcular TMA Real a partir da Nominal");
        fisher_calc_type->setItemData(1, "Calcular TMA Real a partir da Nominal", Qt::UserRole);

        fisher_tma_real = new QLineEdit(widget);
        fisher_tma_nominal = new QLineEdit(widget);
        fisher_inflation = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        fisher_tma_real->setValidator(val);
        fisher_tma_nominal->setValidator(val);
        fisher_inflation->setValidator(val);

        fisher_result = new HistoryContainer(widget);
        fisher_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        fisher_result->setFont(fixed_font);

        fisher_calc_button = new QPushButton(tr_app("Calcular"), widget);
        connect(fisher_calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_fisher);

        label_tma_real = new QLabel(tr_app("TMA Real (i_r % ao ano):"), widget);
        label_tma_nominal = new QLabel(tr_app("TMA Nominal (i_a % ao ano):"), widget);
        label_fisher_inflation = new QLabel(tr_app("Taxa de Inflação (θ % ao ano):"), widget);

        layout->addRow(label_fisher_calc_type, fisher_calc_type);
        layout->addRow(label_tma_real, fisher_tma_real);
        layout->addRow(label_tma_nominal, fisher_tma_nominal);
        layout->addRow(label_fisher_inflation, fisher_inflation);
        layout->addRow(fisher_calc_button);

        auto toggle_fields = [this]() {
            int calc_type = fisher_calc_type->currentIndex();
            if (calc_type == 0) { // Calcular Nominal
                label_tma_real->setVisible(true);
                fisher_tma_real->setVisible(true);
                label_tma_nominal->setVisible(false);
                fisher_tma_nominal->setVisible(false);
                fisher_tma_nominal->clear();
            } else { // Calcular Real
                label_tma_real->setVisible(false);
                fisher_tma_real->setVisible(false);
                fisher_tma_real->clear();
                label_tma_nominal->setVisible(true);
                fisher_tma_nominal->setVisible(true);
            }
        };

        connect(fisher_calc_type, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
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
            export_to_pdf(fisher_result, "fisher.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            fisher_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (fisher_result->is_editing()) {
                fisher_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (fisher_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            fisher_tma_real->clear();
            fisher_tma_nominal->clear();
            fisher_inflation->clear();
            fisher_calc_type->setCurrentIndex(0);
        };

        auto clear_output = [this]() {
            fisher_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(fisher_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba Fisher: %1").arg(e.what()), true);
        throw;
    }
}
