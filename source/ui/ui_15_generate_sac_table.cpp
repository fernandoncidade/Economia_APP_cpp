#include "ui_15_generate_sac_table.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

QList<QVariantMap> FinancialCalculatorApp::generate_sac_table(double p, double i, int n) {
    try {
        double saldo_devedor = p;
        double amortizacao_constante = (n > 0) ? (p / n) : 0.0;

        for (int k = 1; k <= n; ++k) {
            double juros = saldo_devedor * i;
            double prestacao = amortizacao_constante + juros;
            saldo_devedor -= amortizacao_constante;
            set_amort_table_row(k, prestacao, juros, amortizacao_constante, saldo_devedor);
        }

        return get_table_data(n);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar tabela SAC: %1").arg(e.what()), true);
        throw;
    }
}
