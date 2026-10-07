#include "ui_17_generate_price_table.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include <cmath>

QList<QVariantMap> FinancialCalculatorApp::generate_price_table(double p, double i, int n) {
    try {
        double saldo_devedor = p;
        double pow_val = std::pow(1.0 + i, n);
        double factor = (pow_val > 1.0) ? ((i * pow_val) / (pow_val - 1.0)) : 0.0;
        double prestacao = p * factor;

        for (int k = 1; k <= n; ++k) {
            double juros = saldo_devedor * i;
            double amortizacao = prestacao - juros;
            saldo_devedor -= amortizacao;
            set_amort_table_row(k, prestacao, juros, amortizacao, saldo_devedor);
        }

        return get_table_data(n);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar tabela Price: %1").arg(e.what()), true);
        throw;
    }
}
