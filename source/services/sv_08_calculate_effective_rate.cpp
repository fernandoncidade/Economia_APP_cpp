#include "sv_08_calculate_effective_rate.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../ui/ui_23_history_container.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_effective_rate(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_effective_rate();
    }
}

double FinancialCalculatorApp::calculate_tir_newton(double initial_investment, double periodic_return, int num_periods, double initial_guess, double tolerance, int max_iterations) {
    try {
        double tir = initial_guess;
        for (int iter = 0; iter < max_iterations; ++iter) {
            double vpl = -initial_investment;
            for (int t = 1; t <= num_periods; ++t) {
                vpl += periodic_return / std::pow(1.0 + tir, t);
            }

            double dvpl = 0.0;
            for (int t = 1; t <= num_periods; ++t) {
                dvpl -= t * periodic_return / std::pow(1.0 + tir, t + 1);
            }

            if (std::abs(dvpl) < 1e-10) {
                break;
            }

            double tir_new = tir - vpl / dvpl;
            if (std::abs(tir_new - tir) < tolerance) {
                return tir_new;
            }
            tir = tir_new;
        }
        return tir;
    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro no cálculo de TIR por Newton-Raphson: %1").arg(e.what()), LogManager::LogLevel::ERR);
        throw;
    }
}

void FinancialCalculatorApp::calculate_effective_rate() {
    try {
        // Índices: 0=Taxa Efetiva, 1=TIR, 2=Taxa Global, 3=Cobrança Antecipada, 4=TIR Modificada, 5=TMA vs Rentabilidade, 6=Juros Reais
        int calc_mode = eff_rate_calc_mode->currentIndex();
        QString result_text;

        // Taxa Efetiva Anual
        if (calc_mode == 0) {
            double nominal_rate = get_float_from_line_edit(eff_rate_nominal, true);
            double period_nominal = get_float_from_line_edit(eff_rate_period_nominal);
            double period_capitalization = get_float_from_line_edit(eff_rate_period_cap);
            double period_target = get_float_from_line_edit(eff_rate_period_target);

            double m = (period_capitalization != 0.0) ? (period_nominal / period_capitalization) : 0.0;
            double i_cap = (m != 0.0) ? (nominal_rate / m) : 0.0;
            double ratio = (period_capitalization != 0.0) ? (period_target / period_capitalization) : 0.0;
            double i_target = std::pow(1.0 + i_cap, ratio) - 1.0;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "CÁLCULO DE TAXA EFETIVA") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  T (%1) = %2%\n").arg(QCoreApplication::translate("App", "Taxa Nominal"), TextFormat::format_currency(nominal_rate * 100.0, 2));
            steps << QString("  %1 = %2\n").arg(QCoreApplication::translate("App", "Período da Taxa Nominal"), TextFormat::format_currency(period_nominal, 0));
            steps << QString("  %1 = %2\n").arg(QCoreApplication::translate("App", "Período de Capitalização"), TextFormat::format_currency(period_capitalization, 0));
            steps << QString("  %1 = %2\n\n").arg(QCoreApplication::translate("App", "Período Desejado"), TextFormat::format_currency(period_target, 0));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DA TAXA EFETIVA DO PERÍODO DE CAPITALIZAÇÃO"));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QString("  M (%1)\n").arg(QCoreApplication::translate("App", "Número de períodos de capitalização"));
            auto f1 = TextFormat::format_fraction(TextFormat::format_currency(period_nominal, 0), TextFormat::format_currency(period_capitalization, 0), "  M = ");
            steps << f1[0] + "\n" << f1[1] + "\n" << f1[2] + "\n";
            steps << QString("  M = %1\n\n").arg(TextFormat::format_currency(m, 0));

            steps << QString("  i (%1)\n").arg(QCoreApplication::translate("App", "Taxa efetiva por período de capitalização"));
            auto f2 = TextFormat::format_fraction("T", "M", "  i = ");
            steps << f2[0] + "\n" << f2[1] + "\n" << f2[2] + "\n";
            steps << QString("  i = %1% / %2\n").arg(TextFormat::format_currency(nominal_rate * 100.0, 2), TextFormat::format_currency(m, 0));
            steps << QString("  i = %1% %2\n\n").arg(TextFormat::format_currency(i_cap * 100.0, 2), QCoreApplication::translate("App", "por período de capitalização"));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CONVERSÃO PARA O PERÍODO DESEJADO"));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Razão de períodos"));
            auto f3 = TextFormat::format_fraction(TextFormat::format_currency(period_target, 0), TextFormat::format_currency(period_capitalization, 0), "  m = ");
            steps << f3[0] + "\n" << f3[1] + "\n" << f3[2] + "\n";
            steps << QString("  m = %1\n\n").arg(TextFormat::format_currency(ratio, 2));

            QString ratio_super = TextFormat::to_superscript(TextFormat::format_currency(ratio, 2));
            steps << QString("  1 + i%1 = (1 + i%2)%3\n").arg(TextFormat::to_subscript("alvo"), TextFormat::to_subscript("cap"), ratio_super);
            steps << QString("  1 + i%1 = (1 + %2)%3\n").arg(TextFormat::to_subscript("alvo"), TextFormat::format_currency(i_cap, 6), ratio_super);
            double pow_val = std::pow(1.0 + i_cap, ratio);
            steps << QString("  1 + i%1 = %2\n").arg(TextFormat::to_subscript("alvo"), TextFormat::format_currency(pow_val, 6));
            steps << QString("  i%1 = %2\n").arg(TextFormat::to_subscript("alvo"), TextFormat::format_currency(i_target, 6));
            steps << QString("  i%1 = %2%\n\n").arg(TextFormat::to_subscript("alvo"), TextFormat::format_currency(i_target * 100.0, 2));

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
            steps << QString("  %1 = %2%\n").arg(QCoreApplication::translate("App", "Taxa Efetiva do Período Desejado"), TextFormat::format_currency(i_target * 100.0, 4));
            steps << QString(60, QChar(0x2550)) + "\n";

            result_text = steps.join("");

        // TIR
        } else if (calc_mode == 1) {
            double initial_investment = get_float_from_line_edit(tir_initial);
            int num_periods = static_cast<int>(get_float_from_line_edit(tir_periods));
            double periodic_return = get_float_from_line_edit(tir_return);

            double tir = 0.0;
            if (num_periods == 2 && std::abs(periodic_return - initial_investment) < 0.01) {
                double discriminant = 5.0;
                double x = (-1.0 + std::sqrt(discriminant)) / 2.0;
                tir = (1.0 / x) - 1.0;
            } else {
                tir = this->calculate_tir_newton(initial_investment, periodic_return, num_periods);
            }

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "CÁLCULO DA TAXA INTERNA DE RETORNO (TIR)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fluxo de Caixa:") + "\n";
            steps << QString("  t₀ = -%1 (%2)\n").arg(TextFormat::format_currency(initial_investment, 2), QCoreApplication::translate("App", "Desembolso"));
            for (int t = 1; t <= num_periods; ++t) {
                steps << QString("  t%1 = +%2\n").arg(TextFormat::to_subscript(t), TextFormat::format_currency(periodic_return, 2));
            }
            steps << "\n";

            steps << QCoreApplication::translate("App", "Equação de VPL = 0:") + "\n";
            QString vpl_formula = QString("0 = -%1").arg(TextFormat::format_currency(initial_investment, 2));
            for (int t = 1; t <= num_periods; ++t) {
                vpl_formula += QString(" + %1/(1+TIR)%2").arg(TextFormat::format_currency(periodic_return, 2), TextFormat::to_superscript(t));
            }
            steps << QString("  %1\n\n").arg(vpl_formula);

            if (num_periods == 2) {
                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Dividindo por"), TextFormat::format_currency(initial_investment, 2));
                steps << "  0 = -1 + 1/(1+TIR) + 1/(1+TIR)²\n\n";

                steps << QCoreApplication::translate("App", "Substituindo x = 1/(1+TIR):") + "\n";
                steps << "  0 = -1 + x + x²\n";
                steps << "  x² + x - 1 = 0\n\n";

                steps << QCoreApplication::translate("App", "Usando Fórmula de Bhaskara:") + "\n";
                steps << "  x = (-1 ± √(1 + 4)) / 2\n";
                steps << "  x = (-1 ± √5) / 2\n";
                double sqrt5 = std::sqrt(5.0);
                steps << QString("  x = (-1 ± %1) / 2\n\n").arg(TextFormat::format_currency(sqrt5, 6));

                steps << QCoreApplication::translate("App", "Usando a raiz positiva:") + "\n";
                double x = (-1.0 + sqrt5) / 2.0;
                steps << QString("  x = (-1 + %1) / 2\n").arg(TextFormat::format_currency(sqrt5, 6));
                steps << QString("  x = %1\n\n").arg(TextFormat::format_currency(x, 6));

                steps << QCoreApplication::translate("App", "Revertendo a substituição:") + "\n";
                steps << QString("  %1 = 1 / (1 + TIR)\n").arg(TextFormat::format_currency(x, 6));
                steps << QString("  1 + TIR = %1\n").arg(TextFormat::format_currency(1.0 / x, 6));
                steps << QString("  TIR = %1\n").arg(TextFormat::format_currency(tir, 6));
                steps << QString("  TIR = %1%\n\n").arg(TextFormat::format_currency(tir * 100.0, 4));
            }

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
            steps << QString("  %1 = %2%\n").arg(QCoreApplication::translate("App", "Taxa Interna de Retorno"), TextFormat::format_currency(tir * 100.0, 4));
            steps << QString(60, QChar(0x2550)) + "\n";

            result_text = steps.join("");

        // Taxa Global de Juros
        } else if (calc_mode == 2) {
            double real_rate = get_float_from_line_edit(tax_global_real, true);
            double inflation_m1 = get_float_from_line_edit(tax_global_inf_m1, true);
            double inflation_m2 = get_float_from_line_edit(tax_global_inf_m2, true);
            double inflation_m3 = get_float_from_line_edit(tax_global_inf_m3, true);

            double r_trim = std::pow(1.0 + real_rate, 3) - 1.0;
            double theta_trim = (1.0 + inflation_m1) * (1.0 + inflation_m2) * (1.0 + inflation_m3) - 1.0;
            double i_global = (1.0 + theta_trim) * (1.0 + r_trim) - 1.0;

            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "CÁLCULO DA TAXA GLOBAL DE JUROS (APARENTE)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  r (%1) = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa real mensal"), TextFormat::format_currency(real_rate * 100.0, 2), QCoreApplication::translate("App", "ao mês"));
            steps << QString("  θ₁ (%1) = %2%\n").arg(QCoreApplication::translate("App", "Inflação mês 1"), TextFormat::format_currency(inflation_m1 * 100.0, 2));
            steps << QString("  θ₂ (%1) = %2%\n").arg(QCoreApplication::translate("App", "Inflação mês 2"), TextFormat::format_currency(inflation_m2 * 100.0, 2));
            steps << QString("  θ₃ (%1) = %2%\n\n").arg(QCoreApplication::translate("App", "Inflação mês 3"), TextFormat::format_currency(inflation_m3 * 100.0, 2));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "TAXA REAL DO TRIMESTRE"));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QString("  1 + r%1 = (1 + r%2)%3\n").arg(TextFormat::to_subscript("trim"), TextFormat::to_subscript("mês"), TextFormat::to_superscript(3));
            steps << QString("  1 + r%1 = (1 + %2)%3\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(real_rate, 6), TextFormat::to_superscript(3));
            double pow_r = std::pow(1.0 + real_rate, 3);
            steps << QString("  1 + r%1 = %2\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(pow_r, 6));
            steps << QString("  r%1 = %2\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(r_trim, 6));
            steps << QString("  r%1 = %2%\n\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(r_trim * 100.0, 4));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "INFLAÇÃO ACUMULADA DO TRIMESTRE"));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QString("  1 + θ%1 = (1 + θ₁) × (1 + θ₂) × (1 + θ₃)\n").arg(TextFormat::to_subscript("trim"));
            steps << QString("  1 + θ%1 = (1 + %2) × (1 + %3) × (1 + %4)\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(inflation_m1, 6), TextFormat::format_currency(inflation_m2, 6), TextFormat::format_currency(inflation_m3, 6));
            steps << QString("  1 + θ%1 = %2 × %3 × %4\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(1.0 + inflation_m1, 6), TextFormat::format_currency(1.0 + inflation_m2, 6), TextFormat::format_currency(1.0 + inflation_m3, 6));

            double mult_1_2 = (1.0 + inflation_m1) * (1.0 + inflation_m2);
            steps << QString("  1 + θ%1 = %2 × %3\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(mult_1_2, 6), TextFormat::format_currency(1.0 + inflation_m3, 6));

            double pow_theta = (1.0 + inflation_m1) * (1.0 + inflation_m2) * (1.0 + inflation_m3);
            steps << QString("  1 + θ%1 = %2\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(pow_theta, 6));
            steps << QString("  θ%1 = %2\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(theta_trim, 6));
            steps << QString("  θ%1 = %2%\n\n").arg(TextFormat::to_subscript("trim"), TextFormat::format_currency(theta_trim * 100.0, 4));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "TAXA GLOBAL (APARENTE) DO TRIMESTRE"));
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QString("  1 + i%1 = (1 + θ%2) × (1 + r%3)\n").arg(TextFormat::to_subscript("global"), TextFormat::to_subscript("trim"), TextFormat::to_subscript("trim"));
            steps << QString("  1 + i%1 = (1 + %2) × (1 + %3)\n").arg(TextFormat::to_subscript("global"), TextFormat::format_currency(theta_trim, 6), TextFormat::format_currency(r_trim, 6));
            steps << QString("  1 + i%1 = %2 × %3\n").arg(TextFormat::to_subscript("global"), TextFormat::format_currency(pow_theta, 6), TextFormat::format_currency(pow_r, 6));
            double pow_i = pow_theta * pow_r;
            steps << QString("  1 + i%1 = %2\n").arg(TextFormat::to_subscript("global"), TextFormat::format_currency(pow_i, 6));
            steps << QString("  i%1 = %2\n").arg(TextFormat::to_subscript("global"), TextFormat::format_currency(i_global, 6));
            steps << QString("  i%1 = %2%\n\n").arg(TextFormat::to_subscript("global"), TextFormat::format_currency(i_global * 100.0, 4));

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
            steps << QString("  %1 = %2%\n").arg(QCoreApplication::translate("App", "Taxa Global de Juros do Trimestre"), TextFormat::format_currency(i_global * 100.0, 4));
            steps << QString(60, QChar(0x2550)) + "\n";

            result_text = steps.join("");

        // Cobrança Antecipada
        } else if (calc_mode == 3) {
            double nominal_value = get_float_from_line_edit(adv_int_nominal);
            double advance_rate = get_float_from_line_edit(adv_int_rate, true);

            double advance_interest = nominal_value * advance_rate;
            double received_value = nominal_value - advance_interest;

            QStringList steps;
            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "CÁLCULO DE TAXA EFETIVA EM COBRANÇA ANTECIPADA") + "\n";
            steps << QCoreApplication::translate("App", "(SISTEMA ALEMÃO)") + "\n";
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  F (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Valor Nominal do Empréstimo"), TextFormat::format_currency(nominal_value, 2));
            steps << QString("  i%1 (%2) = %3% a.a.\n").arg(TextFormat::to_subscript("a"), QCoreApplication::translate("App", "Taxa de Cobrança Antecipada"), TextFormat::format_currency(advance_rate * 100.0, 2));
            steps << QString("  n (%1) = %2\n\n").arg(QCoreApplication::translate("App", "Período"), QCoreApplication::translate("App", "1 ano"));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DOS JUROS ANTECIPADOS"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QString("  J%1 = F × i%2\n").arg(TextFormat::to_subscript("ant"), TextFormat::to_subscript("a"));
            steps << QString("  J%1 = R$ %2 × %3\n").arg(TextFormat::to_subscript("ant"), TextFormat::format_currency(nominal_value, 2), TextFormat::format_currency(advance_rate, 6));
            steps << QString("  J%1 = R$ %2\n\n").arg(TextFormat::to_subscript("ant"), TextFormat::format_currency(advance_interest, 2));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VALOR RECEBIDO (PRINCIPAL LÍQUIDO)"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QString("  P (%1) = F - J%2\n").arg(QCoreApplication::translate("App", "Valor Recebido"), TextFormat::to_subscript("ant"));
            steps << QString("  P = R$ %1 - R$ %2\n").arg(TextFormat::format_currency(nominal_value, 2), TextFormat::format_currency(advance_interest, 2));
            steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(received_value, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "MÉTODO 1: LÓGICA FINANCEIRA"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Interpretação:") + "\n";
            steps << QString("  • %1 P = R$ %2\n").arg(QCoreApplication::translate("App", "O tomador recebeu"), TextFormat::format_currency(received_value, 2));
            steps << QString("  • %1 F = R$ %2\n").arg(QCoreApplication::translate("App", "Ao final de 1 ano, pagou"), TextFormat::format_currency(nominal_value, 2));
            steps << QString("  • %1 J = R$ %2\n\n").arg(QCoreApplication::translate("App", "Juros efetivamente pagos"), TextFormat::format_currency(advance_interest, 2));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DA TAXA EFETIVA PELO MÉTODO 1"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "A taxa efetiva é calculada pela relação:") + "\n\n";
            auto m1_f1 = TextFormat::format_fraction("J", "P", "  i = ");
            steps << m1_f1[0] + "\n" << m1_f1[1] + "\n" << m1_f1[2] + "\n\n";

            auto m1_f2 = TextFormat::format_fraction(QString("R$ %1").arg(TextFormat::format_currency(advance_interest, 2)), QString("R$ %1").arg(TextFormat::format_currency(received_value, 2)), "  i = ");
            steps << m1_f2[0] + "\n" << m1_f2[1] + "\n" << m1_f2[2] + "\n\n";

            double effective_rate_m1 = (received_value != 0.0) ? (advance_interest / received_value) : 0.0;
            steps << QString("  i = %1\n").arg(TextFormat::format_currency(effective_rate_m1, 6));
            steps << QString("  i = %1%\n\n").arg(TextFormat::format_currency(effective_rate_m1 * 100.0, 4));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "MÉTODO 2: USO DA FÓRMULA DE CONVERSÃO"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula para converter taxa antecipada em taxa efetiva:") + "\n\n";
            auto m2_f1 = TextFormat::format_fraction(QString("i%1").arg(TextFormat::to_subscript("a")), QString("1 - i%1").arg(TextFormat::to_subscript("a")), "  i = ");
            steps << m2_f1[0] + "\n" << m2_f1[1] + "\n" << m2_f1[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Onde:") + "\n";
            steps << QString("  i%1 = %2 = %3% = %4\n\n").arg(TextFormat::to_subscript("a"), QCoreApplication::translate("App", "Taxa de cobrança antecipada"), TextFormat::format_currency(advance_rate * 100.0, 2), TextFormat::format_currency(advance_rate, 6));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("4. %1\n").arg(QCoreApplication::translate("App", "APLICAÇÃO DA FÓRMULA"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            auto m2_f2 = TextFormat::format_fraction(TextFormat::format_currency(advance_rate, 6), QString("1 - %1").arg(TextFormat::format_currency(advance_rate, 6)), "  i = ");
            steps << m2_f2[0] + "\n" << m2_f2[1] + "\n" << m2_f2[2] + "\n\n";

            double denominator = 1.0 - advance_rate;
            auto m2_f3 = TextFormat::format_fraction(TextFormat::format_currency(advance_rate, 6), TextFormat::format_currency(denominator, 6), "  i = ");
            steps << m2_f3[0] + "\n" << m2_f3[1] + "\n" << m2_f3[2] + "\n\n";

            double effective_rate_m2 = (denominator != 0.0) ? (advance_rate / denominator) : 0.0;
            steps << QString("  i = %1\n").arg(TextFormat::format_currency(effective_rate_m2, 6));
            steps << QString("  i = %1%\n\n").arg(TextFormat::format_currency(effective_rate_m2 * 100.0, 4));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "VERIFICAÇÃO DOS RESULTADOS"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QString("  %1: i = %2%\n").arg(QCoreApplication::translate("App", "Método 1 (Lógica Financeira)"), TextFormat::format_currency(effective_rate_m1 * 100.0, 4));
            steps << QString("  %1: i = %2%\n\n").arg(QCoreApplication::translate("App", "Método 2 (Fórmula de Conversão)"), TextFormat::format_currency(effective_rate_m2 * 100.0, 4));

            double diff_percent = std::abs(effective_rate_m1 - effective_rate_m2) * 100.0;
            if (diff_percent < 0.0001) {
                steps << QString("  ✓ %1\n\n").arg(QCoreApplication::translate("App", "Os resultados são idênticos (diferença < 0,0001%)"));
            } else {
                steps << QString("  %1: %2%\n\n").arg(QCoreApplication::translate("App", "Diferença entre métodos"), TextFormat::format_currency(diff_percent, 6));
            }

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESUMO:"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Valor Nominal do Empréstimo (F)"), TextFormat::format_currency(nominal_value, 2));
            steps << QString("  %1 (i%2): %3%\n").arg(QCoreApplication::translate("App", "Taxa de Cobrança Antecipada"), TextFormat::to_subscript("a"), TextFormat::format_currency(advance_rate * 100.0, 2));
            steps << QString("  %1 (J%2): R$ %3\n").arg(QCoreApplication::translate("App", "Juros Pagos Antecipadamente"), TextFormat::to_subscript("ant"), TextFormat::format_currency(advance_interest, 2));
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Valor Recebido (P)"), TextFormat::format_currency(received_value, 2));
            steps << QString("  %1: %2%\n\n").arg(QCoreApplication::translate("App", "Taxa Efetiva Anual (i)"), TextFormat::format_currency(effective_rate_m1 * 100.0, 4));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESPOSTA FINAL:"));
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "A taxa efetiva anual que produz os mesmos juros é de"));
            steps << QString("  %1% %2\n").arg(TextFormat::format_currency(effective_rate_m1 * 100.0, 4), QCoreApplication::translate("App", "ao ano"));
            steps << QString(70, QChar(0x2550)) + "\n";

            result_text = steps.join("");

        // TIR Modificada (TIRm)
        } else if (calc_mode == 4) {
            double initial_investment = get_float_from_line_edit(tirm_initial);
            int num_periods = static_cast<int>(get_float_from_line_edit(tirm_periods));
            double periodic_return = get_float_from_line_edit(tirm_return);
            double capitalization_rate = get_float_from_line_edit(tirm_cap_rate, true);

            if (initial_investment <= 0.0) {
                throw std::invalid_argument(QCoreApplication::translate("App", "Investimento Inicial deve ser maior que zero.").toUtf8().constData());
            }
            if (num_periods <= 0) {
                throw std::invalid_argument(QCoreApplication::translate("App", "Número de períodos deve ser maior que zero.").toUtf8().constData());
            }

            QStringList steps;
            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "CÁLCULO DA TAXA INTERNA DE RETORNO MODIFICADA (TIRm)") + "\n";
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  VPC (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Valor Presente dos Custos"), TextFormat::format_currency(initial_investment, 2));
            steps << QString("  n (%1) = %2\n").arg(QCoreApplication::translate("App", "Número de períodos")).arg(num_periods);
            steps << QString("  %1 = R$ %2\n").arg(QCoreApplication::translate("App", "Retorno por período"), TextFormat::format_currency(periodic_return, 2));
            steps << QString("  %1 = %2% %3\n\n").arg(QCoreApplication::translate("App", "Taxa de capitalização"), TextFormat::format_currency(capitalization_rate * 100.0, 2), QCoreApplication::translate("App", "ao período"));

            // 1. Cálculo do VFB
            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VALOR FUTURO DOS BENEFÍCIOS (VFB)"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            steps << QString("  VFB = Σ B%1 × (1 + i)%2\n\n").arg(TextFormat::to_subscript("k"), TextFormat::to_superscript("n-k"));

            steps << QCoreApplication::translate("App", "Onde:") + "\n";
            steps << QString("  B%1 = %2\n").arg(TextFormat::to_subscript("k"), QCoreApplication::translate("App", "Benefício no período k"));
            steps << QString("  i = %1 = %2\n").arg(QCoreApplication::translate("App", "Taxa de capitalização"), TextFormat::format_currency(capitalization_rate, 6));
            steps << QString("  n = %1 = %2\n\n").arg(QCoreApplication::translate("App", "Horizonte de análise")).arg(num_periods);

            double vfb = 0.0;
            for (int k = 1; k <= num_periods; ++k) {
                int exponent = num_periods - k;
                double factor = std::pow(1.0 + capitalization_rate, exponent);
                double capitalized_value = periodic_return * factor;
                vfb += capitalized_value;

                steps << QString("  %1 %2:\n").arg(QCoreApplication::translate("App", "Período")).arg(k);
                steps << QString("    B%1 × (1 + %2)%3\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(capitalization_rate, 6), TextFormat::to_superscript(exponent));
                steps << QString("    = %1 × %2\n").arg(TextFormat::format_currency(periodic_return, 2), TextFormat::format_currency(factor, 6));
                steps << QString("    = R$ %1\n\n").arg(TextFormat::format_currency(capitalized_value, 2));
            }

            steps << "  VFB = ";
            for (int k = 1; k <= num_periods; ++k) {
                int exponent = num_periods - k;
                if (k > 1) {
                    steps << " + ";
                }
                steps << QString("%1 × (%2)%3").arg(TextFormat::format_currency(periodic_return, 2), TextFormat::format_currency(1.0 + capitalization_rate, 4), TextFormat::to_superscript(exponent));
            }
            steps << "\n";
            steps << QString("  VFB = R$ %1\n\n").arg(TextFormat::format_currency(vfb, 2));

            // 2. Cálculo da TIRm
            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DA TAXA INTERNA DE RETORNO MODIFICADA"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Relação fundamental de equivalência financeira:"));
            steps << QString("  VPC × (1 + TIR%1)%2 = VFB\n").arg(TextFormat::to_subscript("m"), TextFormat::to_superscript("n"));
            steps << QString("  (1 + TIR%1)%2 = VFB / VPC\n\n").arg(TextFormat::to_subscript("m"), TextFormat::to_superscript("n"));

            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Isolando a TIRm:"));
            steps << TextFormat::render_radical_fraction_html(
                "n", "VFB", "VPC", 
                QString("  TIR%1 = ").arg(TextFormat::to_subscript("m")), 
                "  - 1"
            ) + "\n\n";

            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Substituindo os valores:"));
            steps << TextFormat::render_radical_fraction_html(
                QString::number(num_periods),
                QString("R$ %1").arg(TextFormat::format_currency(vfb, 2)),
                QString("R$ %1").arg(TextFormat::format_currency(initial_investment, 2)),
                QString("  TIR%1 = ").arg(TextFormat::to_subscript("m")),
                "  - 1"
            ) + "\n\n";

            double ratio = (initial_investment != 0.0) ? (vfb / initial_investment) : 0.0;
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Efetuando a divisão:"));
            steps << TextFormat::render_radical_single_html(
                QString::number(num_periods),
                TextFormat::format_currency(ratio, 6),
                QString("  TIR%1 = ").arg(TextFormat::to_subscript("m")),
                "  - 1"
            ) + "\n\n";

            double root_value = (num_periods != 0) ? std::pow(ratio, 1.0 / num_periods) : 0.0;
            double tirm = root_value - 1.0;
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Extraindo a raiz:"));
            steps << QString("  TIR%1 = %2 - 1\n").arg(TextFormat::to_subscript("m"), TextFormat::format_currency(root_value, 6));
            steps << QString("  TIR%1 = %2\n").arg(TextFormat::to_subscript("m"), TextFormat::format_currency(tirm, 6));
            steps << QString("  TIR%1 = %2%\n\n").arg(TextFormat::to_subscript("m"), TextFormat::format_currency(tirm * 100.0, 4));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESUMO:"));
            steps << QString(70, QChar(0x2550)) + "\n\n";
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Valor Presente dos Custos (VPC)"), TextFormat::format_currency(initial_investment, 2));
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Valor Futuro dos Benefícios (VFB)"), TextFormat::format_currency(vfb, 2));
            steps << QString("  %1: %2\n").arg(QCoreApplication::translate("App", "Número de períodos (n)")).arg(num_periods);
            steps << QString("  %1: %2%\n\n").arg(QCoreApplication::translate("App", "Taxa de capitalização"), TextFormat::format_currency(capitalization_rate * 100.0, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESPOSTA FINAL:"));
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "A Taxa Interna de Retorno Modificada é de"));
            steps << QString("  %1% %2\n").arg(TextFormat::format_currency(tirm * 100.0, 4), QCoreApplication::translate("App", "ao período"));
            steps << QString(70, QChar(0x2550)) + "\n";

            result_text = steps.join("");

        // TMA vs Rentabilidade
        } else if (calc_mode == 5) {
            double capital = get_float_from_line_edit(tma_capital);
            double monthly_rate = get_float_from_line_edit(tma_monthly_rate, true);
            double tma_annual = get_float_from_line_edit(tma_rate, true);
            int periods = static_cast<int>(get_float_from_line_edit(tma_periods));

            QStringList steps;
            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "ANÁLISE DE INVESTIMENTO: TMA vs RENTABILIDADE") + "\n";
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Capital Aplicado"), TextFormat::format_currency(capital, 2));
            steps << QString("  i%1 (%2) = %3% %4\n").arg(TextFormat::to_subscript("mensal"), QCoreApplication::translate("App", "Taxa da Oportunidade"), TextFormat::format_currency(monthly_rate * 100.0, 2), QCoreApplication::translate("App", "ao mês"));
            steps << QString("  TMA (%1) = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa Mínima de Atratividade"), TextFormat::format_currency(tma_annual * 100.0, 2), QCoreApplication::translate("App", "ao ano"));
            steps << QString("  n (%1) = %2 %3\n\n").arg(QCoreApplication::translate("App", "Prazo")).arg(periods).arg(QCoreApplication::translate("App", "meses"));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO RENDIMENTO DA OPORTUNIDADE"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Primeiro, calculamos a taxa efetiva anual equivalente:") + "\n\n";
            steps << QString("  i%1 = (1 + i%2)%3 - 1\n").arg(TextFormat::to_subscript("anual"), TextFormat::to_subscript("mensal"), TextFormat::to_superscript(12));
            steps << QString("  i%1 = (1 + %2)%3 - 1\n").arg(TextFormat::to_subscript("anual"), TextFormat::format_currency(monthly_rate, 6), TextFormat::to_superscript(12));
            steps << QString("  i%1 = (%2)%3 - 1\n").arg(TextFormat::to_subscript("anual"), TextFormat::format_currency(1.0 + monthly_rate, 6), TextFormat::to_superscript(12));

            double annual_rate = std::pow(1.0 + monthly_rate, 12) - 1.0;
            steps << QString("  i%1 = %2 - 1\n").arg(TextFormat::to_subscript("anual"), TextFormat::format_currency(1.0 + annual_rate, 6));
            steps << QString("  i%1 = %2\n").arg(TextFormat::to_subscript("anual"), TextFormat::format_currency(annual_rate, 6));
            steps << QString("  i%1 = %2% a.a.\n\n").arg(TextFormat::to_subscript("anual"), TextFormat::format_currency(annual_rate * 100.0, 4));

            steps << QCoreApplication::translate("App", "Rendimento da oportunidade:") + "\n\n";
            double rendimento_real = capital * annual_rate;
            steps << QString("  %1 = P × i%2\n").arg(QCoreApplication::translate("App", "Rendimento"), TextFormat::to_subscript("anual"));
            steps << QString("  %1 = R$ %2 × %3\n").arg(QCoreApplication::translate("App", "Rendimento"), TextFormat::format_currency(capital, 2), TextFormat::format_currency(annual_rate, 6));
            steps << QString("  %1 = R$ %2\n\n").arg(QCoreApplication::translate("App", "Rendimento"), TextFormat::format_currency(rendimento_real, 2));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO RENDIMENTO MÍNIMO ACEITÁVEL (TMA)"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            double rendimento_tma = capital * tma_annual;
            steps << QString("  %1%2 = P × TMA\n").arg(QCoreApplication::translate("App", "Rendimento"), TextFormat::to_subscript("TMA"));
            steps << QString("  %1%2 = R$ %3 × %4\n").arg(QCoreApplication::translate("App", "Rendimento"), TextFormat::to_subscript("TMA"), TextFormat::format_currency(capital, 2), TextFormat::format_currency(tma_annual, 6));
            steps << QString("  %1%2 = R$ %3\n\n").arg(QCoreApplication::translate("App", "Rendimento"), TextFormat::to_subscript("TMA"), TextFormat::format_currency(rendimento_tma, 2));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DA DIFERENÇA"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            double diferenca = rendimento_real - rendimento_tma;
            steps << QString("  %1 = %2%3 - %2%4\n").arg(QCoreApplication::translate("App", "Diferença"), QCoreApplication::translate("App", "Rendimento"), TextFormat::to_subscript("Real"), TextFormat::to_subscript("TMA"));
            steps << QString("  %1 = R$ %2 - R$ %3\n").arg(QCoreApplication::translate("App", "Diferença"), TextFormat::format_currency(rendimento_real, 2), TextFormat::format_currency(rendimento_tma, 2));
            steps << QString("  %1 = R$ %2\n\n").arg(QCoreApplication::translate("App", "Diferença"), TextFormat::format_currency(diferenca, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESUMO:"));
            steps << QString(70, QChar(0x2550)) + "\n\n";
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Capital Aplicado"), TextFormat::format_currency(capital, 2));
            steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "Taxa Mensal da Oportunidade"), TextFormat::format_currency(monthly_rate * 100.0, 2));
            steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "Taxa Anual Equivalente"), TextFormat::format_currency(annual_rate * 100.0, 4));
            steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "TMA (anual)"), TextFormat::format_currency(tma_annual * 100.0, 2));
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Rendimento da Oportunidade"), TextFormat::format_currency(rendimento_real, 2));
            steps << QString("  %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Rendimento Mínimo (TMA)"), TextFormat::format_currency(rendimento_tma, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESPOSTA FINAL:"));
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "O valor da diferença é de"));
            steps << QString("  R$ %1\n").arg(TextFormat::format_currency(diferenca, 2));
            steps << QString(70, QChar(0x2550)) + "\n";

            result_text = steps.join("");

        // Juros Reais
        } else if (calc_mode == 6) {
            double capital = get_float_from_line_edit(real_int_capital);
            double global_rate = get_float_from_line_edit(real_int_global_rate, true);
            double inflation_rate = get_float_from_line_edit(real_int_inflation, true);

            QStringList steps;
            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "CÁLCULO DE JUROS REAIS (ACIMA DA INFLAÇÃO)") + "\n";
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1) = R$ %2\n").arg(QCoreApplication::translate("App", "Capital"), TextFormat::format_currency(capital, 2));
            steps << QString("  i (%1) = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa Global"), TextFormat::format_currency(global_rate * 100.0, 2), QCoreApplication::translate("App", "ao ano"));
            steps << QString("  θ (%1) = %2% %3\n\n").arg(QCoreApplication::translate("App", "Inflação"), TextFormat::format_currency(inflation_rate * 100.0, 2), QCoreApplication::translate("App", "ao ano"));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "MÉTODO 1: CÁLCULO PELA TAXA REAL"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "ENCONTRAR A TAXA REAL (r)"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula de Fisher:") + "\n";
            steps << "  1 + i = (1 + θ) × (1 + r)\n\n";

            steps << QCoreApplication::translate("App", "Isolando r:") + "\n";
            auto fish_f1 = TextFormat::format_fraction("1 + i", "1 + θ", "  1 + r = ");
            steps << fish_f1[0] + "\n" << fish_f1[1] + "\n" << fish_f1[2] + "\n\n";

            auto fish_f2 = TextFormat::format_fraction(QString("1 + %1").arg(TextFormat::format_currency(global_rate, 6)), QString("1 + %1").arg(TextFormat::format_currency(inflation_rate, 6)), "  1 + r = ");
            steps << fish_f2[0] + "\n" << fish_f2[1] + "\n" << fish_f2[2] + "\n\n";

            auto fish_f3 = TextFormat::format_fraction(TextFormat::format_currency(1.0 + global_rate, 6), TextFormat::format_currency(1.0 + inflation_rate, 6), "  1 + r = ");
            steps << fish_f3[0] + "\n" << fish_f3[1] + "\n" << fish_f3[2] + "\n\n";

            double real_rate = ((1.0 + inflation_rate) != 0.0) ? ((1.0 + global_rate) / (1.0 + inflation_rate) - 1.0) : 0.0;
            steps << QString("  1 + r = %1\n").arg(TextFormat::format_currency(1.0 + real_rate, 6));
            steps << QString("  r = %1\n").arg(TextFormat::format_currency(real_rate, 6));
            steps << QString("  r = %1%\n\n").arg(TextFormat::format_currency(real_rate * 100.0, 4));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CALCULAR OS JUROS REAIS"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            double juros_reais_m1 = capital * real_rate;
            steps << QString("  %1 = P × r\n").arg(QCoreApplication::translate("App", "Juros Reais"));
            steps << QString("  %1 = R$ %2 × %3\n").arg(QCoreApplication::translate("App", "Juros Reais"), TextFormat::format_currency(capital, 2), TextFormat::format_currency(real_rate, 6));
            steps << QString("  %1 = R$ %2\n\n").arg(QCoreApplication::translate("App", "Juros Reais"), TextFormat::format_currency(juros_reais_m1, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "MÉTODO 2: CÁLCULO PELO GANHO DE PODER DE COMPRA"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CALCULAR O MONTANTE FINAL (APARENTE)"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            double montante = capital * (1.0 + global_rate);
            steps << "  F = P × (1 + i)\n";
            steps << QString("  F = R$ %1 × (1 + %2)\n").arg(TextFormat::format_currency(capital, 2), TextFormat::format_currency(global_rate, 6));
            steps << QString("  F = R$ %1 × %2\n").arg(TextFormat::format_currency(capital, 2), TextFormat::format_currency(1.0 + global_rate, 6));
            steps << QString("  F = R$ %1\n\n").arg(TextFormat::format_currency(montante, 2));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CALCULAR O PODER DE COMPRA DESSE MONTANTE"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            steps << QString("%1\n\n").arg(QCoreApplication::translate("App", "(Descontando a inflação)"));

            double valor_real = ((1.0 + inflation_rate) != 0.0) ? (montante / (1.0 + inflation_rate)) : 0.0;
            auto vp_f1 = TextFormat::format_fraction("F", "1 + θ", "  VP = ");
            steps << vp_f1[0] + "\n" << vp_f1[1] + "\n" << vp_f1[2] + "\n\n";

            auto vp_f2 = TextFormat::format_fraction(QString("R$ %1").arg(TextFormat::format_currency(montante, 2)), QString("1 + %1").arg(TextFormat::format_currency(inflation_rate, 6)), "  VP = ");
            steps << vp_f2[0] + "\n" << vp_f2[1] + "\n" << vp_f2[2] + "\n\n";

            auto vp_f3 = TextFormat::format_fraction(QString("R$ %1").arg(TextFormat::format_currency(montante, 2)), TextFormat::format_currency(1.0 + inflation_rate, 6), "  VP = ");
            steps << vp_f3[0] + "\n" << vp_f3[1] + "\n" << vp_f3[2] + "\n\n";

            steps << QString("  VP = R$ %1\n\n").arg(TextFormat::format_currency(valor_real, 2));

            steps << QString(70, QChar(0x2500)) + "\n";
            steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "CALCULAR O GANHO REAL (JUROS REAIS)"));
            steps << QString(70, QChar(0x2500)) + "\n\n";

            double juros_reais_m2 = valor_real - capital;
            steps << QString("  %1 = VP - P\n").arg(QCoreApplication::translate("App", "Juros Reais"));
            steps << QString("  %1 = R$ %2 - R$ %3\n").arg(QCoreApplication::translate("App", "Juros Reais"), TextFormat::format_currency(valor_real, 2), TextFormat::format_currency(capital, 2));
            steps << QString("  %1 = R$ %2\n\n").arg(QCoreApplication::translate("App", "Juros Reais"), TextFormat::format_currency(juros_reais_m2, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "VERIFICAÇÃO DOS RESULTADOS"));
            steps << QString(70, QChar(0x2550)) + "\n\n";

            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Método 1 (Taxa Real)"), TextFormat::format_currency(juros_reais_m1, 2));
            steps << QString("  %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Método 2 (Poder de Compra)"), TextFormat::format_currency(juros_reais_m2, 2));

            double diff = std::abs(juros_reais_m1 - juros_reais_m2);
            if (diff < 0.01) {
                steps << QString("  ✓ %1\n\n").arg(QCoreApplication::translate("App", "Os resultados são idênticos"));
            }

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESUMO:"));
            steps << QString(70, QChar(0x2550)) + "\n\n";
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Capital Aplicado"), TextFormat::format_currency(capital, 2));
            steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "Taxa Global (i)"), TextFormat::format_currency(global_rate * 100.0, 2));
            steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "Inflação (θ)"), TextFormat::format_currency(inflation_rate * 100.0, 2));
            steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "Taxa Real (r)"), TextFormat::format_currency(real_rate * 100.0, 4));
            steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Montante Final"), TextFormat::format_currency(montante, 2));
            steps << QString("  %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Poder de Compra Final"), TextFormat::format_currency(valor_real, 2));

            steps << QString(70, QChar(0x2550)) + "\n";
            steps << QString("%1\n").arg(QCoreApplication::translate("App", "RESPOSTA FINAL:"));
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "O valor dos juros reais obtidos foi de"));
            steps << QString("  R$ %1\n").arg(TextFormat::format_currency(juros_reais_m1, 2));
            steps << QString(70, QChar(0x2550)) + "\n";

            result_text = steps.join("");
        }

        if (!result_text.isEmpty() && eff_rate_result) {
            eff_rate_result->append(result_text);
        }

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao calcular taxa efetiva/TIR/global: %1").arg(e.what()));
        if (eff_rate_result) {
            QString err_label = QCoreApplication::translate("App", "Erro");
            QString msg = QString::fromUtf8(e.what());
            if (msg.startsWith("Erro: ") || msg.startsWith("Error: ")) {
                eff_rate_result->append(msg);
            } else {
                eff_rate_result->append(QString("%1: %2").arg(err_label, msg));
            }
        }
    }
}
