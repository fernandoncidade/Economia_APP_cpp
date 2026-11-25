#include "sv_04_calculate_real_rate_equivalence.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_rate_equivalence(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_rate_equivalence();
    }
}

void calculate_real_rate(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_real_rate();
    }
}

static QString format_rate_equivalence_result(double i, double n1, double n2) {
    // (1+i_eq) = (1+i)^(n2/n1)
    double exponent = (n1 != 0.0) ? (n2 / n1) : 0.0;
    double i_eq = std::pow(1.0 + i, exponent) - 1.0;

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "EQUIVALÊNCIA DE TAXAS EFETIVAS") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    steps << "  (1 + i_eq) = (1 + i)^(n_2/n_1)\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  i (%1) = %2% %3 (n_1)\n").arg(QCoreApplication::translate("App", "Taxa conhecida"), TextFormat::format_currency(i * 100.0), QCoreApplication::translate("App", "ao período"));
    steps << QString("  n_1 (%1) = %2\n").arg(QCoreApplication::translate("App", "Período atual"), TextFormat::format_currency(n1, 0));
    steps << QString("  n_2 (%1) = %2\n\n").arg(QCoreApplication::translate("App", "Período desejado"), TextFormat::format_currency(n2, 0));

    steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
    auto frac_exp = TextFormat::format_fraction(TextFormat::format_currency(n2, 0), TextFormat::format_currency(n1, 0), QString("  %1: ").arg(QCoreApplication::translate("App", "Expoente")));
    steps << frac_exp[0] + "\n";
    steps << frac_exp[1] + "\n";
    steps << frac_exp[2] + "\n";
    steps << QString("  %1 = %2\n\n").arg(QCoreApplication::translate("App", "Expoente"), TextFormat::format_currency(exponent));

    QString exp_value_str = TextFormat::format_currency(exponent);
    steps << QString("  (1 + i_eq) = (1 + %1)^%2\n\n").arg(TextFormat::format_currency(i), exp_value_str);

    double pow_val = std::pow(1.0 + i, exponent);
    steps << QCoreApplication::translate("App", "Cálculo do fator:") + "\n";
    QString exp_frac_values = QString("(%1/%2)").arg(TextFormat::format_currency(n2, 0), TextFormat::format_currency(n1, 0));
    steps << QString("  (1 + i)^%1 = (1 + %2)^%3\n").arg(exp_frac_values, TextFormat::format_currency(i), exp_value_str);
    steps << QString("  (1 + i)^%1 = %2\n\n").arg(exp_frac_values, TextFormat::format_currency(pow_val));

    steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
    steps << QString("  i_eq = %1 - 1\n").arg(TextFormat::format_currency(pow_val));
    steps << QString("  i_eq = %1\n").arg(TextFormat::format_currency(i_eq));
    steps << QString("  i_eq = %1%\n\n").arg(TextFormat::format_currency(i_eq * 100.0));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("%1 %2% %3 (n_2)\n").arg(QCoreApplication::translate("App", "RESPOSTA: A taxa equivalente é"), TextFormat::format_currency(i_eq * 100.0), QCoreApplication::translate("App", "ao período"));
    steps << QString(60, QChar(0x2500)) + "\n";

    return steps.join("");
}

