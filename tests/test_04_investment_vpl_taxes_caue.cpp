#include "TestSupport.hpp"
#include "ui/ui_23_history_container.hpp"

#include <QLineEdit>
#include <QTableWidget>
#include <cmath>

class TestInvestmentVplTaxesCaue : public QObject {
    Q_OBJECT

private slots:
    void test_investment_vpl_vaue_payback_and_sensitivity() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(5);

        // Mode 0: VPL & VAUE uniform series: I=10000, PMT=4000, TMA=10%, n=4
        // VPL = -10000 + 4000 * (1 - 1.1^-4)/0.1 = -10000 + 12679.46 = +2679.46
        app->invest_analysis_type->setCurrentIndex(0);
        app->invest_initial->setText("10000");
        app->invest_cashflow->setText("4000");
        app->invest_tma->setText("10");
        app->invest_n->setText("4");
        app->calculate_investment();
        QVERIFY(app->invest_result->toPlainText().contains("2.679,46") || app->invest_result->toPlainText().contains("2,679.46"));

        // Mode 1: Detailed (revenue=6000, cost=2000 -> net=4000 -> VPL=+2679.46)
        app->invest_result->clear();
        app->invest_analysis_type->setCurrentIndex(1);
        app->invest_annual_revenue->setText("6000");
        app->invest_annual_cost->setText("2000");
        app->calculate_investment();
        QVERIFY(app->invest_result->toPlainText().contains("2.679,46") || app->invest_result->toPlainText().contains("2,679.46"));

        // Mode 2: Payback descontado: I=1000, CF=400, TMA=10%, n=4 -> payback in period 3
        app->invest_result->clear();
        app->invest_analysis_type->setCurrentIndex(2);
        app->invest_initial->setText("1000");
        app->invest_cashflow->setText("400");
        app->invest_tma->setText("10");
        app->invest_n->setText("4");
        app->calculate_investment();
        QVERIFY(app->invest_result->count() == 1);
        QVERIFY(!app->invest_result->toPlainText().trimmed().isEmpty());

        // Mode 3: Sensitivity variation 20%
        app->invest_result->clear();
        app->invest_analysis_type->setCurrentIndex(3);
        app->invest_sensitivity_variation->setText("20");
        app->calculate_investment();
        QVERIFY(app->invest_result->count() == 1);
        QVERIFY(!app->invest_result->toPlainText().trimmed().isEmpty());
    }

    void test_vpl_with_taxes_and_financing() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(10);

        app->vpl_tax_investment->setText("100000");
        app->vpl_tax_annual_profit->setText("35000");
        app->vpl_tax_useful_life->setText("5");
        app->vpl_tax_irpj->setText("15");
        app->vpl_tax_csll->setText("9");
        app->vpl_tax_tma->setText("12");
        app->vpl_tax_financed->setText("40000");
        app->vpl_tax_finance_rate->setText("10");
        app->vpl_tax_finance_periods->setText("4");
        app->vpl_tax_residual_value->setText("10000");
        app->vpl_tax_sale_year->setText("5");
        app->vpl_tax_sale_value->setText("15000");
        app->calculate_vpl_with_taxes();

        QVERIFY(app->vpl_tax_result->count() == 1);
        double lastVal = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->vpl_tax_result->toPlainText(), lastVal));
        QVERIFY(std::isfinite(lastVal));
    }

    void test_caue_economic_life_and_header_subscripts() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(11);

        // Regression test: column headers must use Unicode subscript n, not literal "_n"
        const QString h1 = app->caue_input_table->horizontalHeaderItem(1)->text();
        const QString h2 = app->caue_input_table->horizontalHeaderItem(2)->text();
        const QString outH1 = app->caue_output_table->horizontalHeaderItem(1)->text();
        const QString outH2 = app->caue_output_table->horizontalHeaderItem(2)->text();
        const QString outH3 = app->caue_output_table->horizontalHeaderItem(3)->text();

        QVERIFY2(h1.contains(QChar(0x2099)) && !h1.contains("_n"), qPrintable(QString("VR header broken: %1").arg(h1)));
        QVERIFY2(h2.contains(QChar(0x2099)) && !h2.contains("_n"), qPrintable(QString("Com header broken: %1").arg(h2)));
        QVERIFY2(outH1.contains(QChar(0x2099)) && !outH1.contains("_n"), qPrintable(outH1));
        QVERIFY2(outH2.contains(QChar(0x2099)) && !outH2.contains("_n"), qPrintable(outH2));
        QVERIFY2(outH3.contains(QChar(0x2099)) && !outH3.contains("_n"), qPrintable(outH3));

        app->caue_initial_cost->setText("50000");
        app->caue_tma->setText("12");
        app->caue_max_years->setText("4");
        app->generate_caue_input_table();
        QCOMPARE(app->caue_input_table->rowCount(), 4);

        // Populate custom resale & O&M costs whose economic minimum occurs clearly at Year 3
        const QList<QPair<double, double>> rows = {
            {35000.0, 4000.0},
            {26000.0, 5000.0},
            {20000.0, 6500.0},
            {10000.0, 18000.0}
        };
        for (int r = 0; r < rows.size(); ++r) {
            app->caue_input_table->item(r, 1)->setText(QString::number(rows[r].first));
            app->caue_input_table->item(r, 2)->setText(QString::number(rows[r].second));
        }

        app->calculate_caue();
        const auto embeddedTables = app->caue_result->findChildren<QTableWidget*>();
        QVERIFY(!embeddedTables.isEmpty());
        QCOMPARE(embeddedTables[0]->rowCount(), 4);
        QCOMPARE(app->caue_output_table->rowCount(), 4);
        QVERIFY(app->caue_result->count() == 1);
    }
};

ECONOMIA_TEST_MAIN(TestInvestmentVplTaxesCaue)
#include "test_04_investment_vpl_taxes_caue.moc"
