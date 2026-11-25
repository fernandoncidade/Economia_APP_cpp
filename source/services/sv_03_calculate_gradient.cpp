#include "sv_03_calculate_gradient.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <functional>

void calculate_gradient(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_gradient();
    }
}

static QString format_gradient_result(double p, double a, double g, double i, double n, int k, int calc_mode_index, bool is_arithmetic) {
    QString result_text;

    // Modo 3: Calcular G_k do Gradiente Aritmético a partir de P
    if (calc_mode_index == 3) {
        QStringList steps;
        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "GRADIENTE ARITMÉTICO - CÁLCULO DO TERMO G_k") + "\n";
        steps << QString(60, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
        steps << QString("  P (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Valor Presente"), TextFormat::format_currency(p, 2));
        steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
        steps << QString("  n (%1)       = %2\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));
        steps << QString("  k (%1) = %2\n\n").arg(QCoreApplication::translate("App", "Termo desejado"), TextFormat::format_currency(k, 0));

        // Passo 1: Calcular G (o incremento do gradiente)
        steps << QString(60, QChar(0x2500)) + "\n";
        steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO INCREMENTO G"));
        steps << QString(60, QChar(0x2500)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula para encontrar G a partir de P:") + "\n";
        steps << "  G = P × (1+i)ⁿ / [((1+i)ⁿ-1)/i² - n/i]\n\n";

        double pow_val = std::pow(1.0 + i, n);
        QString n_super = TextFormat::to_superscript(static_cast<int>(n));

        steps << QCoreApplication::translate("App", "Cálculo de (1+i)ⁿ:") + "\n";
        steps << QString("  (1+i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), n_super);
        steps << QString("  (1+i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val, 6));

        double numerator_fraction = (i != 0.0) ? ((pow_val - 1.0) / (i * i)) : 0.0;
        double denominator_fraction = (i != 0.0) ? (n / i) : 0.0;

        steps << QCoreApplication::translate("App", "Cálculo do denominador:") + "\n";
        steps << QString("  ((1+i)ⁿ-1)/i² = (%1 - 1) / %2\n").arg(TextFormat::format_currency(pow_val, 6), TextFormat::format_currency(i * i, 6));
        steps << QString("                = %1 / %2\n").arg(TextFormat::format_currency(pow_val - 1.0, 6), TextFormat::format_currency(i * i, 6));
        steps << QString("                = %1\n\n").arg(TextFormat::format_currency(numerator_fraction, 6));

        steps << QString("  n/i = %1 / %2\n").arg(TextFormat::format_currency(n, 0), TextFormat::format_currency(i, 6));
        steps << QString("      = %1\n\n").arg(TextFormat::format_currency(denominator_fraction, 6));

        double denominator = numerator_fraction - denominator_fraction;
        steps << QString("  %1 = %2 - %3\n").arg(QCoreApplication::translate("App", "Denominador total"), TextFormat::format_currency(numerator_fraction, 6), TextFormat::format_currency(denominator_fraction, 6));
        steps << QString("                    = %1\n\n").arg(TextFormat::format_currency(denominator, 6));

        double g_calc = (denominator != 0.0) ? (p * pow_val / denominator) : 0.0;

        steps << QCoreApplication::translate("App", "Cálculo de G:") + "\n";
        steps << QString("  G = %1 × %2 / %3\n").arg(TextFormat::format_currency(p, 2), TextFormat::format_currency(pow_val, 6), TextFormat::format_currency(denominator, 6));
        steps << QString("  G = %1 / %2\n").arg(TextFormat::format_currency(p * pow_val, 2), TextFormat::format_currency(denominator, 6));
        steps << QString("  G = R$ %1\n\n").arg(TextFormat::format_currency(g_calc, 2));

        // Passo 2: Calcular G_k
        steps << QString(60, QChar(0x2500)) + "\n";
        steps << QString("2. %1 G%2\n").arg(QCoreApplication::translate("App", "CÁLCULO DO TERMO"), TextFormat::to_subscript(k));
        steps << QString(60, QChar(0x2500)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
        steps << QString("  G%1 = (k - 1) × G\n\n").arg(TextFormat::to_subscript("k"));

        steps << QCoreApplication::translate("App", "Observação: A série em gradiente aritmético padrão é:") + "\n";
        steps << QString("  %1 1: 0\n").arg(QCoreApplication::translate("App", "Período"));
        steps << QString("  %1 2: G\n").arg(QCoreApplication::translate("App", "Período"));
        steps << QString("  %1 3: 2G\n").arg(QCoreApplication::translate("App", "Período"));
        steps << QString("  %1 k: (k-1)G\n\n").arg(QCoreApplication::translate("App", "Período"));

        double g_k = (k - 1) * g_calc;

        steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
        steps << QString("  G%1 = (%2 - 1) × %3\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(k, 0), TextFormat::format_currency(g_calc, 2));
        steps << QString("  G%1 = %2 × %3\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(k - 1, 0), TextFormat::format_currency(g_calc, 2));
        steps << QString("  G%1 = R$ %2\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(g_k, 2));

        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
        steps << QString("  %1 = R$ %2\n").arg(QCoreApplication::translate("App", "G (incremento)"), TextFormat::format_currency(g_calc, 2));
        steps << QString("  G%1 = R$ %2\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(g_k, 2));
        steps << QString(60, QChar(0x2550)) + "\n";

        result_text = steps.join("");

    } else if (calc_mode_index == 2) { // Renda Perpétua
        double p_calc = (i != 0.0) ? (a / i) : 0.0;

        QStringList steps;
        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "RENDA PERPÉTUA (SÉRIE PERPÉTUA)") + "\n";
        steps << QString(60, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
        auto f = TextFormat::format_fraction("A", "i", "  P = ");
        steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

        steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
        steps << QString("  A (%1)   = R$ %2\n").arg(QCoreApplication::translate("App", "Renda mensal"), TextFormat::format_currency(a, 2));
        steps << QString("  i (%1)           = %2% %3\n\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));

        steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
        steps << QString("  %1 R$ %2 %3\n").arg(QCoreApplication::translate("App", "Para gerar juros perpétuos de"), TextFormat::format_currency(a, 2), QCoreApplication::translate("App", "por período,"));
        steps << QString("  %1 P × i = A\n\n").arg(QCoreApplication::translate("App", "o principal P deve ser tal que:"));

        steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
        auto c = TextFormat::format_fraction(TextFormat::format_currency(a, 2), TextFormat::format_currency(i, 6), "  P = ");
        steps << c[0] + "\n" << c[1] + "\n" << c[2] + "\n";
        steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc, 2));

        steps << QString(60, QChar(0x2500)) + "\n";
        steps << QCoreApplication::translate("App", "RESPOSTA: O capital necessário é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc, 2));
        steps << QString(60, QChar(0x2500)) + "\n";

        result_text = steps.join("");

    } else if (calc_mode_index == 1 && !is_arithmetic) { // Calcular X_k do gradiente geométrico
        QStringList steps;
        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "GRADIENTE GEOMÉTRICO - CÁLCULO DO k-ÉSIMO TERMO") + "\n";
        steps << QString(60, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
        steps << QString("  P (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Valor Presente"), TextFormat::format_currency(p, 2));
        steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
        steps << QString("  g (%1)    = %2% %3\n").arg(QCoreApplication::translate("App", "Crescimento"), TextFormat::format_currency(g * 100.0, 2), QCoreApplication::translate("App", "ao período"));
        steps << QString("  n (%1)       = %2\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));
        steps << QString("  k (%1) = %2\n\n").arg(QCoreApplication::translate("App", "Termo desejado"), TextFormat::format_currency(k, 0));

        double x1 = 0.0;

        // Passo 1: Calcular X_1
        if (std::abs(i - g) < 1e-10) {  // i == g
            x1 = (n != 0.0) ? ((p * (1.0 + i)) / n) : 0.0;

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("1. %1 (X%2)\n").arg(QCoreApplication::translate("App", "CÁLCULO DO PRIMEIRO TERMO"), TextFormat::to_subscript(1));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Como g = i, usa-se a fórmula simplificada:") + "\n";
            steps << QString("  P = X%1 × n / (1 + i)\n\n").arg(TextFormat::to_subscript(1));

            steps << QString("%1 X%2:\n").arg(QCoreApplication::translate("App", "Isolando"), TextFormat::to_subscript(1));
            steps << QString("  X%1 = P × (1 + i) / n\n\n").arg(TextFormat::to_subscript(1));

            steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
            steps << QString("  X%1 = %2 × %3 / %4\n").arg(TextFormat::to_subscript(1), TextFormat::format_currency(p, 2), TextFormat::format_currency(1.0 + i, 6), TextFormat::format_currency(n, 0));
            steps << QString("  X%1 = %2 / %3\n").arg(TextFormat::to_subscript(1), TextFormat::format_currency(p * (1.0 + i), 2), TextFormat::format_currency(n, 0));
            steps << QString("  X%1 = R$ %2\n\n").arg(TextFormat::to_subscript(1), TextFormat::format_currency(x1, 2));

        } else {  // i != g
            double num = g - i;
            double r = (1.0 + g) / (1.0 + i);
            double rn = std::pow(r, n);
            double den = rn - 1.0;
            x1 = (den != 0.0) ? (p * (num / den)) : 0.0;

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("1. %1 (X%2)\n").arg(QCoreApplication::translate("App", "CÁLCULO DO PRIMEIRO TERMO"), TextFormat::to_subscript(1));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Como g ≠ i, usa-se a fórmula:") + "\n";
            auto n_frac = TextFormat::format_fraction("g - i", "((1+g)/(1+i))ⁿ - 1", QString("  X%1 = P × ").arg(TextFormat::to_subscript(1)));
            steps << n_frac[0] + "\n" << n_frac[1] + "\n" << n_frac[2] + "\n\n";

            QString n_super = TextFormat::to_superscript(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Cálculo da razão r:") + "\n";
            steps << "  r = (1 + g) / (1 + i)\n";
            steps << QString("  r = %1 / %2\n").arg(TextFormat::format_currency(1.0 + g, 6), TextFormat::format_currency(1.0 + i, 6));
            steps << QString("  r = %1\n\n").arg(TextFormat::format_currency(r, 6));

            steps << QCoreApplication::translate("App", "Cálculo de rⁿ:") + "\n";
            steps << QString("  rⁿ = %1%2\n").arg(TextFormat::format_currency(r, 6), n_super);
            steps << QString("  rⁿ = %1\n\n").arg(TextFormat::format_currency(rn, 6));

            steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
            steps << QString("    g - i = %1 - %2\n").arg(TextFormat::format_currency(g, 6), TextFormat::format_currency(i, 6));
            steps << QString("    g - i = %1\n\n").arg(TextFormat::format_currency(num, 6));

            steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
            steps << QString("    rⁿ - 1 = %1 - 1\n").arg(TextFormat::format_currency(rn, 6));
            steps << QString("    rⁿ - 1 = %1\n\n").arg(TextFormat::format_currency(den, 6));

            steps << QString("%1 X%2:\n").arg(QCoreApplication::translate("App", "Cálculo de"), TextFormat::to_subscript(1));
            auto c = TextFormat::format_fraction(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), QString("  X%1 = P × ").arg(TextFormat::to_subscript(1)));
            steps << c[0] + "\n" << c[1] + "\n" << c[2] + "\n";
            steps << QString("  X%1 = %2 × %3\n").arg(TextFormat::to_subscript(1), TextFormat::format_currency(p, 2), TextFormat::format_currency(num / den, 6));
            steps << QString("  X%1 = R$ %2\n\n").arg(TextFormat::to_subscript(1), TextFormat::format_currency(x1, 2));
        }

        // Passo 2: Calcular X_k
        double xk = x1 * std::pow(1.0 + g, k - 1);

        steps << QString(60, QChar(0x2500)) + "\n";
        steps << QString("2. %1 X%2\n").arg(QCoreApplication::translate("App", "CÁLCULO DO TERMO"), TextFormat::to_subscript(k));
        steps << QString(60, QChar(0x2500)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
        steps << QString("  X%1 = X%2 × (1 + g)%3\n\n").arg(TextFormat::to_subscript("k"), TextFormat::to_subscript(1), TextFormat::to_superscript_parens("k-1"));

        QString k_minus_1_super = TextFormat::to_superscript_parens(k - 1);
        double pow_g = std::pow(1.0 + g, k - 1);

        steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
        steps << QString("  X%1 = %2 × (1 + %3)%4\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(x1, 2), TextFormat::format_currency(g, 6), k_minus_1_super);
        steps << QString("  X%1 = %2 × %3\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(x1, 2), TextFormat::format_currency(pow_g, 6));
        steps << QString("  X%1 = R$ %2\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(xk, 2));

        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
        steps << QString("  X%1 = R$ %2\n").arg(TextFormat::to_subscript(1), TextFormat::format_currency(x1, 2));
        steps << QString("  X%1 = R$ %2\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(xk, 2));
        steps << QString(60, QChar(0x2550)) + "\n";

        result_text = steps.join("");

    } else if (calc_mode_index == 0) { // Calcular P
        if (is_arithmetic) {
            double pow_val = std::pow(1.0 + i, n);
            double num_pa = pow_val - 1.0;
            double den_pa = i * pow_val;
            double factor_pa = (den_pa != 0.0) ? (num_pa / den_pa) : 0.0;
            double factor_pf = (pow_val != 0.0) ? (1.0 / pow_val) : 0.0;
            double p_calc = (i != 0.0) ? ((g / i) * (factor_pa - n * factor_pf)) : 0.0;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "GRADIENTE ARITMÉTICO - CÁLCULO DO VALOR PRESENTE (P)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto n_frac = TextFormat::format_fraction("G", "i", "  P = ");
            steps << n_frac[0] + "\n";
            steps << n_frac[1] + " × [(P/A, i, n) - n × (P/F, i, n)]\n";
            steps << n_frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  G (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Gradiente"), TextFormat::format_currency(g, 2));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            QString n_super = TextFormat::to_superscript(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Cálculo dos fatores:") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i, 6), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val, 6));

            steps << "  " + QCoreApplication::translate("App", "Fator (P/A, i, n):") + "\n";
            steps << QString("    %1   = (1 + i)ⁿ - 1 = %2 - 1\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(pow_val, 6));
            steps << QString("                = %1\n").arg(TextFormat::format_currency(num_pa, 6));
            steps << QString("    %1 = i × (1 + i)ⁿ = %2 × %3\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_val, 6));
            steps << QString("                = %1\n").arg(TextFormat::format_currency(den_pa, 6));
            steps << QString("    (P/A) = %1 / %2 = %3\n\n").arg(TextFormat::format_currency(num_pa, 6), TextFormat::format_currency(den_pa, 6), TextFormat::format_currency(factor_pa, 6));

            steps << "  " + QCoreApplication::translate("App", "Fator (P/F, i, n):") + "\n";
            steps << "    (P/F) = 1 / (1 + i)ⁿ\n";
            steps << QString("    (P/F) = 1 / %1\n").arg(TextFormat::format_currency(pow_val, 6));
            steps << QString("    (P/F) = %1\n\n").arg(TextFormat::format_currency(factor_pf, 6));

            steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
            steps << QString("  G/i = %1 / %2\n").arg(TextFormat::format_currency(g, 2), TextFormat::format_currency(i, 6));
            steps << QString("  G/i = %1\n\n").arg(TextFormat::format_currency((i != 0.0) ? (g / i) : 0.0, 2));

            double term = factor_pa - n * factor_pf;
            steps << QString("  [(P/A) - n × (P/F)] = %1 - %2 × %3\n").arg(TextFormat::format_currency(factor_pa, 6), TextFormat::format_currency(n, 0), TextFormat::format_currency(factor_pf, 6));
            steps << QString("                      = %1\n").arg(TextFormat::format_currency(factor_pa - n * factor_pf, 6));
            steps << QString("                      = %1\n\n").arg(TextFormat::format_currency(term, 6));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  P = %1 × %2\n").arg(TextFormat::format_currency((i != 0.0) ? (g / i) : 0.0, 2), TextFormat::format_currency(term, 6));
            steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc, 2));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O valor presente é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc, 2));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        } else { // Geométrico
            double x1_placeholder = 1.0;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "GRADIENTE GEOMÉTRICO - CÁLCULO DO VALOR PRESENTE (P)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            if (std::abs(i - g) < 1e-10) {
                double p_calc = x1_placeholder * n / (1.0 + i);

                steps << QCoreApplication::translate("App", "Fórmula (caso especial i = g):") + "\n";
                auto f = TextFormat::format_fraction("n", "1 + i", QString("  P = X%1 × ").arg(TextFormat::to_subscript(1)));
                steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

                steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
                steps << QString("  X%1 (%2)    = R$ %3\n").arg(TextFormat::to_subscript(1), QCoreApplication::translate("App", "1ª parcela"), TextFormat::format_currency(x1_placeholder, 2));
                steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
                steps << QString("  g (%1)    = %2% %3\n").arg(QCoreApplication::translate("App", "Crescimento"), TextFormat::format_currency(g * 100.0, 2), QCoreApplication::translate("App", "ao período"));
                steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

                steps << QCoreApplication::translate("App", "Observação: Como i = g, usa-se a fórmula simplificada.") + "\n\n";

                steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
                steps << QString("  P = %1 × %2 / (1 + %3)\n").arg(TextFormat::format_currency(x1_placeholder, 2), TextFormat::format_currency(n, 0), TextFormat::format_currency(i, 6));
                steps << QString("  P = %1 / %2\n\n").arg(TextFormat::format_currency(x1_placeholder * n, 2), TextFormat::format_currency(1.0 + i, 6));

                steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
                steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc, 2));

                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "RESPOSTA: O valor presente é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc, 2));
                steps << QString(60, QChar(0x2500)) + "\n";

                result_text = steps.join("");
            } else {
                double r = (1.0 + g) / (1.0 + i);
                double rn = std::pow(r, n);
                double num = 1.0 - rn;
                double den = i - g;
                double p_calc = (den != 0.0) ? (x1_placeholder * num / den) : 0.0;

                steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
                QString num_str = QString("1 - ((1 + g)/(1 + i))%1").arg(TextFormat::to_superscript(static_cast<int>(n)));
                auto f = TextFormat::format_fraction(num_str, "i - g", QString("  P = X%1 × ").arg(TextFormat::to_subscript(1)));
                steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

                steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
                steps << QString("  X%1 (%2)    = R$ %3\n").arg(TextFormat::to_subscript(1), QCoreApplication::translate("App", "1ª parcela"), TextFormat::format_currency(x1_placeholder, 2));
                steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
                steps << QString("  g (%1)    = %2% %3\n").arg(QCoreApplication::translate("App", "Crescimento"), TextFormat::format_currency(g * 100.0, 2), QCoreApplication::translate("App", "ao período"));
                steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

                QString n_super = TextFormat::to_superscript(static_cast<int>(n));

                steps << QCoreApplication::translate("App", "Cálculo da razão r:") + "\n";
                steps << "  r = (1 + g) / (1 + i)\n";
                steps << QString("  r = (1 + %1) / (1 + %2)\n").arg(TextFormat::format_currency(g, 6), TextFormat::format_currency(i, 6));
                steps << QString("  r = %1 / %2\n").arg(TextFormat::format_currency(1.0 + g, 6), TextFormat::format_currency(1.0 + i, 6));
                steps << QString("  r = %1\n\n").arg(TextFormat::format_currency(r, 6));

                steps << QCoreApplication::translate("App", "Cálculo de rⁿ:") + "\n";
                steps << QString("  rⁿ = %1%2\n").arg(TextFormat::format_currency(r, 6), n_super);
                steps << QString("  rⁿ = %1\n\n").arg(TextFormat::format_currency(rn, 6));

                steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
                steps << QString("    1 - rⁿ = 1 - %1\n").arg(TextFormat::format_currency(rn, 6));
                steps << QString("    1 - rⁿ = %1\n\n").arg(TextFormat::format_currency(num, 6));

                steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
                steps << QString("    i - g = %1 - %2\n").arg(TextFormat::format_currency(i, 6), TextFormat::format_currency(g, 6));
                steps << QString("    i - g = %1\n\n").arg(TextFormat::format_currency(den, 6));

                steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
                steps << QString("  P = %1 × %2 / %3\n").arg(TextFormat::format_currency(x1_placeholder, 2), TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6));
                steps << QString("  P = %1 × %2\n").arg(TextFormat::format_currency(x1_placeholder, 2), TextFormat::format_currency(num / den, 6));
                steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc, 2));

                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "RESPOSTA: O valor presente é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc, 2));
                steps << QString(60, QChar(0x2500)) + "\n";

                result_text = steps.join("");
            }
        }
    }

    return result_text;
}

void FinancialCalculatorApp::calculate_gradient() {
    try {
        int calc_mode_index = grad_calc_mode->currentIndex();
        bool is_arithmetic = grad_type->currentIndex() == 0;

        double p = 0.0;
        double a = 0.0;
        double g = 0.0;
        double i = 0.0;
        double n = 0.0;
        int k = 1;

        if (calc_mode_index == 3) { // Calcular Termo G_k do Gradiente Aritmético
            p = get_float_from_line_edit(grad_p);
            i = get_float_from_line_edit(grad_i, true);
            n = get_float_from_line_edit(grad_n);
            k = static_cast<int>(get_float_from_line_edit(grad_k));
        } else if (calc_mode_index == 2) { // Renda Perpétua
            a = get_float_from_line_edit(grad_a);
            i = get_float_from_line_edit(grad_i, true);
        } else if (calc_mode_index == 1) { // Calcular X_k do Gradiente Geométrico
            p = get_float_from_line_edit(grad_p);
            i = get_float_from_line_edit(grad_i, true);
            g = get_float_from_line_edit(grad_g, true);
            n = get_float_from_line_edit(grad_n);
            k = static_cast<int>(get_float_from_line_edit(grad_k));
        } else { // Calcular P (modo 0)
            i = get_float_from_line_edit(grad_i, true);
            n = get_float_from_line_edit(grad_n);
            if (is_arithmetic) {
                g = get_float_from_line_edit(grad_g);
            } else {
                g = get_float_from_line_edit(grad_g, true);
            }
        }

        QString result_text = format_gradient_result(p, a, g, i, n, k, calc_mode_index, is_arithmetic);

        if (!result_text.isEmpty() && grad_result) {
            auto retrans_fn = [p, a, g, i, n, k, calc_mode_index, is_arithmetic]() -> QString {
                return format_gradient_result(p, a, g, i, n, k, calc_mode_index, is_arithmetic);
            };
            grad_result->append(result_text, nullptr, retrans_fn);
        }

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao calcular gradiente: %1").arg(e.what()));
        if (grad_result) {
            grad_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
