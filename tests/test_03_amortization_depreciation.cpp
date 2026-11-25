#include "TestSupport.hpp"
#include "ui/ui_23_history_container.hpp"
#include "ui/ui_25_export_pdf.hpp"

#include <QCheckBox>
#include <QLineEdit>
#include <QTableWidget>
#include <cmath>

class TestAmortizationDepreciation : public QObject {
    Q_OBJECT

private slots:
    void test_all_five_amortization_systems_and_table_accounting() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(4);

        const double P = 12000.0;
        const double iPct = 1.0; // 1%
        const int n = 6;

        for (int sysIdx = 0; sysIdx <= 4; ++sysIdx) {
            app->amort_result->clear();
            app->amort_system->setCurrentIndex(sysIdx);
            app->amort_p->setText(QString::number(P));
            app->amort_i->setText(QString::number(iPct));
            app->amort_n->setText(QString::number(n));
            app->amort_e->clear();
            app->amort_k->setText("3");
            app->amort_carencia->setText("2");
            app->amort_juros_capitalizados->setChecked(false);
            app->calculate_amortization();

            QVERIFY2(app->amort_table->rowCount() == n + 1,
                     qPrintable(QString("System %1 expected %2 rows, got %3").arg(sysIdx).arg(n + 1).arg(app->amort_table->rowCount())));

            const QList<QVariantMap> rows = app->get_table_data(n);
            QCOMPARE(rows.size(), n);

            // Accounting identity in every non-grace period row: prestacao == juros + amortizacao
            for (int r = 0; r < rows.size(); ++r) {
                const double pmt = rows[r]["prestacao"].toDouble();
                const double j = rows[r]["juros"].toDouble();
                const double a = rows[r]["amortizacao"].toDouble();
                QVERIFY2(std::abs(pmt - (j + a)) < 0.05,
                         qPrintable(QString("System %1 row %2 failed P=J+A: %3 != %4+%5").arg(sysIdx).arg(r + 1).arg(pmt).arg(j).arg(a)));
            }

            // Final remaining balance must settle near zero
            const double finalBalance = rows.last()["saldo"].toDouble();
            QVERIFY2(std::abs(finalBalance) < 0.1,
                     qPrintable(QString("System %1 final balance expected 0, got %2").arg(sysIdx).arg(finalBalance)));

            // Point-query K calculation
            app->calculate_value_at_k();
            QVERIFY(app->amort_result->count() >= 2);
        }

        // Also test Hamburgues with capitalized interest during grace period
        app->amort_result->clear();
        app->amort_system->setCurrentIndex(4);
        app->amort_p->setText("10000");
        app->amort_i->setText("2");
        app->amort_n->setText("5");
        app->amort_carencia->setText("2");
        app->amort_juros_capitalizados->setChecked(true);
        app->calculate_amortization();
        const QList<QVariantMap> hRows = app->get_table_data(5);
        QVERIFY(hRows.size() == 5);
        // Capitalized during grace k=1..2 -> k=1 pmt=0, balance grows 10000 -> 10200 -> 10404
        QVERIFY2(std::abs(hRows[0]["prestacao"].toDouble()) < 0.01, "Capitalized grace row 1 pmt should be 0");
        QVERIFY2(std::abs(hRows[1]["saldo"].toDouble() - 10404.0) < 0.1, "Balance after 2 capitalized periods should be 10404.00");

        const QString tableHtml = PdfExport::amort_table_to_html(app->amort_table);
        QVERIFY(tableHtml.contains("<table") && tableHtml.contains("</table>") && tableHtml.contains("10404"));
    }

    void test_all_four_depreciation_methods() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(6);
        double val = 0.0;

        // P=10000, VRE=1000, n=5, k=1
        // 0: Linear -> D_k = (10000-1000)/5 = 1800
        app->deprec_method->setCurrentIndex(0);
        app->deprec_p->setText("10000");
        app->deprec_vre->setText("1000");
        app->deprec_n->setText("5");
        app->deprec_k->setText("1");
        app->calculate_depreciation();
        QVERIFY(app->deprec_result->toPlainText().contains("1.800,00") || app->deprec_result->toPlainText().contains("1,800.00"));

        // 1: Soma dos Digitos Decrescente -> sum=15, k=1 -> 9000 * 5 / 15 = 3000
        app->deprec_result->clear();
        app->deprec_method->setCurrentIndex(1);
        app->deprec_k->setText("1");
        app->calculate_depreciation();
        QVERIFY(TestSupport::findNumberNear(app->deprec_result->toPlainText(), "3.000,00", val) ||
                app->deprec_result->toPlainText().contains("3.000,00") ||
                app->deprec_result->toPlainText().contains("3,000.00"));

        // 2: Soma dos Digitos Crescente -> sum=15, k=1 -> 9000 * 1 / 15 = 600
        app->deprec_result->clear();
        app->deprec_method->setCurrentIndex(2);
        app->deprec_k->setText("1");
        app->calculate_depreciation();
        QVERIFY(app->deprec_result->toPlainText().contains("600,00") || app->deprec_result->toPlainText().contains("600.00"));

        // 3: Saldo Declinante -> r = 1 - (1000/10000)^(1/5) = 0.369043 -> D_1 = 3690.43
        app->deprec_result->clear();
        app->deprec_method->setCurrentIndex(3);
        app->deprec_k->setText("1");
        app->calculate_depreciation();
        QVERIFY(app->deprec_result->toPlainText().contains("3.690,4") || app->deprec_result->toPlainText().contains("3,690.4"));
    }
};

ECONOMIA_TEST_MAIN(TestAmortizationDepreciation)
#include "test_03_amortization_depreciation.moc"
