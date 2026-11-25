#include "TestSupport.hpp"
#include "ui/ui_23_history_container.hpp"

#include <QLabel>
#include <QLineEdit>
#include <cmath>

class TestRatesFisherMinReturnEffRate : public QObject {
    Q_OBJECT

private slots:
    void test_rate_equivalence_and_real_rate_tab() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(3);

        // 12% for 360 days -> equivalent for 30 days = (1.12^(1/12) - 1)*100 = 0.948879%
        app->rate_equiv_i->setText("12");
        app->rate_equiv_current_n->setText("360");
        app->rate_equiv_target_n->setText("30");
        app->calculate_rate_equivalence();

        double val = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->rate_equiv_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 0.9489) < 0.01, qPrintable(QString("Expected ~0.9489%, got %1").arg(val)));

        // Apparent rate from real 5% + inflation 4% -> 9.20%
        app->rate_real_calc_type->setCurrentIndex(0);
        app->rate_real_r->setText("5");
        app->rate_real_inflation->setText("4");
        app->calculate_real_rate();

        QVERIFY(TestSupport::findLastNumber(app->rate_real_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 9.20) < 0.05, qPrintable(QString("Expected 9.20%, got %1").arg(val)));

        // Real rate from apparent 9.2% + inflation 4% -> 5.00%
        app->rate_real_result->clear();
        app->rate_real_calc_type->setCurrentIndex(1);
        app->rate_real_i->setText("9.2");
        app->rate_real_inflation->setText("4");
        app->calculate_real_rate();

        QVERIFY(TestSupport::findLastNumber(app->rate_real_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 5.0) < 0.05, qPrintable(QString("Expected 5.00%, got %1").arg(val)));
    }

    void test_fisher_tab_calculations_and_subscript_labels() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(9);

        // Regression test: subscript formatting on i_r and i_a labels
        const QString lblReal = app->label_tma_real->text();
        const QString lblNom = app->label_tma_nominal->text();
        QVERIFY2(lblReal.contains("<sub>r</sub>") && !lblReal.contains("i_r"),
                 qPrintable(QString("label_tma_real subscript broken: %1").arg(lblReal)));
        QVERIFY2(lblNom.contains("<sub>a</sub>") && !lblNom.contains("i_a"),
                 qPrintable(QString("label_tma_nominal subscript broken: %1").arg(lblNom)));

        // Mode 0: TMA Real = 6%, Inflation = 4.5% -> Nominal = (1.06*1.045 - 1)*100 = 10.77%
        app->fisher_calc_type->setCurrentIndex(0);
        app->fisher_tma_real->setText("6");
        app->fisher_inflation->setText("4.5");
        app->calculate_fisher();

        double val = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->fisher_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 10.77) < 0.05, qPrintable(QString("Expected 10.77%, got %1").arg(val)));

        // Mode 1: Nominal = 10.77%, Inflation = 4.5% -> Real = 6.00%
        app->fisher_result->clear();
        app->fisher_calc_type->setCurrentIndex(1);
        app->fisher_tma_nominal->setText("10.77");
        app->fisher_inflation->setText("4.5");
        app->calculate_fisher();

        QVERIFY(TestSupport::findLastNumber(app->fisher_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 6.0) < 0.05, qPrintable(QString("Expected 6.00%, got %1").arg(val)));
    }

    void test_minimum_return_tab() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(8);

        app->min_return_investment->setText("100000");
        app->min_return_tma->setText("10");
        app->min_return_periods->setText("5");
        app->calculate_minimum_return();

        const QString out = app->min_return_result->toPlainText();
        double minReturn = 0.0;
        QVERIFY(TestSupport::findLastNumber(out, minReturn));
        // Minimum return per period = 100000 * ((1.1)^(1/5) - 1) = 1924.49
        QVERIFY2(std::abs(minReturn - 1924.49) < 0.1, qPrintable(QString("Expected 1924.49, got %1").arg(minReturn)));
    }

    void test_effective_rate_all_seven_modes() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(7);
        double val = 0.0;

        // Mode 0: Taxa Efetiva (12% nominal/12m, cap 1m, target 12m -> (1.01)^12 - 1 = 12.6825%)
        app->eff_rate_calc_mode->setCurrentIndex(0);
        app->eff_rate_nominal->setText("12");
        app->eff_rate_period_nominal->setText("12");
        app->eff_rate_period_cap->setText("1");
        app->eff_rate_period_target->setText("12");
        app->calculate_effective_rate();
        QVERIFY(TestSupport::findLastNumber(app->eff_rate_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 12.6825) < 0.05, qPrintable(QString("Mode 0 expected 12.6825%, got %1").arg(val)));

        // Mode 1: TIR (P=1000, Return=600 per period, n=2 -> -1000 + 600/(1+r) + 600/(1+r)^2 = 0 -> r = 13.0662%)
        app->eff_rate_result->clear();
        app->eff_rate_calc_mode->setCurrentIndex(1);
        app->tir_initial->setText("1000");
        app->tir_return->setText("600");
        app->tir_periods->setText("2");
        app->calculate_effective_rate();
        QVERIFY(TestSupport::findLastNumber(app->eff_rate_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 13.0662) < 0.05, qPrintable(QString("Mode 1 TIR expected 13.0662%, got %1").arg(val)));

        // Mode 2: Taxa Global (real=2%/month -> (1.02)^3-1, theta=(1.01*1.015*1.005)-1 -> i_global = 9.3337%)
        app->eff_rate_result->clear();
        app->eff_rate_calc_mode->setCurrentIndex(2);
        app->tax_global_real->setText("2");
        app->tax_global_inf_m1->setText("1");
        app->tax_global_inf_m2->setText("1.5");
        app->tax_global_inf_m3->setText("0.5");
        app->calculate_effective_rate();
        QVERIFY(TestSupport::findLastNumber(app->eff_rate_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 9.3337) < 0.05, qPrintable(QString("Mode 2 expected 9.3337%, got %1").arg(val)));

        // Mode 3: Cobranca Antecipada (nominal=1000, rate=10% -> liquid=900, eff=11.11%)
        app->eff_rate_result->clear();
        app->eff_rate_calc_mode->setCurrentIndex(3);
        app->adv_int_nominal->setText("1000");
        app->adv_int_rate->setText("10");
        app->calculate_effective_rate();
        QVERIFY(TestSupport::findLastNumber(app->eff_rate_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 11.1111) < 0.1, qPrintable(QString("Mode 3 expected 11.11%, got %1").arg(val)));

        // Mode 4: TIRm (VPC=1000, B=605/period, n=2, cap_rate=0% -> VFB=1210 -> sqrt(1210/1000)-1 = 10.00%)
        app->eff_rate_result->clear();
        app->eff_rate_calc_mode->setCurrentIndex(4);
        app->tirm_initial->setText("1000");
        app->tirm_return->setText("605");
        app->tirm_periods->setText("2");
        app->tirm_cap_rate->setText("0");
        app->calculate_effective_rate();
        QVERIFY(TestSupport::findLastNumber(app->eff_rate_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 10.0) < 0.05, qPrintable(QString("Mode 4 TIRm expected 10.00%, got %1").arg(val)));

        // Mode 5: TMA vs Rentabilidade (monthly=1% -> annual = 1.01^12 - 1 = 12.6825%)
        app->eff_rate_result->clear();
        app->eff_rate_calc_mode->setCurrentIndex(5);
        app->tma_capital->setText("1000");
        app->tma_monthly_rate->setText("1");
        app->tma_rate->setText("12");
        app->tma_periods->setText("24");
        app->calculate_effective_rate();
        QVERIFY(app->eff_rate_result->toPlainText().contains("12,68") || app->eff_rate_result->toPlainText().contains("12.68"));

        // Mode 6: Juros Reais (P=1000, global=15.5%, inflation=5% -> real=10%, juros reais=100.00)
        app->eff_rate_result->clear();
        app->eff_rate_calc_mode->setCurrentIndex(6);
        app->real_int_capital->setText("1000");
        app->real_int_global_rate->setText("15.5");
        app->real_int_inflation->setText("5");
        app->calculate_effective_rate();
        QVERIFY(TestSupport::findLastNumber(app->eff_rate_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 100.0) < 0.1, qPrintable(QString("Mode 6 expected 100.00, got %1").arg(val)));
    }
};

ECONOMIA_TEST_MAIN(TestRatesFisherMinReturnEffRate)
#include "test_02_rates_fisher_minreturn_effrate.moc"
