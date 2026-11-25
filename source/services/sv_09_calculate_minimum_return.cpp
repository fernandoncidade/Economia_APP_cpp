#include "sv_09_calculate_minimum_return.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../utils/MathRenderer.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_minimum_return(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_minimum_return();
    }
}

static QString format_minimum_return_steps(double aporte, double tma_anual, int periodos_ano) {
    double expoente = (periodos_ano != 0) ? (1.0 / periodos_ano) : 0.0;
    double tma_periodo = std::pow(1.0 + tma_anual, expoente) - 1.0;
    double juros_minimos = aporte * tma_periodo;

    QString sub_periodo_str = QCoreApplication::translate("App", "período");
    QString sub_anual_str = QCoreApplication::translate("App", "anual");
    QString sub_periodo = TextFormat::to_subscript(sub_periodo_str);
    QString sub_anual = TextFormat::to_subscript(sub_anual_str);

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "CÁLCULO DE RETORNO MÍNIMO BASEADO EM TMA") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Aporte (Investimento)"), TextFormat::format_currency(aporte));
    steps << QString("  %1: %2%\n").arg(QCoreApplication::translate("App", "TMA anual"), TextFormat::format_currency(tma_anual * 100.0, 2));
    steps << QString("  %1: %2\n\n").arg(QCoreApplication::translate("App", "Períodos por ano"), TextFormat::format_currency(periodos_ano, 0));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("1. %1\n").arg(QCoreApplication::translate("App", "CONVERSÃO DA TMA ANUAL PARA TMA DO PERÍODO"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula de equivalência:") + "\n";
    steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Relação fundamental de equivalência:"));
    steps << QString("  (1 + i%1)%2 = 1 + i%3\n\n").arg(sub_periodo, TextFormat::to_superscript("m"), sub_anual);

    steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Isolando a taxa do período:"));
    steps << QString("  i%1 = (1 + i%2)%3 - 1\n").arg(sub_periodo, sub_anual, TextFormat::to_superscript("(1/m)"));
    steps << TextFormat::render_radical_single_html("m", QString("1 + i_{%1}").arg(sub_anual_str), QString("  i_{%1} = ").arg(sub_periodo_str), " - 1") + "\n\n";

    steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Substituindo os valores:"));
    steps << QString("  i%1 = (1 + %2)%3 - 1\n").arg(sub_periodo, TextFormat::format_currency(tma_anual, 6), TextFormat::to_superscript(QString("(1/%1)").arg(periodos_ano)));
    steps << TextFormat::render_radical_single_html(QString::number(periodos_ano), QString("1 + %1").arg(TextFormat::format_currency(tma_anual, 6)), QString("  i_{%1} = ").arg(sub_periodo_str), " - 1") + "\n\n";

    QString exp_super = TextFormat::to_superscript(TextFormat::format_currency(expoente, 6));
    steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Efetuando o cálculo:"));
    steps << QString("  i%1 = (1 + %2)%3 - 1\n").arg(sub_periodo, TextFormat::format_currency(tma_anual, 6), exp_super);
    double pow_val = std::pow(1.0 + tma_anual, expoente);
    steps << QString("  i%1 = %2 - 1\n").arg(sub_periodo, TextFormat::format_currency(pow_val, 6));
    steps << QString("  i%1 = %2\n").arg(sub_periodo, TextFormat::format_currency(tma_periodo, 6));
    steps << QString("  i%1 = %2%\n\n").arg(sub_periodo, TextFormat::format_currency(tma_periodo * 100.0, 4));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("2. %1\n").arg(QCoreApplication::translate("App", "CÁLCULO DO RETORNO MÍNIMO POR PERÍODO"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    steps << QString("  %1 = %2 × i%3\n\n").arg(QCoreApplication::translate("App", "Juros"), QCoreApplication::translate("App", "Aporte"), sub_periodo);

    steps << QCoreApplication::translate("App", "Cálculo:") + "\n";
    steps << QString("  %1 = %2 × %3\n").arg(QCoreApplication::translate("App", "Juros"), TextFormat::format_currency(aporte), TextFormat::format_currency(tma_periodo, 6));
    steps << QString("  %1 = R$ %2\n\n").arg(QCoreApplication::translate("App", "Juros"), TextFormat::format_currency(juros_minimos));

    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
    steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Retorno mínimo por período"), TextFormat::format_currency(juros_minimos, 2));
    steps << QString(60, QChar(0x2550)) + "\n";

    return steps.join("");
}

void FinancialCalculatorApp::calculate_minimum_return() {
    try {
        double aporte = get_float_from_line_edit(min_return_investment);
        double tma_anual = get_float_from_line_edit(min_return_tma, true);
        int periodos_ano = static_cast<int>(get_float_from_line_edit(min_return_periods));

        QString result_text = format_minimum_return_steps(aporte, tma_anual, periodos_ano);

        auto retranslate_fn = [aporte, tma_anual, periodos_ano]() -> QString {
            return format_minimum_return_steps(aporte, tma_anual, periodos_ano);
        };

        if (min_return_result) {
            min_return_result->append(result_text, nullptr, retranslate_fn);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular retorno mínimo: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (min_return_result) {
            min_return_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
