#include "TestSupport.hpp"
#include "ui/ui_23_history_container.hpp"

#include <QLineEdit>
#include <cmath>

class TestCoreInterestAnnuityGradient : public QObject {
    Q_OBJECT

private slots:
    void test_compound_interest_future_and_present() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(0);

        // Mode 0: Montante (F), Compound regime (0)
        app->interest_calc_type->setCurrentIndex(0);
        app->interest_regime->setCurrentIndex(0);
        app->interest_p->setText("1000");
        app->interest_i->setText("10");
        app->interest_n->setText("2");
        app->calculate_interest();

        double val = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->interest_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 1210.0) < 0.05, qPrintable(QString("Expected 1210.00, got %1").arg(val)));

        // Mode 1: Capital Inicial (P)
        app->interest_result->clear();
        app->interest_calc_type->setCurrentIndex(1);
        app->interest_f->setText("1210");
        app->interest_i->setText("10");
        app->interest_n->setText("2");
        app->calculate_interest();

        QVERIFY(TestSupport::findLastNumber(app->interest_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 1000.0) < 0.05, qPrintable(QString("Expected 1000.00, got %1").arg(val)));
    }

    void test_simple_interest_and_comparison() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(0);

        // Mode 0: F, Simple regime (1): 1000 * (1 + 0.10 * 2) = 1200
        app->interest_calc_type->setCurrentIndex(0);
        app->interest_regime->setCurrentIndex(1);
        app->interest_p->setText("1000");
        app->interest_i->setText("10");
        app->interest_n->setText("2");
        app->calculate_interest();

        double fSimple = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->interest_result->toPlainText(), fSimple));
        QVERIFY2(std::abs(fSimple - 1200.0) < 0.05, qPrintable(QString("Expected 1200.00, got %1").arg(fSimple)));

        // Mode 2: Compare JS vs JC for P=1000, i=10%, n=3 -> F_c=1331.00, first integer month n_s=4 (F_s=1400.00 > 1331.00)
        app->interest_result->clear();
        app->interest_calc_type->setCurrentIndex(2);
        app->interest_p->setText("1000");
        app->interest_i->setText("10");
        app->interest_n->setText("3");
        app->calculate_interest();

        const QString out = app->interest_result->toPlainText();
        double nMonths = 0.0;
        QVERIFY(TestSupport::findLastNumber(out, nMonths));
        QVERIFY(out.contains("1.331,00") || out.contains("1,331.00"));
        QVERIFY(out.contains("1.400,00") || out.contains("1,400.00"));
        QVERIFY2(std::abs(nMonths - 4.0) < 0.05, qPrintable(QString("Expected n_s=4 months, got %1").arg(nMonths)));
    }

    void test_interest_negative_input_shows_warning_without_history() {
        auto app = TestSupport::createIsolatedApp();
        app->interest_calc_type->setCurrentIndex(0);
        app->interest_regime->setCurrentIndex(0);
        app->interest_p->clear();
        app->interest_i->setText("10");
        app->interest_n->setText("2");

        app->calculate_interest();
        QCOMPARE(app->interest_result->count(), 1);
        QVERIFY(app->interest_result->toPlainText().contains("Erro") ||
                app->interest_result->toPlainText().contains("Error"));
    }

    void test_annuities_postecipada_antecipada_and_zero_rate() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(1);

        // Postecipada: P=10000, i=2%, n=12 -> A = 945.5959...
        app->annuity_calc_type->setCurrentIndex(0);
        app->annuity_type->setCurrentIndex(0);
        app->annuity_p->setText("10000");
        app->annuity_i->setText("2");
        app->annuity_n->setText("12");
        app->calculate_annuity();

        double val = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->annuity_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 945.60) < 0.05, qPrintable(QString("Expected 945.60, got %1").arg(val)));

        // Calc P from A=945.60, i=2%, n=12 -> ~10000.04
        app->annuity_result->clear();
        app->annuity_calc_type->setCurrentIndex(1);
        app->annuity_type->setCurrentIndex(0);
        app->annuity_a->setText("945.60");
        app->annuity_i->setText("2");
        app->annuity_n->setText("12");
        app->calculate_annuity();

        QVERIFY(TestSupport::findLastNumber(app->annuity_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 10000.04) < 0.25, qPrintable(QString("Expected ~10000.04, got %1").arg(val)));

        // Antecipada: A_ant = A_post / 1.02 = 927.0548...
        app->annuity_result->clear();
        app->annuity_calc_type->setCurrentIndex(0);
        app->annuity_type->setCurrentIndex(1);
        app->annuity_p->setText("10000");
        app->annuity_i->setText("2");
        app->annuity_n->setText("12");
        app->calculate_annuity();

        QVERIFY(TestSupport::findLastNumber(app->annuity_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 927.05) < 0.05, qPrintable(QString("Expected 927.05, got %1").arg(val)));

        // Zero rate edge case: sv_02 guards den==0 by returning finite 0.00
        app->annuity_result->clear();
        app->annuity_calc_type->setCurrentIndex(0);
        app->annuity_type->setCurrentIndex(0);
        app->annuity_p->setText("1200");
        app->annuity_i->setText("0");
        app->annuity_n->setText("12");
        app->calculate_annuity();

        QVERIFY(TestSupport::findLastNumber(app->annuity_result->toPlainText(), val));
        QVERIFY(std::isfinite(val));
        QVERIFY2(std::abs(val) < 0.01, qPrintable(QString("Expected guarded 0.00 at i=0, got %1").arg(val)));
    }

    void test_gradient_arithmetic_geometric_and_perpetuity() {
        auto app = TestSupport::createIsolatedApp();
        app->tabs->setCurrentIndex(2);

        // Arithmetic G_k from P, i, n, k (mode 3): P=1000, i=10%, n=4, k=3 -> G=228.41, G_3=456.82
        app->grad_type->setCurrentIndex(0);
        app->grad_calc_mode->setCurrentIndex(3);
        app->grad_p->setText("1000");
        app->grad_i->setText("10");
        app->grad_n->setText("4");
        app->grad_k->setText("3");
        app->calculate_gradient();

        double val = 0.0;
        QVERIFY(TestSupport::findLastNumber(app->grad_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 456.82) < 0.1, qPrintable(QString("Expected 456.82, got %1").arg(val)));

        // Arithmetic Perpetuity (mode 2): P = A / i -> A=100, i=10% -> 1000.00
        app->grad_result->clear();
        app->grad_type->setCurrentIndex(0);
        app->grad_calc_mode->setCurrentIndex(2);
        app->grad_a->setText("100");
        app->grad_i->setText("10");
        app->calculate_gradient();

        QVERIFY(TestSupport::findLastNumber(app->grad_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 1000.0) < 0.05, qPrintable(QString("Expected 1000.00, got %1").arg(val)));

        // Arithmetic P (mode 0): sv_03 formula P = (G/i) * ((P/A) - n*(P/F)) -> G=20, i=10%, n=4 -> 87.6285
        app->grad_result->clear();
        app->grad_type->setCurrentIndex(0);
        app->grad_calc_mode->setCurrentIndex(0);
        app->grad_g->setText("20");
        app->grad_i->setText("10");
        app->grad_n->setText("4");
        app->calculate_gradient();

        QVERIFY(TestSupport::findLastNumber(app->grad_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 87.63) < 0.1, qPrintable(QString("Expected ~87.63, got %1").arg(val)));

        // Geometric X_k from P, i, g, n, k (mode 1): P=1000, i=10%, g=10%, n=4, k=3 -> X_1=275, X_3=275*1.1^2=332.75
        app->grad_result->clear();
        app->grad_type->setCurrentIndex(1);
        app->grad_calc_mode->setCurrentIndex(1);
        app->grad_p->setText("1000");
        app->grad_i->setText("10");
        app->grad_g->setText("10");
        app->grad_n->setText("4");
        app->grad_k->setText("3");
        app->calculate_gradient();

        QVERIFY(TestSupport::findLastNumber(app->grad_result->toPlainText(), val));
        QVERIFY2(std::abs(val - 332.75) < 0.1, qPrintable(QString("Expected 332.75, got %1").arg(val)));

        // Geometric P equal i=g limit: sv_03 uses x1=1.0 -> P = 4 / 1.1 = 3.6363...
        app->grad_result->clear();
        app->grad_type->setCurrentIndex(1);
        app->grad_calc_mode->setCurrentIndex(0);
        app->grad_g->setText("10");
        app->grad_i->setText("10");
        app->grad_n->setText("4");
        app->calculate_gradient();

        QVERIFY(TestSupport::findLastNumber(app->grad_result->toPlainText(), val));
        QVERIFY2(std::abs(val - (4.0 / 1.1)) < 0.05, qPrintable(QString("Expected 3.64, got %1").arg(val)));
    }
};

ECONOMIA_TEST_MAIN(TestCoreInterestAnnuityGradient)
#include "test_01_core_interest_annuity_gradient.moc"
