#include "sv_06_calculate_investment.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_investment(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_investment();
    }
}

static void calculate_vpl_vaue_uniform(FinancialCalculatorApp* app) {
    double inv_inicial = app->get_float_from_line_edit(app->invest_initial);
    double a = app->get_float_from_line_edit(app->invest_cashflow);
    double n = app->get_float_from_line_edit(app->invest_n);
    double tma = app->get_float_from_line_edit(app->invest_tma, true);

    double pow_val = std::pow(1.0 + tma, n);
    double num_pa = pow_val - 1.0;
    double den_pa = tma * pow_val;
    double factor_pa = (den_pa != 0.0) ? (num_pa / den_pa) : 0.0;
    double vpb = a * factor_pa;
    double vpl = vpb - inv_inicial;

    double num_ap = tma * pow_val;
    double den_ap = pow_val - 1.0;
    double factor_ap = (den_ap != 0.0) ? (num_ap / den_ap) : 0.0;
    double vaue = vpl * factor_ap;

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "ANÁLISE DE INVESTIMENTOS - VPL E VAUE") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1   = R$ %2\n").arg(QCoreApplication::translate("App", "Investimento Inicial"), TextFormat::format_currency(inv_inicial));
    steps << QString("  %1 (A)     = R$ %2 %3\n").arg(QCoreApplication::translate("App", "Fluxo de Caixa"), TextFormat::format_currency(a), QCoreApplication::translate("App", "por período"));
    steps << QString("  %1 (n)           = %2\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n));
    steps << QString("  TMA                    = %1% %2\n\n").arg(TextFormat::format_currency(tma * 100.0), QCoreApplication::translate("App", "ao período"));

    QString n_super = TextFormat::to_superscript(static_cast<int>(n));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VPB (Valor Presente dos Benefícios)"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    auto f = TextFormat::format_fraction("(1 + TMA)ⁿ - 1", "TMA × (1 + TMA)ⁿ", "  VPB = A × ");
    steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

    steps << QCoreApplication::translate("App", "Cálculo do fator (P/A):") + "\n";
    steps << QString("  (1 + TMA)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(tma), n_super);
    steps << QString("  (1 + TMA)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val));

    steps << QString("  %1   = (1+TMA)ⁿ - 1 = %2 - 1 = %3\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(pow_val), TextFormat::format_currency(num_pa));
    steps << QString("  %1 = TMA × (1+TMA)ⁿ = %2 × %3 = %4\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(tma), TextFormat::format_currency(pow_val), TextFormat::format_currency(den_pa));
    auto nf = TextFormat::format_fraction(TextFormat::format_currency(num_pa), TextFormat::format_currency(den_pa), QString("  %1 (P/A) = ").arg(QCoreApplication::translate("App", "Fator")));
    steps << nf[0] + "\n" << nf[1] + "\n" << nf[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(factor_pa));

    steps << QCoreApplication::translate("App", "Cálculo do VPB:") + "\n";
    steps << QString("  VPB = A × %1(P/A)\n").arg(QCoreApplication::translate("App", "Fator"));
    steps << QString("  VPB = %1 × %2\n").arg(TextFormat::format_currency(a), TextFormat::format_currency(factor_pa));
    steps << QString("  VPB = R$ %1\n\n").arg(TextFormat::format_currency(vpb));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VPL (Valor Presente Líquido)"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    steps << "  VPL = VPB - VPC\n";
    steps << QString("  VPC = %1\n\n").arg(QCoreApplication::translate("App", "Investimento Inicial"));

    steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
    steps << QString("  VPC = R$ %1\n").arg(TextFormat::format_currency(inv_inicial));
    steps << QString("  VPL = %1 - %2\n").arg(TextFormat::format_currency(vpb), TextFormat::format_currency(inv_inicial));
    steps << QString("  VPL = R$ %1\n\n").arg(TextFormat::format_currency(vpl));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DA VAUE (Valor Anual Uniforme Equivalente)"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    steps << QString("  VAUE = VPL × %1(A/P)\n\n").arg(QCoreApplication::translate("App", "Fator"));

    steps << QCoreApplication::translate("App", "Cálculo do fator (A/P):") + "\n";
    steps << QString("  %1   = TMA × (1+TMA)ⁿ = %2 × %3 = %4\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(tma), TextFormat::format_currency(pow_val), TextFormat::format_currency(num_ap));
    steps << QString("  %1 = (1+TMA)ⁿ - 1 = %2 - 1 = %3\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(pow_val), TextFormat::format_currency(den_ap));
    auto af = TextFormat::format_fraction(TextFormat::format_currency(num_ap), TextFormat::format_currency(den_ap), QString("  %1 (A/P) = ").arg(QCoreApplication::translate("App", "Fator")));
    steps << af[0] + "\n" << af[1] + "\n" << af[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(factor_ap));

    steps << QCoreApplication::translate("App", "Cálculo da VAUE:") + "\n";
    steps << QString("  VAUE = %1 × %2\n").arg(TextFormat::format_currency(vpl), TextFormat::format_currency(factor_ap));
    steps << QString("  VAUE = R$ %1\n\n").arg(TextFormat::format_currency(vaue));

    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "CONCLUSÃO") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    if (vpl > 0.0) {
        steps << QString("  VPL = R$ %1 > 0\n").arg(TextFormat::format_currency(vpl));
        steps << QString("  VAUE = R$ %1 > 0\n\n").arg(TextFormat::format_currency(vaue));
        steps << QString("  ✓ %1\n").arg(QCoreApplication::translate("App", "O projeto é VIÁVEL economicamente"));
        steps << QString("  ✓ %1\n").arg(QCoreApplication::translate("App", "O investimento proporciona retorno acima da TMA"));
    } else if (vpl < 0.0) {
        steps << QString("  VPL = R$ %1 < 0\n").arg(TextFormat::format_currency(vpl));
        steps << QString("  VAUE = R$ %1 < 0\n\n").arg(TextFormat::format_currency(vaue));
        steps << QString("  ✗ %1\n").arg(QCoreApplication::translate("App", "O projeto é INVIÁVEL economicamente"));
        steps << QString("  ✗ %1\n").arg(QCoreApplication::translate("App", "O investimento não atinge a TMA desejada"));
    } else {
        steps << QString("  VPL = R$ %1 = 0\n").arg(TextFormat::format_currency(vpl));
        steps << QString("  VAUE = R$ %1 = 0\n\n").arg(TextFormat::format_currency(vaue));
        steps << QString("  ~ %1\n").arg(QCoreApplication::translate("App", "O projeto está no limite de viabilidade"));
        steps << QString("  ~ %1\n").arg(QCoreApplication::translate("App", "O investimento retorna exatamente a TMA"));
    }

    steps << "\n" + QString(60, QChar(0x2500)) + "\n";
    if (app->invest_result) {
        app->invest_result->append(steps.join(""));
    }
}

static void calculate_vpl_detailed(FinancialCalculatorApp* app) {
    double inv_inicial = app->get_float_from_line_edit(app->invest_initial);
    double receita_anual = app->get_float_from_line_edit(app->invest_annual_revenue);
    double custo_anual = app->get_float_from_line_edit(app->invest_annual_cost);
    int n = static_cast<int>(app->get_float_from_line_edit(app->invest_n));
    double tma = app->get_float_from_line_edit(app->invest_tma, true);

    double fluxo_liquido = receita_anual - custo_anual;
    double pow_val = std::pow(1.0 + tma, n);
    double num_pa = pow_val - 1.0;
    double den_pa = tma * pow_val;
    double factor_pa = (den_pa != 0.0) ? (num_pa / den_pa) : 0.0;
    double vpb = fluxo_liquido * factor_pa;
    double vpl = vpb - inv_inicial;

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "VPL DETALHADO (RECEITAS E CUSTOS SEPARADOS)") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Investimento Inicial (C₀)"), TextFormat::format_currency(inv_inicial));
    steps << QString("  %1:            R$ %2\n").arg(QCoreApplication::translate("App", "Receita Anual"), TextFormat::format_currency(receita_anual));
    steps << QString("  %1:   R$ %2\n").arg(QCoreApplication::translate("App", "Custo/Desembolso Anual"), TextFormat::format_currency(custo_anual));
    steps << QString("  %1:  R$ %2\n").arg(QCoreApplication::translate("App", "Fluxo Líquido Anual (A)"), TextFormat::format_currency(fluxo_liquido));
    steps << QString("  %1 (n):             %2\n").arg(QCoreApplication::translate("App", "Períodos")).arg(n);
    steps << QString("  TMA:                        %1% %2\n\n").arg(TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano"));

    QString n_super = TextFormat::to_superscript(n);

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO FLUXO LÍQUIDO"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QString("  A = %1 - %2\n").arg(QCoreApplication::translate("App", "Receita"), QCoreApplication::translate("App", "Custo"));
    steps << QString("  A = %1 - %2\n").arg(TextFormat::format_currency(receita_anual), TextFormat::format_currency(custo_anual));
    steps << QString("  A = R$ %1\n\n").arg(TextFormat::format_currency(fluxo_liquido));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO FATOR (P/A)"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    auto f = TextFormat::format_fraction("(1 + i)ⁿ - 1", "i × (1 + i)ⁿ", "  (P/A; i; n) = ");
    steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

    steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
    steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(tma, 6), n_super);
    steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val, 6));

    steps << QString("  %1   = (1+i)ⁿ - 1\n").arg(QCoreApplication::translate("App", "Numerador"));
    steps << QString("                = %1 - 1\n").arg(TextFormat::format_currency(pow_val, 6));
    steps << QString("                = %1\n\n").arg(TextFormat::format_currency(num_pa, 6));

    steps << QString("  %1 = i × (1+i)ⁿ\n").arg(QCoreApplication::translate("App", "Denominador"));
    steps << QString("                = %1 × %2\n").arg(TextFormat::format_currency(tma, 6), TextFormat::format_currency(pow_val, 6));
    steps << QString("                = %1\n\n").arg(TextFormat::format_currency(den_pa, 6));

    auto nf = TextFormat::format_fraction(
        TextFormat::format_currency(num_pa, 6),
        TextFormat::format_currency(den_pa, 6),
        QString("  (P/A; %1%; %2) = ").arg(TextFormat::format_currency(tma * 100.0, 2)).arg(n)
    );
    steps << nf[0] + "\n" << nf[1] + "\n" << nf[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(factor_pa, 6));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VPB (Valor Presente dos Benefícios)"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << "  VPB = A × (P/A; i; n)\n";
    steps << QString("  VPB = %1 × %2\n").arg(TextFormat::format_currency(fluxo_liquido), TextFormat::format_currency(factor_pa, 6));
    steps << QString("  VPB = R$ %1\n\n").arg(TextFormat::format_currency(vpb));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("4. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VPL (Valor Presente Líquido)"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    steps << "  VPL = VPB - VPC\n";
    steps << "  VPL = VPB - C₀\n\n";

    steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
    steps << QString("  VPL = %1 - %2\n").arg(TextFormat::format_currency(vpb), TextFormat::format_currency(inv_inicial));
    steps << QString("  VPL = R$ %1\n\n").arg(TextFormat::format_currency(vpl));

    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
    steps << QString("  %1 R$ %2\n").arg(QCoreApplication::translate("App", "O Valor Presente Líquido é de"), TextFormat::format_currency(vpl, 2));
    steps << QString(60, QChar(0x2550)) + "\n";

    if (app->invest_result) {
        app->invest_result->append(steps.join(""));
    }
}

static void calculate_payback_discounted(FinancialCalculatorApp* app) {
    double inv_inicial = app->get_float_from_line_edit(app->invest_initial);
    double fluxo_anual = app->get_float_from_line_edit(app->invest_cashflow);
    int n_max = static_cast<int>(app->get_float_from_line_edit(app->invest_n));
    double tma = app->get_float_from_line_edit(app->invest_tma, true);

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "PAYBACK DESCONTADO (PERÍODO DE RECUPERAÇÃO)") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Investimento Inicial (C₀)"), TextFormat::format_currency(inv_inicial));
    steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Fluxo de Caixa Anual (A)"), TextFormat::format_currency(fluxo_anual));
    steps << QString("  TMA:                         %1% %2\n").arg(TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano"));
    steps << QString("  %1:  %2 %3\n\n").arg(QCoreApplication::translate("App", "Período máximo analisado")).arg(n_max).arg(QCoreApplication::translate("App", "anos"));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QCoreApplication::translate("App", "OBJETIVO:") + "\n";
    steps << QString(60, QChar(0x2500)) + "\n\n";

    double target = (fluxo_anual != 0.0) ? (inv_inicial / fluxo_anual) : 0.0;
    steps << QString("  %1: A × (P/A; i; k) ≥ C₀\n").arg(QCoreApplication::translate("App", "Encontrar k tal que"));
    steps << QString("  %1: (P/A; i; k) ≥ %2 / %3\n").arg(QCoreApplication::translate("App", "Ou seja"), TextFormat::format_currency(inv_inicial), TextFormat::format_currency(fluxo_anual));
    steps << QString("  (P/A; i; k) ≥ %1\n\n").arg(TextFormat::format_currency(target, 6));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QCoreApplication::translate("App", "CÁLCULO DOS FATORES (P/A) E VP ACUMULADO:") + "\n";
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QString("%1 %2 %3 %4 %5\n")
             .arg(QCoreApplication::translate("App", "Ano"), -6)
             .arg("(P/F)", -12)
             .arg(QCoreApplication::translate("App", "VP Anual"), -15)
             .arg(QCoreApplication::translate("App", "VP Acum."), -15)
             .arg("(P/A)", -12);
    steps << QString(60, QChar(0x2500)) + "\n";

    double vp_acumulado = 0.0;
    int payback_year = -1;

    for (int k = 1; k <= n_max; ++k) {
        double factor_pf = 1.0 / std::pow(1.0 + tma, k);
        double vp_anual = fluxo_anual * factor_pf;
        vp_acumulado += vp_anual;
        double factor_pa_k = (fluxo_anual != 0.0) ? (vp_acumulado / fluxo_anual) : 0.0;

        steps << QString("%1 %2 R$ %3 R$ %4 %5\n")
                 .arg(QString::number(k), -6)
                 .arg(TextFormat::format_currency(factor_pf, 6), -12)
                 .arg(TextFormat::format_currency(vp_anual), -13)
                 .arg(TextFormat::format_currency(vp_acumulado), -13)
                 .arg(TextFormat::format_currency(factor_pa_k, 6), -12);

        if (payback_year == -1 && vp_acumulado >= inv_inicial) {
            payback_year = k;
        }
    }

    steps << "\n";
    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QCoreApplication::translate("App", "ANÁLISE:") + "\n";
    steps << QString(60, QChar(0x2500)) + "\n\n";

    if (payback_year != -1) {
        double vp_ano_anterior = 0.0;
        for (int k = 1; k < payback_year; ++k) {
            vp_ano_anterior += fluxo_anual * (1.0 / std::pow(1.0 + tma, k));
        }
        double vp_ano_payback = 0.0;
        for (int k = 1; k <= payback_year; ++k) {
            vp_ano_payback += fluxo_anual * (1.0 / std::pow(1.0 + tma, k));
        }

        steps << QString("  %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Investimento inicial"), TextFormat::format_currency(inv_inicial));
        if (payback_year > 1) {
            steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "VP acumulado até ano")).arg(payback_year - 1).arg(TextFormat::format_currency(vp_ano_anterior));
        }
        steps << QString("  %1 %2: R$ %3\n\n").arg(QCoreApplication::translate("App", "VP acumulado até ano")).arg(payback_year).arg(TextFormat::format_currency(vp_ano_payback));
        steps << QString("  %1 %2.\n\n").arg(QCoreApplication::translate("App", "O investimento é recuperado durante o ano")).arg(payback_year);

        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
        steps << QString("  %1: %2 %3\n").arg(QCoreApplication::translate("App", "Payback Descontado")).arg(payback_year).arg(QCoreApplication::translate("App", "anos"));
        steps << QString(60, QChar(0x2550)) + "\n";
    } else {
        steps << QString("  %1 %2 %3.\n").arg(QCoreApplication::translate("App", "O investimento NÃO é recuperado em")).arg(n_max).arg(QCoreApplication::translate("App", "anos"));
        steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "VP acumulado máximo"), TextFormat::format_currency(vp_acumulado));
        steps << QString("  %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Investimento inicial"), TextFormat::format_currency(inv_inicial));

        steps << QString(60, QChar(0x2550)) + "\n";
        steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
        steps << QString("  %1 %2 %3\n").arg(QCoreApplication::translate("App", "Payback Descontado: Não recuperado em")).arg(n_max).arg(QCoreApplication::translate("App", "anos"));
        steps << QString(60, QChar(0x2550)) + "\n";
    }

    if (app->invest_result) {
        app->invest_result->append(steps.join(""));
    }
}

