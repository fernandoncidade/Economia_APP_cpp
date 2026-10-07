#ifndef FCA_01_FINANCIAL_CALCULATOR_APP_HPP
#define FCA_01_FINANCIAL_CALCULATOR_APP_HPP

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QLabel>
#include <QSplitter>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMap>
#include <QVariantMap>
#include <tuple>
#include <optional>

#include "ui/ui_23_history_container.hpp"

class GerenciadorTraducao;
class QPushButton;

class FinancialCalculatorApp : public QMainWindow {
    Q_OBJECT

public:
    explicit FinancialCalculatorApp(QWidget* parent = nullptr);
    ~FinancialCalculatorApp() override = default;

public slots:
    void on_language_changed(const QString& codigo_idioma);
    void rebuild_ui();
    void retranslate_ui(const QString& codigo_idioma);

    // UI Tab builders
    std::tuple<QWidget*, QFormLayout*, QVBoxLayout*> create_layout();
    double get_float_from_line_edit(QLineEdit* line_edit, bool is_percentage = false, std::optional<double> default_val = std::nullopt);

    void create_interest_tab();
    void create_annuity_tab();
    void create_gradient_tab();
    void create_rates_tab();
    void create_amortization_tab();
    void create_investment_tab();
    void create_depreciation_tab();
    void create_effective_rate_tab();
    void create_minimum_return_tab();
    void create_fisher_tab();
    void create_vpl_tax_tab();
    void create_caue_tab();

    // Table Generation & Helpers
    QList<QVariantMap> generate_sac_table(double p, double i, int n);
    QList<QVariantMap> generate_price_table(double p, double i, int n);
    QList<QVariantMap> generate_sam_table(double p, double i, int n);
    QList<QVariantMap> generate_american_table(double p, double i, int n);
    QList<QVariantMap> generate_hamburgues_table(double p, double i, int n, int carencia, bool capitalizar_juros);
    void set_amort_table_row(int k, double prestacao, double juros, double amortizacao, double saldo_devedor);
    QList<QVariantMap> get_table_data(int n);
    void generate_caue_input_table();

    // Calculations / Services
    void calculate_interest();
    void calculate_annuity();
    void calculate_gradient();
    void calculate_rate_equivalence();
    void calculate_real_rate();
    void calculate_amortization();
    void calculate_investment();
    void _calculate_vpl_vaue_uniform();
    void _calculate_vpl_detailed();
    void _calculate_payback_discounted();
    void _calculate_sensitivity_analysis();
    void calculate_depreciation();
    void calculate_effective_rate();
    double calculate_tir_newton(double initial_investment, double periodic_return, int num_periods,
                                double initial_guess = 0.1, double tolerance = 1e-6, int max_iterations = 100);
    double _calculate_tir_newton(double initial_investment, double periodic_return, int num_periods,
                                 double initial_guess = 0.1, double tolerance = 1e-8, int max_iterations = 100) {
        return calculate_tir_newton(initial_investment, periodic_return, num_periods, initial_guess, tolerance, max_iterations);
    }
    void calculate_minimum_return();
    void calculate_fisher();
    void _calculate_nominal_rate();
    void _calculate_real_rate_fisher();
    void calculate_value_at_k();
    void calculate_vpl_with_taxes();
    void calculate_caue();

    // Export & Menu
    void export_to_pdf(QWidget* text_widget, const QString& suggested_name = "export.pdf");
    void export_amortization_pdf(const QString& suggested_name = "amortizacao.pdf");
    QString amort_table_to_html();
    void create_menu_bar();

public:
    // Attributes mirroring the Python application members
    QTabWidget* tabs = nullptr;
    GerenciadorTraducao* gerenciador = nullptr;

    // Tab 1: Interest
    QComboBox* interest_calc_type = nullptr;
    QComboBox* interest_regime = nullptr;
    QLineEdit* interest_p = nullptr;
    QLineEdit* interest_f = nullptr;
    QLineEdit* interest_i = nullptr;
    QLineEdit* interest_n = nullptr;
    HistoryContainer* interest_result = nullptr;

    // Tab 2: Annuity
    QComboBox* annuity_calc_type = nullptr;
    QComboBox* annuity_type = nullptr;
    QLineEdit* annuity_p = nullptr;
    QLineEdit* annuity_a = nullptr;
    QLineEdit* annuity_i = nullptr;
    QLineEdit* annuity_n = nullptr;
    HistoryContainer* annuity_result = nullptr;

