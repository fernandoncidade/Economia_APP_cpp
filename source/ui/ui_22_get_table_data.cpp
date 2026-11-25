#include "ui_22_get_table_data.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

#include <QTableWidgetItem>

QList<QVariantMap> FinancialCalculatorApp::get_table_data(int n) {
    try {
        QList<QVariantMap> data;
        if (!amort_table) return data;

        for (int k = 1; k <= n; ++k) {
            QVariantMap row;
            auto* it_p = amort_table->item(k, 1);
            auto* it_j = amort_table->item(k, 2);
            auto* it_a = amort_table->item(k, 3);
            auto* it_s = amort_table->item(k, 4);

            row["prestacao"] = it_p ? it_p->text().toDouble() : 0.0;
            row["juros"] = it_j ? it_j->text().toDouble() : 0.0;
            row["amortizacao"] = it_a ? it_a->text().toDouble() : 0.0;
            row["saldo"] = it_s ? it_s->text().toDouble() : 0.0;
            data.append(row);
        }
        return data;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao obter dados da tabela de amortização: %1").arg(e.what()), true);
        throw;
    }
}
