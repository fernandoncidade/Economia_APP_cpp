#include "sv_12_calculate_vpl_with_taxes.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <algorithm>
#include <vector>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_vpl_with_taxes(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_vpl_with_taxes();
    }
}

static QString format_vpl_tax_steps(double investimento, double lucro_anual, int vida_util,
                                    double taxa_irpj, double taxa_csll, double tma,
                                    double valor_residual, int ano_venda, double valor_venda,
                                    bool financiado, double taxa_financiamento, int n_parcelas) {
    double taxa_imposto_total = taxa_irpj + taxa_csll;
    double dc_anual = (vida_util > 0) ? ((investimento - valor_residual) / vida_util) : 0.0;

    QStringList steps;
    steps << QString(70, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "VPL COM IMPOSTOS, DEPRECIAÇÃO E FINANCIAMENTO") + "\n";
    steps << QString(70, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "DADOS DO PROBLEMA:") + "\n";
    steps << QString(70, QChar(0x2500)) + "\n";
    steps << QString("  • %1: R$ %2\n").arg(QCoreApplication::translate("App", "Investimento inicial"), TextFormat::format_currency(investimento));
    steps << QString("  • %1: R$ %2 %3\n").arg(QCoreApplication::translate("App", "Lucro antes impostos e juros"), TextFormat::format_currency(lucro_anual), QCoreApplication::translate("App", "por ano"));
    steps << QString("  • %1: %2 %3\n").arg(QCoreApplication::translate("App", "Vida útil para depreciação")).arg(vida_util).arg(QCoreApplication::translate("App", "anos"));
    steps << QString("  • %1: R$ %2\n").arg(QCoreApplication::translate("App", "Valor residual"), TextFormat::format_currency(valor_residual));
    steps << QString("  • IRPJ: %1%\n").arg(TextFormat::format_currency(taxa_irpj * 100.0, 2));
    steps << QString("  • CSLL: %1%\n").arg(TextFormat::format_currency(taxa_csll * 100.0, 2));
    steps << QString("  • %1 (IRPJ+CSLL): %2%\n").arg(QCoreApplication::translate("App", "Imposto total"), TextFormat::format_currency(taxa_imposto_total * 100.0, 2));
    steps << QString("  • TMA: %1% %2\n").arg(TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano"));

    if (financiado) {
        steps << QString("  • %1: %2\n").arg(QCoreApplication::translate("App", "Forma de pagamento"), QCoreApplication::translate("App", "Financiamento pelo Sistema SAC"));
        steps << QString("  • %1: %2% %3\n").arg(QCoreApplication::translate("App", "Taxa de juros do financiamento"), TextFormat::format_currency(taxa_financiamento * 100.0, 2), QCoreApplication::translate("App", "ao ano"));
        steps << QString("  • %1: %2 %3\n").arg(QCoreApplication::translate("App", "Número de parcelas")).arg(n_parcelas).arg(QCoreApplication::translate("App", "anos"));
    } else {
        steps << QString("  • %1: %2\n").arg(QCoreApplication::translate("App", "Forma de pagamento"), QCoreApplication::translate("App", "À vista"));
    }

    if (valor_venda > 0.0) {
        steps << QString("  • %1: %2\n").arg(QCoreApplication::translate("App", "Venda no ano")).arg(ano_venda);
        steps << QString("  • %1: R$ %2\n").arg(QCoreApplication::translate("App", "Valor de venda"), TextFormat::format_currency(valor_venda));
    }
    steps << "\n";

    // ETAPA 1
    steps << QString(70, QChar(0x2550)) + "\n";
    steps << QString("%1 1: %2\n").arg(QCoreApplication::translate("App", "ETAPA"), QCoreApplication::translate("App", "CÁLCULO DA DEPRECIAÇÃO CONTÁBIL ANUAL"));
    steps << QString(70, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "A depreciação contábil (DC) é calculada pelo método linear:") + "\n\n";
    steps << QString("  %1: %2\n\n").arg(QCoreApplication::translate("App", "Fórmula"), QCoreApplication::translate("App", "DC = (Valor de Aquisição - Valor Residual) / Vida Útil"));

    auto f_dc = TextFormat::format_fraction(QString("%1 - %2").arg(TextFormat::format_currency(investimento), TextFormat::format_currency(valor_residual)), QString::number(vida_util), "  DC = ");
    steps << f_dc[0] + "\n" << f_dc[1] + "\n" << f_dc[2] + "\n\n";
    steps << QString("  DC = %1 / %2\n").arg(TextFormat::format_currency(investimento - valor_residual)).arg(vida_util);
    steps << QString("  DC = R$ %1 %2\n\n").arg(TextFormat::format_currency(dc_anual), QCoreApplication::translate("App", "por ano"));
    steps << QCoreApplication::translate("App", "A depreciação contábil é dedutível da base tributável.") + "\n\n";

    // ETAPA 2: Amortização
    struct AmortDado {
        int ano;
        double prestacao;
        double amortizacao;
        double juros;
        double saldo;
    };
    std::vector<AmortDado> amortizacao_dados;

    if (financiado) {
        steps << QString(70, QChar(0x2550)) + "\n";
        steps << QString("%1 2: %2\n").arg(QCoreApplication::translate("App", "ETAPA"), QCoreApplication::translate("App", "TABELA DE AMORTIZAÇÃO PELO SISTEMA SAC (Sistema de Amortização Constante)"));
        steps << QString(70, QChar(0x2550)) + "\n\n";

        steps << QCoreApplication::translate("App", "No Sistema SAC, a amortização é constante em todos os períodos:") + "\n\n";
        steps << QString("  %1: a = P / n\n\n").arg(QCoreApplication::translate("App", "Fórmula"));

        double amortizacao_constante = (n_parcelas > 0) ? (investimento / n_parcelas) : 0.0;
        auto f_sac = TextFormat::format_fraction(TextFormat::format_currency(investimento), QString::number(n_parcelas), "  a = ");
        steps << f_sac[0] + "\n" << f_sac[1] + "\n" << f_sac[2] + "\n\n";
        steps << QString("  %1: a = R$ %2\n\n").arg(QCoreApplication::translate("App", "Amortização constante"), TextFormat::format_currency(amortizacao_constante));

        steps << QCoreApplication::translate("App", "Os juros de cada período são calculados sobre o saldo devedor do período anterior:") + "\n";
        steps << "  j(k) = i × SD(k-1)\n\n";
        steps << QCoreApplication::translate("App", "A prestação é a soma da amortização constante com os juros:") + "\n";
        steps << "  p(k) = a + j(k)\n\n";

        steps << QString("%1 | %2 | %3 | %4 | %5 | %6\n")
                 .arg(QCoreApplication::translate("App", "Ano"), 4)
                 .arg(QCoreApplication::translate("App", "Saldo Inicial"), 18)
                 .arg(QCoreApplication::translate("App", "Juros"), 15)
                 .arg(QCoreApplication::translate("App", "Amortização"), 15)
                 .arg(QCoreApplication::translate("App", "Prestação"), 15)
                 .arg(QCoreApplication::translate("App", "Saldo Final"), 18);
        steps << QString(70, QChar(0x2500)) + "\n";

        double saldo_devedor = investimento;
        steps << QString("%1 | %2 | %3 | %4 | %5 | %6\n")
                 .arg(0, 4)
                 .arg(TextFormat::format_currency(saldo_devedor), 18)
                 .arg("-", 15)
                 .arg("-", 15)
                 .arg("-", 15)
                 .arg(TextFormat::format_currency(saldo_devedor), 18);

        for (int ano = 1; ano <= n_parcelas; ++ano) {
            double saldo_inicial = saldo_devedor;
            double juros = saldo_devedor * taxa_financiamento;
            double prestacao = amortizacao_constante + juros;
            saldo_devedor -= amortizacao_constante;

            amortizacao_dados.push_back({ano, prestacao, amortizacao_constante, juros, std::max(0.0, saldo_devedor)});

            steps << QString("%1 | %2 | %3 | %4 | %5 | %6\n")
                     .arg(ano, 4)
                     .arg(TextFormat::format_currency(saldo_inicial), 18)
                     .arg(TextFormat::format_currency(juros), 15)
                     .arg(TextFormat::format_currency(amortizacao_constante), 15)
                     .arg(TextFormat::format_currency(prestacao), 15)
                     .arg(TextFormat::format_currency(std::max(0.0, saldo_devedor)), 18);
        }

        steps << "\n";
        steps << QCoreApplication::translate("App", "Cálculos detalhados por período:") + "\n\n";

        saldo_devedor = investimento;
        for (int ano = 1; ano <= n_parcelas; ++ano) {
            double j = saldo_devedor * taxa_financiamento;
            double p_parc = amortizacao_constante + j;
            steps << QString("  %1 %2:\n").arg(QCoreApplication::translate("App", "Ano")).arg(ano);
            steps << QString("    j(%1) = %2% × %3 = R$ %4\n").arg(ano).arg(TextFormat::format_currency(taxa_financiamento * 100.0, 2), TextFormat::format_currency(saldo_devedor), TextFormat::format_currency(j));
            steps << QString("    p(%1) = %2 + %3 = R$ %4\n").arg(ano).arg(TextFormat::format_currency(amortizacao_constante), TextFormat::format_currency(j), TextFormat::format_currency(p_parc));
            saldo_devedor -= amortizacao_constante;
            steps << QString("    SD(%1) = %2 - %3 = R$ %4\n\n").arg(ano).arg(TextFormat::format_currency(saldo_devedor + amortizacao_constante), TextFormat::format_currency(amortizacao_constante), TextFormat::format_currency(std::max(0.0, saldo_devedor)));
        }

        steps << QCoreApplication::translate("App", "IMPORTANTE: Os juros do financiamento são dedutíveis da base tributável.") + "\n\n";
    }

    // ETAPA 3 / 2
    steps << QString(70, QChar(0x2550)) + "\n";
    int numero_secao = financiado ? 3 : 2;
    steps << QString("%1 %2: %3\n").arg(QCoreApplication::translate("App", "ETAPA")).arg(numero_secao).arg(QCoreApplication::translate("App", "CÁLCULO DO FLUXO DE CAIXA COM IMPOSTOS"));
    steps << QString(70, QChar(0x2550)) + "\n\n";

    if (financiado) {
        steps << QCoreApplication::translate("App", "Fórmula da Renda Tributável (com financiamento):") + "\n";
        steps << QString("  %1\n\n").arg(QCoreApplication::translate("App", "Renda Tributável = Lucro Bruto - Depreciação Contábil - Juros do Financiamento ± Diferença Contábil"));
    } else {
        steps << QCoreApplication::translate("App", "Fórmula da Renda Tributável (sem financiamento):") + "\n";
        steps << QString("  %1\n\n").arg(QCoreApplication::translate("App", "Renda Tributável = Lucro Bruto - Depreciação Contábil ± Diferença Contábil"));
    }

    steps << QCoreApplication::translate("App", "Onde:") + "\n";
    steps << QString("  • %1 = %2 - %3\n").arg(QCoreApplication::translate("App", "Diferença Contábil"), QCoreApplication::translate("App", "Valor de Venda"), QCoreApplication::translate("App", "Valor Contábil"));
    steps << QString("  • %1 = %2 - (%3 × %4)\n").arg(QCoreApplication::translate("App", "Valor Contábil"), QCoreApplication::translate("App", "Valor de Aquisição"), QCoreApplication::translate("App", "Depreciação Anual"), QCoreApplication::translate("App", "Anos de Uso"));
    steps << QString("  • %1: %2\n").arg(QCoreApplication::translate("App", "Se Diferença Contábil < 0"), QCoreApplication::translate("App", "Perda de capital (dedutível)"));
    steps << QString("  • %1: %2\n\n").arg(QCoreApplication::translate("App", "Se Diferença Contábil > 0"), QCoreApplication::translate("App", "Ganho de capital (tributável)"));

    steps << QCoreApplication::translate("App", "Cálculo do Imposto:") + "\n";
    steps << QString("  %1 = %2 × %3%\n\n").arg(QCoreApplication::translate("App", "Imposto"), QCoreApplication::translate("App", "Renda Tributável"), TextFormat::format_currency(taxa_imposto_total * 100.0, 2));

    if (financiado) {
        steps << QCoreApplication::translate("App", "Cálculo do Fluxo de Caixa Líquido:") + "\n";
        steps << QString("  %1\n\n").arg(QCoreApplication::translate("App", "Fluxo Líquido = Lucro Bruto + Valor de Venda - Imposto - Prestação"));
    } else {
        steps << QCoreApplication::translate("App", "Cálculo do Fluxo de Caixa Líquido:") + "\n";
        steps << QString("  %1\n\n").arg(QCoreApplication::translate("App", "Fluxo Líquido = Lucro Bruto + Valor de Venda - Imposto"));
    }

    steps << QString("%1 | %2 | %3 | %4 | %5 | %6 | %7 | %8 | %9\n")
             .arg(QCoreApplication::translate("App", "Ano"), 4)
             .arg(QCoreApplication::translate("App", "Fluxo Bruto"), 13)
             .arg(QCoreApplication::translate("App", "DC"), 10)
             .arg(QCoreApplication::translate("App", "Juros"), 10)
             .arg(QCoreApplication::translate("App", "Dif.Cont."), 10)
             .arg(QCoreApplication::translate("App", "Renda Trib."), 13)
             .arg(QCoreApplication::translate("App", "Imposto"), 13)
             .arg(QCoreApplication::translate("App", "Prestação"), 13)
             .arg(QCoreApplication::translate("App", "Fluxo Líq."), 13);
    steps << QString(70, QChar(0x2500)) + "\n";

    double fluxo_ano_0 = financiado ? 0.0 : -investimento;
    steps << QString("%1 | %2 | %3 | %4 | %5 | %6 | %7 | %8 | %9\n")
             .arg(0, 4)
             .arg(TextFormat::format_currency(fluxo_ano_0), 13)
             .arg("-", 10)
             .arg("-", 10)
             .arg("-", 10)
             .arg("-", 13)
             .arg("-", 13)
             .arg("-", 13)
             .arg(TextFormat::format_currency(fluxo_ano_0), 13);

    std::vector<double> fluxos_liquidos = {fluxo_ano_0};
    int max_anos = std::max(ano_venda, financiado ? n_parcelas : ano_venda);

    for (int ano = 1; ano <= max_anos; ++ano) {
        double fluxo_bruto = (ano <= ano_venda) ? lucro_anual : 0.0;
        double deprec = (ano <= vida_util) ? dc_anual : 0.0;
        double juros_financ = (financiado && ano <= n_parcelas) ? amortizacao_dados[ano - 1].juros : 0.0;
        double prestacao = (financiado && ano <= n_parcelas) ? amortizacao_dados[ano - 1].prestacao : 0.0;
        double diferenca_cont = 0.0;

        if (ano == ano_venda && valor_venda > 0.0) {
            double vc = investimento - (ano * dc_anual);
            diferenca_cont = valor_venda - vc;
            fluxo_bruto += valor_venda;
        }

        double lucro_tributavel = lucro_anual - deprec - juros_financ + diferenca_cont;
        double imposto = lucro_tributavel * taxa_imposto_total;
        double fluxo_liquido = fluxo_bruto - imposto - prestacao;

        fluxos_liquidos.push_back(fluxo_liquido);

        QString juros_str = (juros_financ != 0.0) ? TextFormat::format_currency(juros_financ) : "-";
        QString dif_str = (diferenca_cont != 0.0) ? TextFormat::format_currency(diferenca_cont) : "-";
        QString prest_str = (prestacao != 0.0) ? TextFormat::format_currency(prestacao) : "-";

        steps << QString("%1 | %2 | %3 | %4 | %5 | %6 | %7 | %8 | %9\n")
                 .arg(ano, 4)
                 .arg(TextFormat::format_currency(fluxo_bruto), 13)
                 .arg(TextFormat::format_currency(deprec), 10)
                 .arg(juros_str, 10)
                 .arg(dif_str, 10)
                 .arg(TextFormat::format_currency(lucro_tributavel), 13)
                 .arg(TextFormat::format_currency(imposto), 13)
                 .arg(prest_str, 13)
                 .arg(TextFormat::format_currency(fluxo_liquido), 13);
    }

    steps << "\n";
    steps << QCoreApplication::translate("App", "CÁLCULOS DETALHADOS POR ANO:") + "\n";
    steps << QString(70, QChar(0x2500)) + "\n\n";

    if (!financiado) {
        steps << QString("  %1 0:\n").arg(QCoreApplication::translate("App", "Ano"));
        steps << QString("    %1: -R$ %2\n").arg(QCoreApplication::translate("App", "Investimento inicial pago à vista"), TextFormat::format_currency(investimento));
        steps << QString("    %1: -R$ %2\n\n").arg(QCoreApplication::translate("App", "Fluxo de caixa"), TextFormat::format_currency(investimento));
    } else {
        steps << QString("  %1 0:\n").arg(QCoreApplication::translate("App", "Ano"));
        steps << QString("    %1\n").arg(QCoreApplication::translate("App", "Investimento 100% financiado - sem desembolso inicial"));
        steps << QString("    %1: R$ 0,00\n\n").arg(QCoreApplication::translate("App", "Fluxo de caixa"));
    }

    for (int ano = 1; ano <= max_anos; ++ano) {
        double fluxo_bruto = (ano <= ano_venda) ? lucro_anual : 0.0;
        double deprec = (ano <= vida_util) ? dc_anual : 0.0;
        double juros_financ = (financiado && ano <= n_parcelas) ? amortizacao_dados[ano - 1].juros : 0.0;
        double prestacao = (financiado && ano <= n_parcelas) ? amortizacao_dados[ano - 1].prestacao : 0.0;
        double diferenca_cont = 0.0;

        steps << QString("  %1 %2:\n").arg(QCoreApplication::translate("App", "Ano")).arg(ano);

        if (ano == ano_venda && valor_venda > 0.0) {
            double vc = investimento - (ano * dc_anual);
            diferenca_cont = valor_venda - vc;
            fluxo_bruto += valor_venda;

            steps << QString("    %1: R$ %2\n").arg(QCoreApplication::translate("App", "Lucro bruto operacional"), TextFormat::format_currency(lucro_anual));
            steps << QString("    %1: R$ %2\n").arg(QCoreApplication::translate("App", "Venda do ativo"), TextFormat::format_currency(valor_venda));
            steps << QString("    %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Fluxo bruto total"), TextFormat::format_currency(fluxo_bruto));

            steps << QString("    %1:\n").arg(QCoreApplication::translate("App", "Cálculo da Diferença Contábil"));
            steps << QString("      %1 (VC%2): %3 - (%4 × %5) = R$ %6\n").arg(QCoreApplication::translate("App", "Valor Contábil"), TextFormat::to_subscript(ano), TextFormat::format_currency(investimento)).arg(ano).arg(TextFormat::format_currency(dc_anual), TextFormat::format_currency(vc));
            steps << QString("      %1: %2 - %3 = R$ %4\n").arg(QCoreApplication::translate("App", "Diferença Contábil"), TextFormat::format_currency(valor_venda), TextFormat::format_currency(vc), TextFormat::format_currency(diferenca_cont));

            if (diferenca_cont < 0.0) {
                steps << QString("      → %1 R$ %2 (%3)\n\n").arg(QCoreApplication::translate("App", "Perda de capital de"), TextFormat::format_currency(std::abs(diferenca_cont)), QCoreApplication::translate("App", "dedutível"));
            } else if (diferenca_cont > 0.0) {
                steps << QString("      → %1 R$ %2 (%3)\n\n").arg(QCoreApplication::translate("App", "Ganho de capital de"), TextFormat::format_currency(diferenca_cont), QCoreApplication::translate("App", "tributável"));
            } else {
                steps << QString("      → %1\n\n").arg(QCoreApplication::translate("App", "Sem ganho ou perda de capital"));
            }
        } else {
            steps << QString("    %1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Lucro bruto operacional"), TextFormat::format_currency(lucro_anual));
        }

        steps << QString("    %1:\n").arg(QCoreApplication::translate("App", "Cálculo da Renda Tributável"));
        QString rt_calc = QString("      %1 = %2 - %3").arg(QCoreApplication::translate("App", "Renda Tributável"), TextFormat::format_currency(lucro_anual), TextFormat::format_currency(deprec));
        if (juros_financ > 0.0) {
            rt_calc += QString(" - %1").arg(TextFormat::format_currency(juros_financ));
        }
        if (diferenca_cont != 0.0) {
            if (diferenca_cont > 0.0) {
                rt_calc += QString(" + %1").arg(TextFormat::format_currency(diferenca_cont));
            } else {
                rt_calc += QString(" - %1").arg(TextFormat::format_currency(std::abs(diferenca_cont)));
            }
        }
        double lucro_tributavel = lucro_anual - deprec - juros_financ + diferenca_cont;
        rt_calc += QString(" = R$ %1\n\n").arg(TextFormat::format_currency(lucro_tributavel));
        steps << rt_calc;

        double imposto = lucro_tributavel * taxa_imposto_total;
        steps << QString("    %1:\n").arg(QCoreApplication::translate("App", "Cálculo do Imposto"));
        steps << QString("      %1 = %2 × %3% = R$ %4\n\n").arg(QCoreApplication::translate("App", "Imposto"), TextFormat::format_currency(lucro_tributavel), TextFormat::format_currency(taxa_imposto_total * 100.0, 2), TextFormat::format_currency(imposto));

        double fluxo_liquido = fluxo_bruto - imposto - prestacao;
        steps << QString("    %1:\n").arg(QCoreApplication::translate("App", "Cálculo do Fluxo de Caixa Líquido"));
        QString fl_calc = QString("      %1 = %2 - %3").arg(QCoreApplication::translate("App", "Fluxo Líquido"), TextFormat::format_currency(fluxo_bruto), TextFormat::format_currency(imposto));
        if (prestacao > 0.0) {
            fl_calc += QString(" - %1").arg(TextFormat::format_currency(prestacao));
        }
        fl_calc += QString(" = R$ %1\n\n").arg(TextFormat::format_currency(fluxo_liquido));
        steps << fl_calc;
    }

    // ETAPA FINAL: VPL
    steps << QString(70, QChar(0x2550)) + "\n";
    numero_secao += 1;
    steps << QString("%1 %2: %3\n").arg(QCoreApplication::translate("App", "ETAPA")).arg(numero_secao).arg(QCoreApplication::translate("App", "CÁLCULO DO VALOR PRESENTE LÍQUIDO (VPL)"));
    steps << QString(70, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula do VPL:") + "\n\n";
    steps << QString("         n    %1\n").arg(QCoreApplication::translate("App", "Fluxo(k)"));
    steps << "  VPL = Σ  ─────────────\n";
    steps << QString("        k=0  (1 + %1)ᵏ\n\n").arg(QCoreApplication::translate("App", "TMA"));

    steps << QCoreApplication::translate("App", "Onde:") + "\n";
    steps << QString("  • n = %1 (%2)\n").arg(max_anos).arg(QCoreApplication::translate("App", "número de períodos"));
    steps << QString("  • TMA = %1% = %2 (%3)\n").arg(TextFormat::format_currency(tma * 100.0, 2), TextFormat::format_currency(tma, 6), QCoreApplication::translate("App", "taxa mínima de atratividade"));
    steps << QString("  • Fluxo(k) = %1\n\n").arg(QCoreApplication::translate("App", "Fluxo de caixa líquido no período k"));

    steps << QCoreApplication::translate("App", "CÁLCULO DETALHADO DO VPL:") + "\n";
    steps << QString(70, QChar(0x2500)) + "\n\n";

    double vpl = 0.0;
    for (size_t ano = 0; ano < fluxos_liquidos.size(); ++ano) {
        double fluxo = fluxos_liquidos[ano];
        double vp = 0.0;
        if (ano == 0) {
            vp = fluxo;
            if (financiado) {
                steps << QString("  %1 %2: VP₀ = %3 (%4)\n\n").arg(QCoreApplication::translate("App", "Ano")).arg(ano).arg(TextFormat::format_currency(fluxo), 15).arg(QCoreApplication::translate("App", "sem desembolso inicial"));
            } else {
                steps << QString("  %1 %2: VP₀ = %3 (%4)\n\n").arg(QCoreApplication::translate("App", "Ano")).arg(ano).arg(TextFormat::format_currency(fluxo), 15).arg(QCoreApplication::translate("App", "investimento inicial"));
            }
        } else {
            double fator = std::pow(1.0 + tma, static_cast<double>(ano));
            vp = fluxo / fator;
            steps << QString("  %1 %2:\n").arg(QCoreApplication::translate("App", "Ano")).arg(ano);
            steps << QString("    VP%1 = %2 / (1 + %3)%4\n").arg(TextFormat::to_subscript(static_cast<int>(ano)), TextFormat::format_currency(fluxo), TextFormat::format_currency(tma, 6), TextFormat::to_superscript(static_cast<int>(ano)));
            steps << QString("    VP%1 = %2 / %3\n").arg(TextFormat::to_subscript(static_cast<int>(ano)), TextFormat::format_currency(fluxo), TextFormat::format_currency(fator, 6));
            steps << QString("    VP%1 = R$ %2\n\n").arg(TextFormat::to_subscript(static_cast<int>(ano)), TextFormat::format_currency(vp));
        }
        vpl += vp;
    }

    steps << QString(70, QChar(0x2500)) + "\n\n";
    steps << QCoreApplication::translate("App", "SOMATÓRIO DOS VALORES PRESENTES:") + "\n\n";
    steps << "  VPL = ";
    for (size_t ano = 0; ano < fluxos_liquidos.size(); ++ano) {
        if (ano > 0) steps << " + ";
        steps << QString("VP%1").arg(TextFormat::to_subscript(static_cast<int>(ano)));
    }
    steps << "\n\n";
    steps << QString("  VPL = R$ %1\n\n").arg(TextFormat::format_currency(vpl, 2));

    steps << QString(70, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "ANÁLISE DO RESULTADO") + "\n";
    steps << QString(70, QChar(0x2550)) + "\n\n";

    if (vpl > 0.0) {
        steps << QString("  ✓ VPL > 0: %1\n\n").arg(QCoreApplication::translate("App", "PROJETO VIÁVEL ECONOMICAMENTE"));
        steps << QString("  %1 R$ %2\n").arg(QCoreApplication::translate("App", "O investimento gera um valor adicional de"), TextFormat::format_currency(vpl));
        steps << QString("  %1 %2% %3\n").arg(QCoreApplication::translate("App", "Isso significa que o retorno do projeto supera a TMA de"), TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano."));
        steps << QString("  %1.\n").arg(QCoreApplication::translate("App", "O projeto cria valor para a empresa e deve ser aceito"));
    } else if (vpl < 0.0) {
        steps << QString("  ✗ VPL < 0: %1\n\n").arg(QCoreApplication::translate("App", "PROJETO INVIÁVEL ECONOMICAMENTE"));
        steps << QString("  %1 R$ %2\n").arg(QCoreApplication::translate("App", "O investimento resulta em uma perda de"), TextFormat::format_currency(std::abs(vpl)));
        steps << QString("  %1 %2% %3\n").arg(QCoreApplication::translate("App", "Isso significa que o retorno do projeto é inferior à TMA de"), TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano."));
        steps << QString("  %1.\n").arg(QCoreApplication::translate("App", "O projeto destrói valor para a empresa e deve ser rejeitado"));
    } else {
        steps << QString("  VPL = 0: %1\n\n").arg(QCoreApplication::translate("App", "PROJETO NO LIMITE DE VIABILIDADE"));
        steps << QString("  %1 %2% %3\n").arg(QCoreApplication::translate("App", "O retorno do projeto é exatamente igual à TMA de"), TextFormat::format_currency(tma * 100.0, 2), QCoreApplication::translate("App", "ao ano."));
        steps << QString("  %1.\n").arg(QCoreApplication::translate("App", "O projeto não cria nem destrói valor"));
    }

    steps << "\n" + QString(70, QChar(0x2550)) + "\n";
    steps << QString("%1 %2 R$ %3\n").arg(QCoreApplication::translate("App", "RESPOSTA:"), QCoreApplication::translate("App", "O Valor Presente Líquido (VPL) é de"), TextFormat::format_currency(vpl, 2));
    steps << QString(70, QChar(0x2550)) + "\n";

    return steps.join("");
}

void FinancialCalculatorApp::calculate_vpl_with_taxes() {
    try {
        double investimento = get_float_from_line_edit(vpl_tax_investment);
        double lucro_anual = get_float_from_line_edit(vpl_tax_annual_profit);
        int vida_util = static_cast<int>(get_float_from_line_edit(vpl_tax_useful_life));
        double taxa_irpj = get_float_from_line_edit(vpl_tax_irpj, true);
        double taxa_csll = get_float_from_line_edit(vpl_tax_csll, true);
        double tma = get_float_from_line_edit(vpl_tax_tma, true);

        double valor_residual = get_float_from_line_edit(vpl_tax_residual_value, false, 0.0);
        int ano_venda = static_cast<int>(get_float_from_line_edit(vpl_tax_sale_year, false, static_cast<double>(vida_util)));
        double valor_venda = get_float_from_line_edit(vpl_tax_sale_value, false, 0.0);

        bool financiado = vpl_tax_financed && vpl_tax_financed->isChecked();
        double taxa_financiamento = 0.0;
        int n_parcelas = 0;

        if (financiado) {
            taxa_financiamento = get_float_from_line_edit(vpl_tax_finance_rate, true);
            n_parcelas = static_cast<int>(get_float_from_line_edit(vpl_tax_finance_periods));
        }

        if (ano_venda > vida_util) {
            if (vpl_tax_result) {
                vpl_tax_result->append(QCoreApplication::translate("App", "Erro: Ano de venda não pode ser maior que a vida útil"), nullptr, []() {
                    return QCoreApplication::translate("App", "Erro: Ano de venda não pode ser maior que a vida útil");
                });
            }
            return;
        }

        if (financiado && n_parcelas <= 0) {
            if (vpl_tax_result) {
                vpl_tax_result->append(QCoreApplication::translate("App", "Erro: Número de parcelas deve ser maior que zero"), nullptr, []() {
                    return QCoreApplication::translate("App", "Erro: Número de parcelas deve ser maior que zero");
                });
            }
            return;
        }

        QString result_text = format_vpl_tax_steps(investimento, lucro_anual, vida_util,
                                                    taxa_irpj, taxa_csll, tma,
                                                    valor_residual, ano_venda, valor_venda,
                                                    financiado, taxa_financiamento, n_parcelas);

        auto retranslate_fn = [investimento, lucro_anual, vida_util,
                               taxa_irpj, taxa_csll, tma,
                               valor_residual, ano_venda, valor_venda,
                               financiado, taxa_financiamento, n_parcelas]() -> QString {
            return format_vpl_tax_steps(investimento, lucro_anual, vida_util,
                                         taxa_irpj, taxa_csll, tma,
                                         valor_residual, ano_venda, valor_venda,
                                         financiado, taxa_financiamento, n_parcelas);
        };

        if (vpl_tax_result) {
            vpl_tax_result->append(result_text, nullptr, retranslate_fn);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular VPL com impostos: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (vpl_tax_result) {
            vpl_tax_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
