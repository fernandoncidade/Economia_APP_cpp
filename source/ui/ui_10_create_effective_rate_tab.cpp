#include "ui_10_create_effective_rate_tab.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"

#include <QDoubleValidator>
#include <QFontDatabase>
#include <QPushButton>
#include <QCoreApplication>
#include <QPair>

void FinancialCalculatorApp::create_effective_rate_tab() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        auto [widget, layout, right_layout] = create_layout();
        tabs->addTab(widget, tr_app("Taxa Efetiva / TIR / Taxa Global"));

        eff_rate_calc_mode = new QComboBox(widget);
        struct ModeItem { const char* key; };
        static const ModeItem mode_items[] = {
            {"Taxa Efetiva Anual"},
            {"Taxa Interna de Retorno (TIR)"},
            {"Taxa Global de Juros"},
            {"Taxa Efetiva em Cobrança Antecipada"},
            {"TIR Modificada (TIRm)"},
            {"TMA vs Rentabilidade"},
            {"Juros Reais"}
        };
        for (const auto& item : mode_items) {
            eff_rate_calc_mode->addItem(tr_app(item.key), QString::fromUtf8(item.key));
        }

        // Mode 0: Taxa Efetiva
        eff_rate_nominal = new QLineEdit(widget);
        eff_rate_period_nominal = new QLineEdit(widget);
        eff_rate_period_cap = new QLineEdit(widget);
        eff_rate_period_target = new QLineEdit(widget);

        // Mode 1: TIR
        tir_initial = new QLineEdit(widget);
        tir_periods = new QLineEdit(widget);
        tir_return = new QLineEdit(widget);

        // Mode 2: Taxa Global
        tax_global_real = new QLineEdit(widget);
        tax_global_inf_m1 = new QLineEdit(widget);
        tax_global_inf_m2 = new QLineEdit(widget);
        tax_global_inf_m3 = new QLineEdit(widget);

        // Mode 3: Cobrança Antecipada
        adv_int_nominal = new QLineEdit(widget);
        adv_int_rate = new QLineEdit(widget);

        // Mode 4: TIRm
        tirm_initial = new QLineEdit(widget);
        tirm_periods = new QLineEdit(widget);
        tirm_return = new QLineEdit(widget);
        tirm_cap_rate = new QLineEdit(widget);

        // Mode 5: TMA vs Rentabilidade
        tma_capital = new QLineEdit(widget);
        tma_monthly_rate = new QLineEdit(widget);
        tma_rate = new QLineEdit(widget);
        tma_periods = new QLineEdit(widget);

        // Mode 6: Juros Reais
        real_int_capital = new QLineEdit(widget);
        real_int_global_rate = new QLineEdit(widget);
        real_int_inflation = new QLineEdit(widget);

        auto* val = new QDoubleValidator(widget);
        val->setNotation(QDoubleValidator::StandardNotation);

        QList<QLineEdit*> all_edits = {
            eff_rate_nominal, eff_rate_period_nominal, eff_rate_period_cap, eff_rate_period_target,
            tir_initial, tir_periods, tir_return,
            tax_global_real, tax_global_inf_m1, tax_global_inf_m2, tax_global_inf_m3,
            adv_int_nominal, adv_int_rate,
            tirm_initial, tirm_periods, tirm_return, tirm_cap_rate,
            tma_capital, tma_monthly_rate, tma_rate, tma_periods,
            real_int_capital, real_int_global_rate, real_int_inflation
        };
        for (auto* le : all_edits) {
            le->setValidator(val);
        }

        eff_rate_result = new HistoryContainer(widget);
        eff_rate_result->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        QFont fixed_font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        eff_rate_result->setFont(fixed_font);

        auto* calc_button = new QPushButton(tr_app("Calcular"), widget);
        eff_rate_calc_button = calc_button;
        connect(calc_button, &QPushButton::clicked, this, &FinancialCalculatorApp::calculate_effective_rate);

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
            export_to_pdf(eff_rate_result, "taxa_efetiva_tir.pdf");
        });
        connect(btn_delete, &QPushButton::clicked, this, [this]() {
            eff_rate_result->delete_selected();
        });

        connect(btn_edit, &QPushButton::clicked, this, [this, btn_edit, tr_app]() {
            if (eff_rate_result->is_editing()) {
                eff_rate_result->commit_edit();
                btn_edit->setText(tr_app("Editar Cálculo"));
            } else {
                if (eff_rate_result->edit_selected()) {
                    btn_edit->setText(tr_app("Salvar Edição"));
                }
            }
        });

        eff_rate_labels.clear();
        auto* label_calc_mode = new QLabel(tr_app("Modo de Cálculo:"), widget);
        eff_rate_labels.append(qMakePair(label_calc_mode, "Modo de Cálculo:"));
        layout->addRow(label_calc_mode, eff_rate_calc_mode);

        // Store rows per mode
        using WidgetPair = QPair<QLabel*, QWidget*>;
        QMap<int, QList<WidgetPair>> mode_fields;

        auto add_field_row = [&](int mode, const char* labelKey, QWidget* field) {
            auto* lbl = new QLabel(tr_app(labelKey), widget);
            eff_rate_labels.append(qMakePair(lbl, labelKey));
            layout->addRow(lbl, field);
            mode_fields[mode].append(qMakePair(lbl, field));
        };

        // Mode 0: Taxa Efetiva
        add_field_row(0, "Taxa Nominal (%):", eff_rate_nominal);
        add_field_row(0, "Período da Taxa Nominal:", eff_rate_period_nominal);
        add_field_row(0, "Período de Capitalização:", eff_rate_period_cap);
        add_field_row(0, "Período Desejado:", eff_rate_period_target);

        // Mode 1: TIR
        add_field_row(1, "Investimento Inicial (R$):", tir_initial);
        add_field_row(1, "Número de Períodos:", tir_periods);
        add_field_row(1, "Retorno por Período (R$):", tir_return);

        // Mode 2: Taxa Global
        add_field_row(2, "Taxa Real Mensal (%):", tax_global_real);
        add_field_row(2, "Inflação Mês 1 (%):", tax_global_inf_m1);
        add_field_row(2, "Inflação Mês 2 (%):", tax_global_inf_m2);
        add_field_row(2, "Inflação Mês 3 (%):", tax_global_inf_m3);

        // Mode 3: Cobrança Antecipada
        add_field_row(3, "Valor Nominal do Empréstimo (R$):", adv_int_nominal);
        add_field_row(3, "Taxa de Cobrança Antecipada (%):", adv_int_rate);

        // Mode 4: TIRm
        add_field_row(4, "Investimento Inicial (R$):", tirm_initial);
        add_field_row(4, "Número de Períodos:", tirm_periods);
        add_field_row(4, "Retorno por Período (R$):", tirm_return);
        add_field_row(4, "Taxa de Capitalização (%):", tirm_cap_rate);

        // Mode 5: TMA vs Rentabilidade
        add_field_row(5, "Capital (R$):", tma_capital);
        add_field_row(5, "Taxa da Oportunidade (% ao mês):", tma_monthly_rate);
        add_field_row(5, "TMA (% ao ano):", tma_rate);
        add_field_row(5, "Número de Períodos (meses):", tma_periods);

        // Mode 6: Juros Reais
        add_field_row(6, "Capital (R$):", real_int_capital);
        add_field_row(6, "Taxa Global (% ao ano):", real_int_global_rate);
        add_field_row(6, "Inflação (% ao ano):", real_int_inflation);

        layout->addRow(calc_button);
        layout->addRow(btn_widget);
        right_layout->addWidget(eff_rate_result, 1);

        auto toggle_fields = [this, mode_fields]() {
            int mode = eff_rate_calc_mode->currentIndex();
            for (auto it = mode_fields.begin(); it != mode_fields.end(); ++it) {
                bool visible = (it.key() == mode);
                for (const auto& pair : it.value()) {
                    pair.first->setVisible(visible);
                    pair.second->setVisible(visible);
                }
            }
        };

        connect(eff_rate_calc_mode, QOverload<int>::of(&QComboBox::currentIndexChanged), this, toggle_fields);
        toggle_fields();

        auto clear_inputs = [all_edits]() {
            for (auto* le : all_edits) {
                le->clear();
            }
        };

        auto clear_output = [this]() {
            eff_rate_result->clear();
        };

        connect(btn_clear_inputs, &QPushButton::clicked, this, clear_inputs);
        connect(btn_clear_output, &QPushButton::clicked, this, clear_output);
        connect(btn_clear_all, &QPushButton::clicked, this, [clear_inputs, clear_output]() {
            clear_inputs();
            clear_output();
        });
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar aba de taxa efetiva/TIR: %1").arg(e.what()), true);
        throw;
    }
}
