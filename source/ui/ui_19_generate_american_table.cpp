#include "ui_19_generate_american_table.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

QList<QVariantMap> FinancialCalculatorApp::generate_american_table(double p, double i, int n) {
    try {
        double saldo_devedor = p;
        double juros_periodo = p * i;

        for (int k = 1; k < n; ++k) {
            double prestacao = juros_periodo;
            double amortizacao = 0.0;
            set_amort_table_row(k, prestacao, juros_periodo, amortizacao, saldo_devedor);
        }

        double prestacao_final = juros_periodo + p;
        set_amort_table_row(n, prestacao_final, juros_periodo, p, 0.0);

        return get_table_data(n);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar tabela Sistema Americano: %1").arg(e.what()), true);
        throw;
    }
}
