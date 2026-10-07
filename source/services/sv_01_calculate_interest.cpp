#include "sv_01_calculate_interest.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <functional>

void calculate_interest(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_interest();
    }
}

static QString format_interest_result(double p, double f_in, double i, double n, int calc_type_index, bool is_compound) {
    QString result_text;

    // Comparar JS vs JC
    if (calc_type_index == 2) {
        int n_base = static_cast<int>(n);

        QStringList steps;
        steps << QString(70, QChar(0x2550)) + "\n";
        steps << TextFormat::to_unicode_subscripts(QCoreApplication::translate("App", "COMPARAÇÃO: JUROS SIMPLES vs JUROS COMPOSTOS")) + "\n";
        steps << QString(70, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "Objetivo:") + "\n";
        steps << TextFormat::to_unicode_subscripts(
            QCoreApplication::translate("App", "Encontrar quantos meses (n_s) são necessários para que o montante a juros simples (F_s) supere o montante a juros compostos (F_c) calculado para %1 meses.")
            .arg(n_base)) + "\n\n";

        steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
        steps << QString("  P (%1)           = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
        steps << QString("  i (%1)                = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0), QCoreApplication::translate("App", "ao período"));
        steps << TextFormat::to_unicode_subscripts(QString("  n_c (%1)   = %2 %3").arg(QCoreApplication::translate("App", "Período base JC")).arg(n_base).arg(QCoreApplication::translate("App", "meses"))) + "\n\n";

        // Passo 1: Calcular F_c para n_base meses
        double f_c = p * std::pow(1.0 + i, n_base);
        steps << QString(70, QChar(0x2500)) + "\n";
        steps << TextFormat::to_unicode_subscripts(
            QCoreApplication::translate("App", "PASSO 1: Calcular montante a juros compostos para %1 meses").arg(n_base)) + "\n";
        steps << QString(70, QChar(0x2500)) + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
        steps << TextFormat::to_unicode_subscripts("  F_c = P × (1 + i)ⁿᶜ") + "\n\n";

        QString n_super = TextFormat::to_superscript(n_base);
        steps << QCoreApplication::translate("App", "Substituindo os valores:") + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  F_c = %1 × (1 + %2)%3").arg(TextFormat::format_currency(p), TextFormat::format_currency(i), n_super)) + "\n";

        double fator_jc = std::pow(1.0 + i, n_base);
        steps << TextFormat::to_unicode_subscripts(QString("  F_c = %1 × %2").arg(TextFormat::format_currency(p), TextFormat::format_currency(fator_jc))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  F_c = R$ %1").arg(TextFormat::format_currency(f_c))) + "\n\n";

        // Passo 2: Estabelecer inequação e resolver
        steps << QString(70, QChar(0x2500)) + "\n";
        steps << TextFormat::to_unicode_subscripts(QCoreApplication::translate("App", "PASSO 2: Estabelecer a inequação e resolver para n_s")) + "\n";
        steps << QString(70, QChar(0x2500)) + "\n\n";

        steps << TextFormat::to_unicode_subscripts(QCoreApplication::translate("App", "Queremos encontrar n_s tal que:")) + "\n";
        steps << TextFormat::to_unicode_subscripts("  F_s > F_c") + "\n\n";

        steps << QCoreApplication::translate("App", "Fórmula de juros simples:") + "\n";
        steps << TextFormat::to_unicode_subscripts("  F_s = P × (1 + n_s × i)") + "\n\n";

        steps << QCoreApplication::translate("App", "Inequação:") + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  P × (1 + n_s × i) > %1").arg(TextFormat::format_currency(f_c))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  %1 × (1 + n_s × %2) > %3").arg(TextFormat::format_currency(p), TextFormat::format_currency(i), TextFormat::format_currency(f_c))) + "\n\n";

        steps << QCoreApplication::translate("App", "Dividindo ambos os lados por P:") + "\n";
        double razao = (p != 0.0) ? (f_c / p) : 0.0;
        steps << TextFormat::to_unicode_subscripts(QString("  1 + n_s × %1 > %2 / %3").arg(TextFormat::format_currency(i), TextFormat::format_currency(f_c), TextFormat::format_currency(p))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  1 + n_s × %1 > %2").arg(TextFormat::format_currency(i), TextFormat::format_currency(razao))) + "\n\n";

        steps << TextFormat::to_unicode_subscripts(QCoreApplication::translate("App", "Isolando n_s:")) + "\n";
        double diferenca = razao - 1.0;
        steps << TextFormat::to_unicode_subscripts(QString("  n_s × %1 > %2 - 1").arg(TextFormat::format_currency(i), TextFormat::format_currency(razao))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  n_s × %1 > %2").arg(TextFormat::format_currency(i), TextFormat::format_currency(diferenca))) + "\n";

        double n_s_real = (i != 0.0) ? (diferenca / i) : 0.0;
        steps << TextFormat::to_unicode_subscripts(QString("  n_s > %1 / %2").arg(TextFormat::format_currency(diferenca), TextFormat::format_currency(i))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  n_s > %1").arg(TextFormat::format_currency(n_s_real))) + "\n\n";

        // Resposta final
        int n_s_inteiro = static_cast<int>(n_s_real) + 1;
        steps << QString(70, QChar(0x2500)) + "\n";
        steps << QCoreApplication::translate("App", "RESULTADO") + "\n";
        steps << QString(70, QChar(0x2500)) + "\n\n";

        steps << QCoreApplication::translate("App", "Como o número de meses deve ser inteiro, o menor valor que satisfaz a condição é %1.").arg(n_s_inteiro) + "\n\n";

        // Verificação
        double f_s_verificacao = p * (1.0 + n_s_inteiro * i);
        steps << QCoreApplication::translate("App", "Verificação:") + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  F_s(%1) = %2 × (1 + %3 × %4)").arg(n_s_inteiro).arg(TextFormat::format_currency(p)).arg(n_s_inteiro).arg(TextFormat::format_currency(i))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  F_s(%1) = %2 × %3").arg(n_s_inteiro).arg(TextFormat::format_currency(p), TextFormat::format_currency(1.0 + n_s_inteiro * i))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  F_s(%1) = R$ %2").arg(n_s_inteiro).arg(TextFormat::format_currency(f_s_verificacao))) + "\n\n";

        steps << TextFormat::to_unicode_subscripts(QString("  F_c(%1) = R$ %2").arg(n_base).arg(TextFormat::format_currency(f_c))) + "\n";
        steps << TextFormat::to_unicode_subscripts(QString("  F_s(%1) = R$ %2").arg(n_s_inteiro).arg(TextFormat::format_currency(f_s_verificacao))) + "\n\n";

        if (f_s_verificacao > f_c) {
            steps << TextFormat::to_unicode_subscripts(QString("  ✓ F_s(%1) > F_c(%2)").arg(n_s_inteiro).arg(n_base)) + "\n\n";
        }

        steps << QString(70, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "RESPOSTA: São necessários %1 meses consecutivos.").arg(n_s_inteiro) + "\n";
        steps << QString(70, QChar(0x2550)) + "\n";

        result_text = steps.join("");

    } else if (calc_type_index == 0) {  // Calcular Montante (F)
        if (is_compound) {
            double f = p * std::pow(1.0 + i, n);
            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "JUROS COMPOSTOS - CÁLCULO DO MONTANTE (F)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            steps << "  F = P × (1 + i)ⁿ\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos")).arg(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
            QString n_super = TextFormat::to_superscript(static_cast<int>(n));
            steps << QString("  F = %1 × (1 + %2)%3\n\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(i), n_super);

            double pow_val = std::pow(1.0 + i, n);
            steps << QCoreApplication::translate("App", "Cálculo do fator:") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  F = %1 × %2\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(pow_val));
            steps << QString("  F = R$ %1\n\n").arg(TextFormat::format_currency(f));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O montante final é R$") + QString(" %1\n").arg(TextFormat::format_currency(f));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        } else { // Juros Simples
            double f = p * (1.0 + n * i);
            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "JUROS SIMPLES - CÁLCULO DO MONTANTE (F)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            steps << "  F = P × (1 + n × i)\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos")).arg(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
            steps << QString("  F = %1 × (1 + %2 × %3)\n\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(static_cast<int>(n)), TextFormat::format_currency(i));

            double interp = 1.0 + n * i;
            steps << QCoreApplication::translate("App", "Cálculo do fator:") + "\n";
            steps << QString("  1 + n × i = 1 + %1 × %2\n").arg(static_cast<int>(n)).arg(TextFormat::format_currency(i));
            steps << QString("  1 + n × i = 1 + %1\n").arg(TextFormat::format_currency(n * i));
            steps << QString("  1 + n × i = %1\n\n").arg(TextFormat::format_currency(interp));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  F = %1 × %2\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(interp));
            steps << QString("  F = R$ %1\n\n").arg(TextFormat::format_currency(f));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O montante final é R$") + QString(" %1\n").arg(TextFormat::format_currency(f));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        }
    } else { // Calcular Principal (P)
        double f = f_in;
        if (is_compound) {
            double p_calc = (std::pow(1.0 + i, n) != 0.0) ? (f / std::pow(1.0 + i, n)) : 0.0;
            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "JUROS COMPOSTOS - CÁLCULO DO PRINCIPAL (P)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto frac = TextFormat::format_fraction("F", "(1 + i)ⁿ", "  P = ");
            steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  F (%1)       = R$ %2\n").arg(QCoreApplication::translate("App", "Montante"), TextFormat::format_currency(f));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos")).arg(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
            QString n_super = TextFormat::to_superscript(static_cast<int>(n));
            steps << QString("  P = %1 / (1 + %2)%3\n\n").arg(TextFormat::format_currency(f), TextFormat::format_currency(i), n_super);

            double denom = std::pow(1.0 + i, n);
            steps << QCoreApplication::translate("App", "Cálculo do fator:") + "\n";
            steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(i), n_super);
            steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(denom));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  P = %1 / %2\n").arg(TextFormat::format_currency(f), TextFormat::format_currency(denom));
            steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O principal necessário é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        } else { // Juros Simples
            double p_calc = ((1.0 + n * i) != 0.0) ? (f / (1.0 + n * i)) : 0.0;
            QStringList steps;
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "JUROS SIMPLES - CÁLCULO DO PRINCIPAL (P)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto frac = TextFormat::format_fraction("F", "1 + n × i", "  P = ");
            steps << frac[0] + "\n" << frac[1] + "\n" << frac[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  F (%1)       = R$ %2\n").arg(QCoreApplication::translate("App", "Montante"), TextFormat::format_currency(f));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos")).arg(static_cast<int>(n));

            steps << QCoreApplication::translate("App", "Desenvolvimento:") + "\n";
            steps << QString("  P = %1 / (1 + %2 × %3)\n\n").arg(TextFormat::format_currency(f)).arg(static_cast<int>(n)).arg(TextFormat::format_currency(i));

            double denom = 1.0 + n * i;
            steps << QCoreApplication::translate("App", "Cálculo do fator:") + "\n";
            steps << QString("  1 + n × i = 1 + %1 × %2\n").arg(static_cast<int>(n)).arg(TextFormat::format_currency(i));
            steps << QString("  1 + n × i = 1 + %1\n").arg(TextFormat::format_currency(n * i));
            steps << QString("  1 + n × i = %1\n\n").arg(TextFormat::format_currency(denom));

            steps << QCoreApplication::translate("App", "Cálculo final:") + "\n";
            steps << QString("  P = %1 / %2\n").arg(TextFormat::format_currency(f), TextFormat::format_currency(denom));
            steps << QString("  P = R$ %1\n\n").arg(TextFormat::format_currency(p_calc));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "RESPOSTA: O principal necessário é R$") + QString(" %1\n").arg(TextFormat::format_currency(p_calc));
            steps << QString(60, QChar(0x2500)) + "\n";

            result_text = steps.join("");
        }
    }

    return result_text;
}

void FinancialCalculatorApp::calculate_interest() {
    try {
        double i = get_float_from_line_edit(interest_i, true);
        double n = get_float_from_line_edit(interest_n);

        int calc_type_index = interest_calc_type->currentIndex();
        bool is_compound = interest_regime->currentIndex() == 0;

        double p = 0.0;
        double f_in = 0.0;
        if (calc_type_index == 1) { // Calcular Principal (P)
            f_in = get_float_from_line_edit(interest_f);
        } else {
            p = get_float_from_line_edit(interest_p);
        }

        QString result_text = format_interest_result(p, f_in, i, n, calc_type_index, is_compound);

        if (!result_text.isEmpty() && interest_result) {
            auto retrans_fn = [p, f_in, i, n, calc_type_index, is_compound]() -> QString {
                return format_interest_result(p, f_in, i, n, calc_type_index, is_compound);
            };
            interest_result->append(result_text, nullptr, retrans_fn);
        }

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao calcular juros: %1").arg(e.what()));
        if (interest_result) {
            interest_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
