#include "ui_07_create_amortization_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QIntValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QHeaderView>
#include <QCoreApplication>
#include <QTimer>

void FinancialCalculatorApp::create_amortization_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Amortização"));

        amort_system = new QComboBox(widget);
        const QStringList system_keys = {
            "Sistema Francês (Price)",
            "Sistema de Amortização Constante (SAC)",
            "Sistema de Amortização Misto (SAM)",
            "Sistema Americano",
            "Sistema Hamburguês (SAC com Carência)"
        };
        for (const QString& key : system_keys) {
            amort_system->addItem(tr_app(key.toUtf8().constData()));
            amort_system->setItemData(amort_system->count() - 1, key, Qt::UserRole);
        }

        amort_p = new QLineEdit(widget);
        amort_i = new QLineEdit(widget);
        amort_n = new QLineEdit(widget);
        amort_e = new QLineEdit(widget);
        amort_e->setPlaceholderText(tr_app("Entrada (E)"));
        amort_k = new QLineEdit(widget);
        amort_k->setPlaceholderText(tr_app("Período desejado (k)"));
        amort_carencia = new QLineEdit(widget);
        amort_carencia->setPlaceholderText(tr_app("Períodos de carência"));

        amort_juros_capitalizados = new QCheckBox(tr_app("Capitalizar juros durante carência"), widget);
        amort_juros_capitalizados->setChecked(false);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);
        amort_p->setValidator(val);
        amort_i->setValidator(val);
        amort_n->setValidator(val);
        amort_carencia->setValidator(val);
        amort_e->setValidator(val);
        amort_k->setValidator(new QIntValidator(1, 1000000000, widget));

        auto* calc_button = new QPushButton(tr_app("Gerar Tabela de Amortização"), widget);
        connect(calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_amortization);

        auto* calc_k_button = new QPushButton(tr_app("Calcular valor no período k"), widget);
        connect(calc_k_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_value_at_k);

        amort_layout_mode = nullptr;

        amort_table = new QTableWidget();
        amort_table->setVisible(false);
        amort_table->setColumnCount(5);
        QStringList headers = {
            tr_app("Período (k)"),
            tr_app("Prestação"),
            tr_app("Juros"),
            tr_app("Amortização"),
            tr_app("Saldo Devedor")
        };
        amort_table->setHorizontalHeaderLabels(headers);

        amort_result = new HistoryContainer(widget);
        amort_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        amort_result->setFont(fixed_font);
        amort_result->setMinimumSize(0, 0);

        amort_splitter = nullptr;

        layout->addRow(tr_app("Sistema de Amortização:"), amort_system);
        layout->addRow(tr_app("Valor do Financiamento (P):"), amort_p);
        layout->addRow(tr_app("Entrada (E):"), amort_e);
        layout->addRow(tr_app("Taxa de Juros (i % ao período):"), amort_i);
        layout->addRow(tr_app("Prazo (n períodos):"), amort_n);
        layout->addRow(tr_app("Carência (períodos):"), amort_carencia);
        layout->addRow(amort_juros_capitalizados);
        layout->addRow(tr_app("Período desejado (k):"), amort_k);
        layout->addRow(calc_button);
        layout->addRow(calc_k_button);

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
            export_amortization_pdf("amortizacao.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            amort_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (amort_result->is_editing()) {
                amort_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (amort_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        layout->addRow(btn_widget);

        auto clear_inputs = [this]() {
            amort_p->clear();
            amort_i->clear();
            amort_n->clear();
            amort_carencia->clear();
            amort_e->clear();
            amort_k->clear();
            amort_system->setCurrentIndex(0);
            amort_juros_capitalizados->setChecked(false);
        };

        auto clear_output = [this]() {
            if (amort_table) {
                amort_table->clearContents();
                amort_table->setRowCount(0);
            }
            if (amort_result) {
                amort_result->clear();
            }
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });

        auto toggle_carencia_fields = [this]() {
            int sys = amort_system->currentIndex();
            bool is_price_or_sac_or_hamb = (sys == 0 || sys == 1 || sys == 4);
            amort_carencia->setVisible(is_price_or_sac_or_hamb);
            amort_juros_capitalizados->setVisible(is_price_or_sac_or_hamb);
        };

        connect(amort_system, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_carencia_fields);
        toggle_carencia_fields();

        right_layout->addWidget(amort_result, 1);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de amortização: %1").arg(e.what()), true);
        throw;
    }
}
