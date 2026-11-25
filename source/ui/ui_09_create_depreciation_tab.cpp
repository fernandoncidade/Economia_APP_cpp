#include "ui_09_create_depreciation_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>

void FinancialCalculatorApp::create_depreciation_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Depreciação"));

        deprec_method = new QComboBox(widget);
        deprec_method->addItem(tr_app("Método Linear"), "Método Linear");
        deprec_method->addItem(tr_app("Soma dos Dígitos (Decrescente)"), "Soma dos Dígitos (Decrescente)");
        deprec_method->addItem(tr_app("Soma dos Dígitos (Crescente)"), "Soma dos Dígitos (Crescente)");
        deprec_method->addItem(tr_app("Saldo Declinante"), "Saldo Declinante");

        deprec_p = new QLineEdit(widget);
        deprec_vre = new QLineEdit(widget);
        deprec_n = new QLineEdit(widget);
        deprec_k = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        deprec_p->setValidator(val);
        deprec_vre->setValidator(val);
        deprec_n->setValidator(val);
        deprec_k->setValidator(new QIntValidator(1, 1000000000, widget));
        deprec_k->setPlaceholderText(tr_app("Opcional: para cálculo específico do ano k"));

        deprec_result = new HistoryContainer(widget);
        deprec_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        deprec_result->setFont(fixed_font);

        deprec_calc_button = new QPushButton(tr_app("Calcular"), widget);
        connect(deprec_calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_depreciation);

        label_deprec_method = new QLabel(tr_app("Método de Depreciação:"), widget);
        label_deprec_p = new QLabel(tr_app("Valor de Aquisição do Ativo (P):"), widget);
        label_deprec_vre = new QLabel(tr_app("Valor Residual Estimado (VRE):"), widget);
        label_deprec_n = new QLabel(tr_app("Vida Útil (N anos):"), widget);
        label_deprec_k = new QLabel(tr_app("Analisar ano específico (k):"), widget);

        layout->addRow(label_deprec_method, deprec_method);
        layout->addRow(label_deprec_p, deprec_p);
        layout->addRow(label_deprec_vre, deprec_vre);
        layout->addRow(label_deprec_n, deprec_n);
        layout->addRow(label_deprec_k, deprec_k);
        layout->addRow(deprec_calc_button);

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
            export_to_pdf(deprec_result, "depreciacao.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            deprec_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (deprec_result->is_editing()) {
                deprec_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (deprec_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            deprec_p->clear();
            deprec_vre->clear();
            deprec_n->clear();
            deprec_k->clear();
            deprec_method->setCurrentIndex(0);
        };

        auto clear_output = [this]() {
            deprec_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        right_layout->addWidget(deprec_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de depreciação: %1").arg(e.what()), true);
        throw;
    }
}
