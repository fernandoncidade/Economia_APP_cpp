#include "ui_16_generate_sam_table.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

QList<QVariantMap> FinancialCalculatorApp::generate_sam_table(double p, double i, int n) {
    try {
        if (amort_table) amort_table->setVisible(false);
        auto sac_data = generate_sac_table(p, i, n);
        auto price_data = generate_price_table(p, i, n);
        if (amort_table) amort_table->setVisible(true);

        for (int k = 1; k <= n; ++k) {
            double prestacao = (sac_data[k - 1]["prestacao"].toDouble() + price_data[k - 1]["prestacao"].toDouble()) / 2.0;
            double juros = (sac_data[k - 1]["juros"].toDouble() + price_data[k - 1]["juros"].toDouble()) / 2.0;
            double amortizacao = (sac_data[k - 1]["amortizacao"].toDouble() + price_data[k - 1]["amortizacao"].toDouble()) / 2.0;
            double saldo_devedor = (sac_data[k - 1]["saldo"].toDouble() + price_data[k - 1]["saldo"].toDouble()) / 2.0;
            set_amort_table_row(k, prestacao, juros, amortizacao, saldo_devedor);
        }

        return get_table_data(n);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar tabela SAM: %1").arg(e.what()), true);
        throw;
    }
}