    // Tab 3: Gradient
    QComboBox* grad_calc_mode = nullptr;
    QComboBox* grad_type = nullptr;
    QLineEdit* grad_p = nullptr;
    QLineEdit* grad_a = nullptr;
    QLineEdit* grad_g = nullptr;
    QLineEdit* grad_i = nullptr;
    QLineEdit* grad_n = nullptr;
    QLineEdit* grad_k = nullptr;
    HistoryContainer* grad_result = nullptr;

    // Tab 4: Rates
    QComboBox* rate_layout_mode = nullptr;
    QSplitter* rate_splitter = nullptr;
    QWidget* rate_equiv_container = nullptr;
    QWidget* rate_real_container = nullptr;
    QLabel* rate_equiv_label = nullptr;
    QLineEdit* rate_equiv_i = nullptr;
    QLineEdit* rate_equiv_current_n = nullptr;
    QLineEdit* rate_equiv_target_n = nullptr;
    HistoryContainer* rate_equiv_result = nullptr;
    QComboBox* rate_real_calc_type = nullptr;
    QLabel* rate_real_label = nullptr;
    QLineEdit* rate_real_i = nullptr;
    QLineEdit* rate_real_inflation = nullptr;
    QLineEdit* rate_real_r = nullptr;
    HistoryContainer* rate_real_result = nullptr;

    // Tab 5: Amortization
    QComboBox* amort_system = nullptr;
    QComboBox* amort_layout_mode = nullptr;
    QSplitter* amort_splitter = nullptr;
    QLineEdit* amort_p = nullptr;
    QLineEdit* amort_i = nullptr;
    QLineEdit* amort_n = nullptr;
    QLineEdit* amort_e = nullptr;
    QLineEdit* amort_k = nullptr;
    QLineEdit* amort_carencia = nullptr;
    QCheckBox* amort_juros_capitalizados = nullptr;
    QTableWidget* amort_table = nullptr;
    HistoryContainer* amort_result = nullptr;

    // Tab 6: Investment
    QComboBox* invest_analysis_type = nullptr;
    QLineEdit* invest_initial = nullptr;
    QLineEdit* invest_cashflow = nullptr;
    QLineEdit* invest_annual_revenue = nullptr;
    QLineEdit* invest_annual_cost = nullptr;
    QLineEdit* invest_sensitivity_variation = nullptr;
    QLineEdit* invest_tma = nullptr;
    QLineEdit* invest_n = nullptr;
    QLabel* label_cashflow = nullptr;
    QLabel* label_revenue = nullptr;
    QLabel* label_cost = nullptr;
    QLabel* label_sensitivity = nullptr;
    HistoryContainer* invest_result = nullptr;
    QPushButton* invest_calc_button = nullptr;

    // Tab 7: Depreciation
    QComboBox* deprec_method = nullptr;
    QLineEdit* deprec_p = nullptr;
    QLineEdit* deprec_vre = nullptr;
    QLineEdit* deprec_n = nullptr;
    QLineEdit* deprec_k = nullptr;
    QLabel* label_deprec_method = nullptr;
    QLabel* label_deprec_p = nullptr;
    QLabel* label_deprec_vre = nullptr;
    QLabel* label_deprec_n = nullptr;
    QLabel* label_deprec_k = nullptr;
    QPushButton* deprec_calc_button = nullptr;
    HistoryContainer* deprec_result = nullptr;

    // Tab 8: Effective Rate / TIR / TMA
    QComboBox* eff_rate_calc_mode = nullptr;
    QLineEdit* eff_rate_nominal = nullptr;
    QLineEdit* eff_rate_period_nominal = nullptr;
    QLineEdit* eff_rate_period_cap = nullptr;
    QLineEdit* eff_rate_period_target = nullptr;
    QLineEdit* adv_int_rate = nullptr;
    QLineEdit* adv_int_nominal = nullptr;
    QLineEdit* real_int_global_rate = nullptr;
    QLineEdit* real_int_inflation = nullptr;
    QLineEdit* real_int_capital = nullptr;
    QLineEdit* tma_rate = nullptr;
    QLineEdit* tma_periods = nullptr;
    QLineEdit* tma_monthly_rate = nullptr;
    QLineEdit* tma_capital = nullptr;
    QLineEdit* tax_global_real = nullptr;
    QLineEdit* tax_global_inf_m1 = nullptr;
    QLineEdit* tax_global_inf_m2 = nullptr;
    QLineEdit* tax_global_inf_m3 = nullptr;
    QLineEdit* tir_initial = nullptr;
    QLineEdit* tir_return = nullptr;
    QLineEdit* tir_periods = nullptr;
    QLineEdit* tirm_initial = nullptr;
    QLineEdit* tirm_return = nullptr;
    QLineEdit* tirm_periods = nullptr;
    QLineEdit* tirm_cap_rate = nullptr;
    HistoryContainer* eff_rate_result = nullptr;
    QPushButton* eff_rate_calc_button = nullptr;
    QList<QPair<QLabel*, const char*>> eff_rate_labels;
    QMap<QString, QWidget*> eff_rate_fields;

