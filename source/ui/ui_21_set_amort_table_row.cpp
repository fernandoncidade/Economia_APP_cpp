#include "ui_21_set_amort_table_row.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <cmath>

void FinancialCalculatorApp::set_amort_table_row(int k, double prestacao, double juros, double amortizacao, double saldo_devedor) {
    try {
        if (!amort_table) return;

        if (k >= amort_table->rowCount()) {
            amort_table->setRowCount(k + 1);
        }

        amort_table->setItem(k, 0, new QTableWidgetItem(QString::number(k)));
        amort_table->setItem(k, 1, new QTableWidgetItem(QString::number(prestacao, 'f', 2)));
        amort_table->setItem(k, 2, new QTableWidgetItem(QString::number(juros, 'f', 2)));
        amort_table->setItem(k, 3, new QTableWidgetItem(QString::number(amortizacao, 'f', 2)));
        amort_table->setItem(k, 4, new QTableWidgetItem(QString::number(std::abs(saldo_devedor), 'f', 2)));
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao definir linha da tabela de amortização: %1").arg(e.what()), true);
        throw;
    }
}
