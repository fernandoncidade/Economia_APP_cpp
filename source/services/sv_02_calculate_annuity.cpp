#include "sv_02_calculate_annuity.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <functional>

void calculate_annuity(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_annuity();
    }
}

static QString format_annuity_result(double p, double a, double i, double n, bool is_postecipada, bool calc_a) {
    QString result_text;

    if (calc_a) {
        if (is_postecipada) {
            // Fator A/P
            double pow_val = std::pow(1.0 + i, n);
            double num = i * pow_val;
            double den = pow_val - 1.0;
            double factor = (den != 0.0) ? (num / den) : 0.0;
            double a_calc = p * factor;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SÉRIE UNIFORME POSTECIPADA - CÁLCULO DA PRESTAÇÃO (A)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto frac = TextFormat::format_fraction("i × (1 + i)ⁿ", "(1 + i)ⁿ - 1", "  A = P × ");
            steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Valor Presente"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            QString n_super = TextFormat::to_superscript(static_cast<int>(n));
            steps << QCoreApplication::translate("App", "Cálculo do fator (A/P):") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val, 6));

            steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
            steps << QString("    i × (1 + i)ⁿ = %1 × %2\n").arg(TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_val, 6));
            steps << QString("    i × (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(num, 6));

            steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
            steps << QString("    (1 + i)ⁿ - 1 = %1 - 1\n").arg(TextFormat::format_currency(pow_val, 6));
            steps << QString("    (1 + i)ⁿ - 1 = %1\n\n").arg(TextFormat::format_currency(den, 6));

            steps << "  " + QCoreApplication::translate("App", "Fator A/P:") + "\n";
            steps << QString("    %1 / %2 = %3\n\n").arg(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), TextFormat::format_currency(factor, 6));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  A = %1 × %2\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(factor, 6));
            steps << QString("  A = R$ %1\n\n").arg(TextFormat::format_currency(a_calc));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: A prestação é R$") + QString(" %1\n").arg(TextFormat::format_currency(a_calc));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        } else { // Antecipada
            double pow_n = std::pow(1.0 + i, n);
            double pow_nm1 = (n > 0.0) ? std::pow(1.0 + i, n - 1.0) : 1.0;
            double num = i * pow_nm1;
            double den = pow_n - 1.0;
            double factor = (den != 0.0) ? (num / den) : 0.0;
            double a_calc = p * factor;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SÉRIE UNIFORME ANTECIPADA - CÁLCULO DA PRESTAÇÃO (A')") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto frac = TextFormat::format_fraction("i × (1 + i)ⁿ⁻¹", "(1 + i)ⁿ - 1", "  A' = P × ");
            steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Valor Presente"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            QString n_super = TextFormat::to_superscript(static_cast<int>(n));
            QString nm1_super = TextFormat::to_superscript(static_cast<int>(n - 1));

            steps << QCoreApplication::translate("App", "Cálculo do fator (A'/P):") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_n, 6));

            steps << QString("  (1 + i)ⁿ⁻¹ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), nm1_super);
            steps << QString("  (1 + i)ⁿ⁻¹ = %1\n\n").arg(TextFormat::format_currency(pow_nm1, 6));

            steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
            steps << QString("    i × (1 + i)ⁿ⁻¹ = %1 × %2\n").arg(TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_nm1, 6));
            steps << QString("    i × (1 + i)ⁿ⁻¹ = %1\n\n").arg(TextFormat::format_currency(num, 6));

            steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
            steps << QString("    (1 + i)ⁿ - 1 = %1 - 1\n").arg(TextFormat::format_currency(pow_n, 6));
            steps << QString("    (1 + i)ⁿ - 1 = %1\n\n").arg(TextFormat::format_currency(den, 6));

            steps << "  " + QCoreApplication::translate("App", "Fator A'/P:") + "\n";
            steps << QString("    %1 / %2 = %3\n\n").arg(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), TextFormat::format_currency(factor, 6));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  A' = %1 × %2\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(factor, 6));
            steps << QString("  A' = R$ %1\n\n").arg(TextFormat::format_currency(a_calc));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: A prestação antecipada é R$") + QString(" %1\n").arg(TextFormat::format_currency(a_calc));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        }
    } else { // Calcular P
        if (is_postecipada) {
            double pow_val = std::pow(1.0 + i, n);
            double num = pow_val - 1.0;
            double den = i * pow_val;
            double factor = (den != 0.0) ? (num / den) : 0.0;
            double p_calc = a * factor;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SÉRIE UNIFORME POSTECIPADA - CÁLCULO DO VALOR PRESENTE (P)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto frac = TextFormat::format_fraction("(1 + i)ⁿ - 1", "i × (1 + i)ⁿ", "  P = A × ");
            steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  A (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Prestação"), TextFormat::format_currency(a));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            QString n_super = TextFormat::to_superscript(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Cálculo do fator (P/A):") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val, 6));

            steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
            steps << QString("    (1 + i)ⁿ - 1 = %1 - 1\n").arg(TextFormat::format_currency(pow_val, 6));
            steps << QString("    (1 + i)ⁿ - 1 = %1\n\n").arg(TextFormat::format_currency(num, 6));

            steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
            steps << QString("    i × (1 + i)ⁿ = %1 × %2\n").arg(TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_val, 6));
            steps << QString("    i × (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(den, 6));

            steps << "  " + QCoreApplication::translate("App", "Fator P/A:") + "\n";
            steps << QString("    %1 / %2 = %3\n\n").arg(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), TextFormat::format_currency(factor, 6));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  P = %1 × %2\n").arg(TextFormat::format_currency(a), TextFormat::format_currency(factor, 6));
            steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O valor presente é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        } else { // Antecipada
            double pow_n = std::pow(1.0 + i, n);
            double pow_nm1 = (n > 0.0) ? std::pow(1.0 + i, n - 1.0) : 1.0;
            double num = pow_n - 1.0;
            double den = i * pow_nm1;
            double factor = (den != 0.0) ? (num / den) : 0.0;
            double p_calc = a * factor;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SÉRIE UNIFORME ANTECIPADA - CÁLCULO DO VALOR PRESENTE (P)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto frac = TextFormat::format_fraction("(1 + i)ⁿ - 1", "i × (1 + i)ⁿ⁻¹", "  P = A' × ");
            steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  A' (%1)     = R$ %2\n").arg(QCoreApplication::translate("App", "Prestação"), TextFormat::format_currency(a));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            QString n_super = TextFormat::to_superscript(static_cast<int>(n));
            QString nm1_super = TextFormat::to_superscript(static_cast<int>(n - 1));

            steps << QCoreApplication::translate("App", "Cálculo do fator (P/A'):") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_n, 6));

            steps << QString("  (1 + i)ⁿ⁻¹ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), nm1_super);
            steps << QString("  (1 + i)ⁿ⁻¹ = %1\n\n").arg(TextFormat::format_currency(pow_nm1, 6));

            steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
            steps << QString("    (1 + i)ⁿ - 1 = %1 - 1\n").arg(TextFormat::format_currency(pow_n, 6));
            steps << QString("    (1 + i)ⁿ - 1 = %1\n\n").arg(TextFormat::format_currency(num, 6));

            steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
            steps << QString("    i × (1 + i)ⁿ⁻¹ = %1 × %2\n").arg(TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_nm1, 6));
            steps << QString("    i × (1 + i)ⁿ⁻¹ = %1\n\n").arg(TextFormat::format_currency(den, 6));

            steps << "  " + QCoreApplication::translate("App", "Fator P/A':") + "\n";
            steps << QString("    %1 / %2 = %3\n\n").arg(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), TextFormat::format_currency(factor, 6));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  P = %1 × %2\n").arg(TextFormat::format_currency(a), TextFormat::format_currency(factor, 6));
            steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O valor presente é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        }
    }

    return result_text;
}

void FinancialCalculatorApp::calculate_annuity() {
    try {
        double i = get_float_from_line_edit(annuity_i, true);
        double n = get_float_from_line_edit(annuity_n);

        bool is_postecipada = annuity_type->currentIndex() == 0; // 0 = Postecipada, 1 = Antecipada
        bool calc_a = annuity_calc_type->currentIndex() == 0;    // 0 = Calcular Prestação (A), 1 = Calcular P

        double p = 0.0;
        double a = 0.0;
        if (calc_a) {
            p = get_float_from_line_edit(annuity_p);
        } else {
            a = get_float_from_line_edit(annuity_a);
        }

        QString result_text = format_annuity_result(p, a, i, n, is_postecipada, calc_a);

        if (!result_text.isEmpty() && annuity_result) {
            auto retrans_fn = [p, a, i, n, is_postecipada, calc_a]() -> QString {
                return format_annuity_result(p, a, i, n, is_postecipada, calc_a);
            };
            annuity_result->append(result_text, nullptr, retrans_fn);
        }

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao calcular anuidades: %1").arg(e.what()));
        if (annuity_result) {
            annuity_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