static void calculate_sensitivity_analysis(FinancialCalculatorApp* app) {
    double inv_inicial = app->get_float_from_line_edit(app->invest_initial);
    double receita_base = app->get_float_from_line_edit(app->invest_annual_revenue);
    double custo_anual = app->get_float_from_line_edit(app->invest_annual_cost);
    int n = static_cast<int>(app->get_float_from_line_edit(app->invest_n));
    double tma = app->get_float_from_line_edit(app->invest_tma, true);
    double variacao_percentual = app->get_float_from_line_edit(app->invest_sensitivity_variation, true);

    // VPL Base
    double fluxo_base = receita_base - custo_anual;
    double pow_val = std::pow(1.0 + tma, n);
    double num_pa = pow_val - 1.0;
    double den_pa = tma * pow_val;
    double factor_pa = (den_pa != 0.0) ? (num_pa / den_pa) : 0.0;
    double vpb_base = fluxo_base * factor_pa;
    double vpl_base = vpb_base - inv_inicial;

    // VPL com Variação
    double receita_nova = receita_base * (1.0 + variacao_percentual);
    double fluxo_novo = receita_nova - custo_anual;
    double vpb_novo = fluxo_novo * factor_pa;
    double vpl_novo = vpb_novo - inv_inicial;

    double variacao_vpl = (vpl_base != 0.0) ? (((vpl_novo - vpl_base) / vpl_base) * 100.0) : 0.0;

    QStringList steps;
    steps << QString(70, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "ANÁLISE DE SENSIBILIDADE DO VPL") + "\n";
    steps << QString(70, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Investimento Inicial (C₀)"), TextFormat::format_currency(inv_inicial));
    steps << QString("  %1:       R$ %2\n").arg(QCoreApplication::translate("App", "Receita Anual Base"), TextFormat::format_currency(receita_base));
    steps << QString("  %1:              R$ %2\n").arg(QCoreApplication::translate("App", "Custo Anual"), TextFormat::format_currency(custo_anual));
    steps << QString("  %1:   R$ %2\n").arg(QCoreApplication::translate("App", "Fluxo Líquido Base (A)"), TextFormat::format_currency(fluxo_base));
    steps << QString("  %1 (n):             %2 %3\n").arg(QCoreApplication::translate("App", "Períodos")).arg(n).arg(QCoreApplication::translate("App", "anos"));
    steps << QString("  TMA (i):                     %1% %2\n").arg(TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano"));
    steps << QString("  %1:      %2%\n\n").arg(QCoreApplication::translate("App", "Variação na Receita"), TextFormat::format_currency(variacao_percentual * 100.0, 2));

    QString n_super = TextFormat::to_superscript(n);

    steps << QString(70, QChar(0x2500)) + "\n";
    steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO FATOR (P/A)"));
    steps << QString(70, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    auto f = TextFormat::format_fraction("(1 + i)ⁿ - 1", "i × (1 + i)ⁿ", "  (P/A; i; n) = ");
    steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

    steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
    steps << QString("  (1 + i)ⁿ = (1 + %1)%2\n").arg(TextFormat::format_currency(tma, 6), n_super);
    steps << QString("  (1 + i)ⁿ = %1\n\n").arg(TextFormat::format_currency(pow_val, 6));

    steps << QString("  %1   = (1+i)ⁿ - 1 = %2 - 1\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(pow_val, 6));
    steps << QString("                = %1\n\n").arg(TextFormat::format_currency(num_pa, 6));

    steps << QString("  %1 = i × (1+i)ⁿ = %2 × %3\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(tma, 6), TextFormat::format_currency(pow_val, 6));
    steps << QString("                = %1\n\n").arg(TextFormat::format_currency(den_pa, 6));

    auto nf = TextFormat::format_fraction(TextFormat::format_currency(num_pa, 6), TextFormat::format_currency(den_pa, 6), QString("  (P/A; %1%; %2) = ").arg(TextFormat::format_currency(tma * 100.0)).arg(n));
    steps << nf[0] + "\n" << nf[1] + "\n" << nf[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(factor_pa, 6));

    steps << QString(70, QChar(0x2500)) + "\n";
    steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VPL BASE (CENÁRIO ORIGINAL)"));
    steps << QString(70, QChar(0x2500)) + "\n\n";

    steps << QString("  %1 = %2 - %3\n").arg(QCoreApplication::translate("App", "Fluxo Líquido Base"), QCoreApplication::translate("App", "Receita"), QCoreApplication::translate("App", "Custo"));
    steps << QString("  A_base = %1 - %2\n").arg(TextFormat::format_currency(receita_base), TextFormat::format_currency(custo_anual));
    steps << QString("  A_base = R$ %1\n\n").arg(TextFormat::format_currency(fluxo_base));

    steps << "  VPB_base = A_base × (P/A; i; n)\n";
    steps << QString("  VPB_base = %1 × %2\n").arg(TextFormat::format_currency(fluxo_base), TextFormat::format_currency(factor_pa, 6));
    steps << QString("  VPB_base = R$ %1\n\n").arg(TextFormat::format_currency(vpb_base));

    steps << "  VPL_base = VPB_base - C₀\n";
    steps << QString("  VPL_base = %1 - %2\n").arg(TextFormat::format_currency(vpb_base), TextFormat::format_currency(inv_inicial));
    steps << QString("  VPL_base = R$ %1\n\n").arg(TextFormat::format_currency(vpl_base));

    steps << QString(70, QChar(0x2500)) + "\n";
    steps << QString("3. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO VPL COM VARIAÇÃO NA RECEITA"));
    steps << QString(70, QChar(0x2500)) + "\n\n";

    QString sinal = (variacao_percentual >= 0.0) ? "+" : "";
    steps << QString("  %1: %2%3%\n\n").arg(QCoreApplication::translate("App", "Variação aplicada"), sinal, TextFormat::format_currency(variacao_percentual * 100.0, 2));

    steps << QString("  %1 = %2 × (1 %3 %4)\n").arg(QCoreApplication::translate("App", "Nova Receita"), QCoreApplication::translate("App", "Receita Base"), sinal, TextFormat::format_currency(std::abs(variacao_percentual), 6));
    steps << QString("  %1 = %2 × %3\n").arg(QCoreApplication::translate("App", "Nova Receita"), TextFormat::format_currency(receita_base), TextFormat::format_currency(1.0 + variacao_percentual, 6));
    steps << QString("  %1 = R$ %2\n\n").arg(QCoreApplication::translate("App", "Nova Receita"), TextFormat::format_currency(receita_nova));

    steps << QString("  %1 = %2 - %3\n").arg(QCoreApplication::translate("App", "Novo Fluxo Líquido"), QCoreApplication::translate("App", "Nova Receita"), QCoreApplication::translate("App", "Custo"));
    steps << QString("  A_novo = %1 - %2\n").arg(TextFormat::format_currency(receita_nova), TextFormat::format_currency(custo_anual));
    steps << QString("  A_novo = R$ %1\n\n").arg(TextFormat::format_currency(fluxo_novo));

    steps << "  VPB_novo = A_novo × (P/A; i; n)\n";
    steps << QString("  VPB_novo = %1 × %2\n").arg(TextFormat::format_currency(fluxo_novo), TextFormat::format_currency(factor_pa, 6));
    steps << QString("  VPB_novo = R$ %1\n\n").arg(TextFormat::format_currency(vpb_novo));

    steps << "  VPL_novo = VPB_novo - C₀\n";
    steps << QString("  VPL_novo = %1 - %2\n").arg(TextFormat::format_currency(vpb_novo), TextFormat::format_currency(inv_inicial));
    steps << QString("  VPL_novo = R$ %1\n\n").arg(TextFormat::format_currency(vpl_novo));

    steps << QString(70, QChar(0x2500)) + "\n";
    steps << QString("4. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DA VARIAÇÃO PERCENTUAL DO VPL"));
    steps << QString(70, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    auto vf = TextFormat::format_fraction("VPL_novo - VPL_base", "VPL_base", QString("  %1 = ").arg(QCoreApplication::translate("App", "Variação %")));
    steps << vf[0] + " × 100\n";
    steps << vf[1] + "\n";
    steps << vf[2] + "\n\n";

    double diff_vpl = vpl_novo - vpl_base;
    steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
    steps << QString("  %1   = VPL_novo - VPL_base\n").arg(QCoreApplication::translate("App", "Numerador"));
    steps << QString("                = %1 - %2\n").arg(TextFormat::format_currency(vpl_novo), TextFormat::format_currency(vpl_base));
    steps << QString("                = R$ %1\n\n").arg(TextFormat::format_currency(diff_vpl));

    steps << QString("  %1 = VPL_base\n").arg(QCoreApplication::translate("App", "Denominador"));
    steps << QString("                = R$ %1\n\n").arg(TextFormat::format_currency(vpl_base));

    auto varf = TextFormat::format_fraction(TextFormat::format_currency(diff_vpl), TextFormat::format_currency(vpl_base), QString("  %1 = ").arg(QCoreApplication::translate("App", "Variação %")));
    steps << varf[0] + " × 100\n";
    steps << varf[1] + "\n";
    steps << varf[2] + "\n\n";

    double ratio = (vpl_base != 0.0) ? (diff_vpl / vpl_base) : 0.0;
    steps << QString("  %1 = %2 × 100\n").arg(QCoreApplication::translate("App", "Variação %"), TextFormat::format_currency(ratio, 6));
    steps << QString("  %1 = %2%\n\n").arg(QCoreApplication::translate("App", "Variação %"), TextFormat::format_currency(variacao_vpl, 2));

    steps << QString(70, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "RESUMO DA ANÁLISE DE SENSIBILIDADE") + "\n";
    steps << QString(70, QChar(0x2550)) + "\n\n";

    steps << QString("  VPL_base = R$ %1\n").arg(TextFormat::format_currency(vpl_base));
    steps << QString("  VPL_novo = R$ %1\n").arg(TextFormat::format_currency(vpl_novo));
    steps << QString("  %1 = R$ %2\n").arg(QCoreApplication::translate("App", "Variação do VPL"), TextFormat::format_currency(diff_vpl));
    steps << QString("  %1 = %2%\n\n").arg(QCoreApplication::translate("App", "Variação Percentual"), TextFormat::format_currency(variacao_vpl, 2));

    if (variacao_vpl > 0.0) {
        steps << QString("  ✓ %1\n").arg(QCoreApplication::translate("App", "O VPL aumentou com a variação na receita"));
    } else if (variacao_vpl < 0.0) {
        steps << QString("  ✗ %1\n").arg(QCoreApplication::translate("App", "O VPL diminuiu com a variação na receita"));
    } else {
        steps << QString("  = %1\n").arg(QCoreApplication::translate("App", "O VPL permaneceu inalterado"));
    }

    steps << "\n" + QString(70, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
    steps << QString("  %1\n").arg(QCoreApplication::translate("App", "A respectiva variação percentual do Valor Presente Líquido é de"));
    steps << QString("  %1%\n").arg(TextFormat::format_currency(variacao_vpl, 2));
    steps << QString(70, QChar(0x2550)) + "\n";

    if (app->invest_result) {
        app->invest_result->append(steps.join(""));
    }
}

void FinancialCalculatorApp::calculate_investment() {
    try {
        int analysis_type = invest_analysis_type->currentIndex();

        if (invest_initial->text().trimmed().isEmpty()) {
            if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Investimento Inicial é obrigatório"));
            return;
        }

        if (invest_n->text().trimmed().isEmpty()) {
            if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Número de Períodos é obrigatório"));
            return;
        }

        if (invest_tma->text().trimmed().isEmpty()) {
            if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: TMA é obrigatória"));
            return;
        }

        if (analysis_type == 0) { // VPL/VAUE Uniforme
            if (invest_cashflow->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Fluxo de Caixa é obrigatório"));
                return;
            }
            calculate_vpl_vaue_uniform(this);
        } else if (analysis_type == 1) { // VPL Detalhado
            if (invest_annual_revenue->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Receita Anual é obrigatória"));
                return;
            }
            if (invest_annual_cost->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Custo Anual é obrigatório"));
                return;
            }
            calculate_vpl_detailed(this);
        } else if (analysis_type == 2) { // Payback Descontado
            if (invest_cashflow->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Fluxo de Caixa é obrigatório"));
                return;
            }
            calculate_payback_discounted(this);
        } else { // Análise de Sensibilidade
            if (invest_annual_revenue->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Receita Anual é obrigatória"));
                return;
            }
            if (invest_annual_cost->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Custo Anual é obrigatório"));
                return;
            }
            if (invest_sensitivity_variation->text().trimmed().isEmpty()) {
                if (invest_result) invest_result->append(QCoreApplication::translate("App", "Erro: Variação Percentual é obrigatória"));
                return;
            }
            calculate_sensitivity_analysis(this);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular investimento: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (invest_result) {
            invest_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}

void FinancialCalculatorApp::_calculate_vpl_vaue_uniform() {
    calculate_vpl_vaue_uniform(this);
}

void FinancialCalculatorApp::_calculate_vpl_detailed() {
    calculate_vpl_detailed(this);
}

void FinancialCalculatorApp::_calculate_payback_discounted() {
    calculate_payback_discounted(this);
}

void FinancialCalculatorApp::_calculate_sensitivity_analysis() {
    calculate_sensitivity_analysis(this);
}