    // Tab 9: Minimum Return
    QLabel* label_min_return_investment = nullptr;
    QLabel* label_min_return_tma = nullptr;
    QLabel* label_min_return_periods = nullptr;
    QPushButton* min_return_calc_button = nullptr;
    QLineEdit* min_return_investment = nullptr;
    QLineEdit* min_return_tma = nullptr;
    QLineEdit* min_return_periods = nullptr;
    HistoryContainer* min_return_result = nullptr;

    // Tab 10: Fisher
    QLabel* label_fisher_calc_type = nullptr;
    QComboBox* fisher_calc_type = nullptr;
    QLineEdit* fisher_tma_real = nullptr;
    QLineEdit* fisher_tma_nominal = nullptr;
    QLineEdit* fisher_inflation = nullptr;
    QLabel* label_tma_real = nullptr;
    QLabel* label_tma_nominal = nullptr;
    QLabel* label_fisher_inflation = nullptr;
    QPushButton* fisher_calc_button = nullptr;
    HistoryContainer* fisher_result = nullptr;

    // Tab 11: VPL Tax
    QLabel* label_vpl_tax_investment = nullptr;
    QLineEdit* vpl_tax_investment = nullptr;
    QLabel* label_vpl_tax_annual_profit = nullptr;
    QLineEdit* vpl_tax_annual_profit = nullptr;
    QLabel* label_vpl_tax_useful_life = nullptr;
    QLineEdit* vpl_tax_useful_life = nullptr;
    QLabel* label_vpl_tax_irpj = nullptr;
    QLineEdit* vpl_tax_irpj = nullptr;
    QLabel* label_vpl_tax_csll = nullptr;
    QLineEdit* vpl_tax_csll = nullptr;
    QLabel* label_vpl_tax_tma = nullptr;
    QLineEdit* vpl_tax_tma = nullptr;
    QCheckBox* vpl_tax_financed = nullptr;
    QLabel* label_vpl_tax_finance_rate = nullptr;
    QLineEdit* vpl_tax_finance_rate = nullptr;
    QLabel* label_vpl_tax_finance_periods = nullptr;
    QLineEdit* vpl_tax_finance_periods = nullptr;
    QLabel* label_vpl_tax_residual_value = nullptr;
    QLineEdit* vpl_tax_residual_value = nullptr;
    QLabel* label_vpl_tax_sale_year = nullptr;
    QLineEdit* vpl_tax_sale_year = nullptr;
    QLabel* label_vpl_tax_sale_value = nullptr;
    QLineEdit* vpl_tax_sale_value = nullptr;
    QPushButton* vpl_tax_calc_button = nullptr;
    HistoryContainer* vpl_tax_result = nullptr;

    // Tab 12: CAUE
    QLabel* label_caue_asset_data = nullptr;
    QLabel* label_caue_initial_cost = nullptr;
    QLineEdit* caue_initial_cost = nullptr;
    QLabel* label_caue_tma = nullptr;
    QLineEdit* caue_tma = nullptr;
    QLabel* label_caue_max_years = nullptr;
    QLineEdit* caue_max_years = nullptr;
    QPushButton* caue_btn_generate_table = nullptr;
    QLabel* label_caue_resale_costs = nullptr;
    QTableWidget* caue_input_table = nullptr;
    QPushButton* caue_calc_button = nullptr;
    QTableWidget* caue_output_table = nullptr;
    HistoryContainer* caue_result = nullptr;
};

#endif // FCA_01_FINANCIAL_CALCULATOR_APP_HPP
