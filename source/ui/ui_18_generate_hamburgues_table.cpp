#include "ui_18_generate_hamburgues_table.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

QList<QVariantMap> FinancialCalculatorApp::generate_hamburgues_table(double p, double i, int n, int carencia, bool capitalizar_juros) {
    try {
        double saldo_devedor = p;

        // Fase 1: Carência
        for (int k = 1; k <= carencia; ++k) {
            double juros = saldo_devedor * i;
            double prestacao = 0.0;
            double amortizacao = 0.0;
            if (capitalizar_juros) {
                prestacao = 0.0;
                amortizacao = 0.0;
                saldo_devedor = saldo_devedor + juros;
            } else {
                prestacao = juros;
                amortizacao = 0.0;
            }
            set_amort_table_row(k, prestacao, juros, amortizacao, saldo_devedor);
        }

        // Fase 2: Amortização (SAC sobre o saldo devedor ao final da carência)
        int n_amort = n - carencia;
        double amortizacao_constante = (n_amort > 0) ? (saldo_devedor / n_amort) : 0.0;

        for (int k = carencia + 1; k <= n; ++k) {
            double juros = saldo_devedor * i;
            double amortizacao = amortizacao_constante;
            double prestacao = juros + amortizacao;
            saldo_devedor -= amortizacao;
            set_amort_table_row(k, prestacao, juros, amortizacao, saldo_devedor);
        }

        return get_table_data(n);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar tabela Sistema Hamburguês: %1").arg(e.what()), true);
        throw;
    }
}