static QString format_real_rate_result(bool calc_apparent, double r, double i, double inflation) {
    if (calc_apparent) {
        // 1+i = (1+r)*(1+inflation)
        double app_i = (1.0 + r) * (1.0 + inflation) - 1.0;

        QStringList steps;
        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "CÁLCULO DA TAXA APARENTE (i)") + "\n";
        steps << QString(60, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
        steps << "  1 + i = (1 + r) × (1 + θ)\n\n";

        steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
        steps << QString("  r (%1)      = %2%\n").arg(QCoreApplication::translate("App", "Taxa real"), TextFormat::format_currency(r * 100.0));
        steps << QString("  θ (%1)       = %2%\n\n").arg(QCoreApplication::translate("App", "Inflação"), TextFormat::format_currency(inflation * 100.0));

        steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
        steps << QString("  1 + i = (1 + %1) × (1 + %2)\n").arg(TextFormat::format_currency(r), TextFormat::format_currency(inflation));
        steps << QString("  1 + i = %1 × %2\n\n").arg(TextFormat::format_currency(1.0 + r), TextFormat::format_currency(1.0 + inflation));

        double prod = (1.0 + r) * (1.0 + inflation);
        steps << QCoreApplication::translate("App", "Cálculo intermediário:") + "\n";
        steps << QString("  (1 + r) × (1 + θ) = %1\n\n").arg(TextFormat::format_currency(prod));

        steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
        steps << QString("  i = %1 - 1\n").arg(TextFormat::format_currency(prod));
        steps << QString("  i = %1\n").arg(TextFormat::format_currency(app_i));
        steps << QString("  i = %1%\n\n").arg(TextFormat::format_currency(app_i * 100.0));

        steps << QString(60, QChar(0x2500)) + "\n";
        steps << QString("%1 %2%\n").arg(QCoreApplication::translate("App", "RESPOSTA: A taxa aparente é"), TextFormat::format_currency(app_i * 100.0));
        steps << QString(60, QChar(0x2500)) + "\n";

        return steps.join("");
    } else { // Calcular Taxa Real
        // 1+r = (1+i)/(1+inflation)
        double real_r = ((1.0 + inflation) != 0.0) ? ((1.0 + i) / (1.0 + inflation) - 1.0) : 0.0;

        QStringList steps;
        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "CÁLCULO DA TAXA REAL (r)") + "\n";
        steps << QString(60, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
        auto frac = TextFormat::format_fraction("(1 + i)", "(1 + θ)", "  1 + r = ");
        steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

        steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
        steps << QString("  i (%1)  = %2%\n").arg(QCoreApplication::translate("App", "Taxa aparente"), TextFormat::format_currency(i * 100.0));
        steps << QString("  θ (%1)       = %2%\n\n").arg(QCoreApplication::translate("App", "Inflação"), TextFormat::format_currency(inflation * 100.0));

        steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
        steps << QString("  1 + r = (1 + %1) / (1 + %2)\n").arg(TextFormat::format_currency(i), TextFormat::format_currency(inflation));
        steps << QString("  1 + r = %1 / %2\n\n").arg(TextFormat::format_currency(1.0 + i), TextFormat::format_currency(1.0 + inflation));

        double div = ((1.0 + inflation) != 0.0) ? ((1.0 + i) / (1.0 + inflation)) : 0.0;
        steps << QCoreApplication::translate("App", "Cálculo intermediário:") + "\n";
        steps << QString("  (1 + i) / (1 + θ) = %1\n\n").arg(TextFormat::format_currency(div));

        steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
        steps << QString("  r = %1 - 1\n").arg(TextFormat::format_currency(div));
        steps << QString("  r = %1\n").arg(TextFormat::format_currency(real_r));
        steps << QString("  r = %1%\n\n").arg(TextFormat::format_currency(real_r * 100.0));

        steps << QString(60, QChar(0x2500)) + "\n";
        steps << QString("%1 %2%\n").arg(QCoreApplication::translate("App", "RESPOSTA: A taxa real é"), TextFormat::format_currency(real_r * 100.0));
        steps << QString(60, QChar(0x2500)) + "\n";

        return steps.join("");
    }
}

void FinancialCalculatorApp::calculate_rate_equivalence() {
    try {
        double i = get_float_from_line_edit(rate_equiv_i, true);
        double n1 = get_float_from_line_edit(rate_equiv_current_n);
        double n2 = get_float_from_line_edit(rate_equiv_target_n);

        QString result_text = format_rate_equivalence_result(i, n1, n2);

        if (rate_equiv_result) {
            auto retrans_fn = [i, n1, n2]() -> QString {
                return format_rate_equivalence_result(i, n1, n2);
            };
            rate_equiv_result->append(result_text, nullptr, retrans_fn);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular equivalência de taxas: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (rate_equiv_result) {
            rate_equiv_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}

void FinancialCalculatorApp::calculate_real_rate() {
    try {
        bool calc_apparent = rate_real_calc_type->currentIndex() == 0; // 0 = Calcular Taxa Aparente (i), 1 = Calcular Taxa Real (r)
        double r = 0.0;
        double i = 0.0;
        double inflation = get_float_from_line_edit(rate_real_inflation, true);

        if (calc_apparent) {
            r = get_float_from_line_edit(rate_real_r, true);
        } else {
            i = get_float_from_line_edit(rate_real_i, true);
        }

        QString result_text = format_real_rate_result(calc_apparent, r, i, inflation);

        if (rate_real_result) {
            auto retrans_fn = [calc_apparent, r, i, inflation]() -> QString {
                return format_real_rate_result(calc_apparent, r, i, inflation);
            };
            rate_real_result->append(result_text, nullptr, retrans_fn);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular taxa aparente/real: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (rate_real_result) {
            rate_real_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
