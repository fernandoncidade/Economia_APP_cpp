#include "sv_10_calculate_fisher.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_fisher(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_fisher();
    }
}

static QString format_fisher_nominal_steps(double r, double theta) {
    double factor_r = 1.0 + r;
    double factor_theta = 1.0 + theta;
    double factor_i = factor_r * factor_theta;
    double i = factor_i - 1.0;

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "CÁLCULO DA TMA NOMINAL (RELAÇÃO DE FISHER)") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1:       %2% %3\n").arg(QCoreApplication::translate("App", "TMA Real (r)"), TextFormat::format_currency(r * 100.0, 4), QCoreApplication::translate("App", "ao ano"));
    steps << QString("  %1 (θ): %2% %3\n\n").arg(QCoreApplication::translate("App", "Taxa de Inflação"), TextFormat::format_currency(theta * 100.0, 4), QCoreApplication::translate("App", "ao ano"));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QCoreApplication::translate("App", "RELAÇÃO DE FISHER") + "\n";
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
    steps << "  1 + i = (1 + r) × (1 + θ)\n\n";

    steps << QString("  %1:\n").arg(QCoreApplication::translate("App", "Onde"));
    steps << QString("    i = %1\n").arg(QCoreApplication::translate("App", "Taxa Nominal (ou Aparente)"));
    steps << QString("    r = %1\n").arg(QCoreApplication::translate("App", "Taxa Real"));
    steps << QString("    θ = %1\n\n").arg(QCoreApplication::translate("App", "Taxa de Inflação"));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("%1:\n").arg(QCoreApplication::translate("App", "CÁLCULO"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QString("  1 + r = 1 + %1\n").arg(TextFormat::format_currency(r, 6));
    steps << QString("  1 + r = %1\n\n").arg(TextFormat::format_currency(factor_r, 6));

    steps << QString("  1 + θ = 1 + %1\n").arg(TextFormat::format_currency(theta, 6));
    steps << QString("  1 + θ = %1\n\n").arg(TextFormat::format_currency(factor_theta, 6));

    steps << "  1 + i = (1 + r) × (1 + θ)\n";
    steps << QString("  1 + i = %1 × %2\n").arg(TextFormat::format_currency(factor_r, 6), TextFormat::format_currency(factor_theta, 6));
    steps << QString("  1 + i = %1\n\n").arg(TextFormat::format_currency(factor_i, 6));

    steps << QString("  i = %1 - 1\n").arg(TextFormat::format_currency(factor_i, 6));
    steps << QString("  i = %1\n").arg(TextFormat::format_currency(i, 6));
    steps << QString("  i = %1%\n\n").arg(TextFormat::format_currency(i * 100.0, 4));

    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
    steps << QString("  %1 %2%\n").arg(QCoreApplication::translate("App", "A TMA Nominal para esse ano é de"), TextFormat::format_currency(i * 100.0, 2));
    steps << QString(60, QChar(0x2550)) + "\n";

    return steps.join("");
}

static void calculate_nominal_rate(FinancialCalculatorApp* app) {
    double r = app->get_float_from_line_edit(app->fisher_tma_real, true);
    double theta = app->get_float_from_line_edit(app->fisher_inflation, true);

    QString result_text = format_fisher_nominal_steps(r, theta);
    auto retranslate_fn = [r, theta]() -> QString {
        return format_fisher_nominal_steps(r, theta);
    };

    if (app->fisher_result) {
        app->fisher_result->append(result_text, nullptr, retranslate_fn);
    }
}

static QString format_fisher_real_steps(double i, double theta) {
    double factor_i = 1.0 + i;
    double factor_theta = 1.0 + theta;
    double factor_r = (factor_theta != 0.0) ? (factor_i / factor_theta) : 0.0;
    double r = factor_r - 1.0;

    QStringList steps;
    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "CÁLCULO DA TMA REAL (RELAÇÃO DE FISHER)") + "\n";
    steps << QString(60, QChar(0x2550)) + "\n\n";

    steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
    steps << QString("  %1:    %2% %3\n").arg(QCoreApplication::translate("App", "TMA Nominal (i)"), TextFormat::format_currency(i * 100.0, 4), QCoreApplication::translate("App", "ao ano"));
    steps << QString("  %1 (θ): %2% %3\n\n").arg(QCoreApplication::translate("App", "Taxa de Inflação"), TextFormat::format_currency(theta * 100.0, 4), QCoreApplication::translate("App", "ao ano"));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QCoreApplication::translate("App", "RELAÇÃO DE FISHER (REARRANJADA)") + "\n";
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QCoreApplication::translate("App", "Fórmula original:") + "\n";
    steps << "  1 + i = (1 + r) × (1 + θ)\n\n";

    steps << QCoreApplication::translate("App", "Rearranjando para isolar a taxa real:") + "\n";
    auto f = TextFormat::format_fraction("1 + i", "1 + θ", "  1 + r = ");
    steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

    steps << QString("  %1:\n").arg(QCoreApplication::translate("App", "Onde"));
    steps << QString("    r = %1\n").arg(QCoreApplication::translate("App", "Taxa Real"));
    steps << QString("    i = %1\n").arg(QCoreApplication::translate("App", "Taxa Nominal (ou Aparente)"));
    steps << QString("    θ = %1\n\n").arg(QCoreApplication::translate("App", "Taxa de Inflação"));

    steps << QString(60, QChar(0x2500)) + "\n";
    steps << QString("%1:\n").arg(QCoreApplication::translate("App", "CÁLCULO"));
    steps << QString(60, QChar(0x2500)) + "\n\n";

    steps << QString("  1 + i = 1 + %1\n").arg(TextFormat::format_currency(i, 6));
    steps << QString("  1 + i = %1\n\n").arg(TextFormat::format_currency(factor_i, 6));

    steps << QString("  1 + θ = 1 + %1\n").arg(TextFormat::format_currency(theta, 6));
    steps << QString("  1 + θ = %1\n\n").arg(TextFormat::format_currency(factor_theta, 6));

    auto nf = TextFormat::format_fraction(TextFormat::format_currency(factor_i, 6), TextFormat::format_currency(factor_theta, 6), "  1 + r = ");
    steps << nf[0] + "\n" << nf[1] + "\n" << nf[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(factor_r, 6));

    steps << QString("  r = %1 - 1\n").arg(TextFormat::format_currency(factor_r, 6));
    steps << QString("  r = %1\n").arg(TextFormat::format_currency(r, 6));
    steps << QString("  r = %1%\n\n").arg(TextFormat::format_currency(r * 100.0, 4));

    steps << QString(60, QChar(0x2550)) + "\n";
    steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
    steps << QString("  %1 %2%\n").arg(QCoreApplication::translate("App", "A TMA Real para esse ano é de"), TextFormat::format_currency(r * 100.0, 4));
    steps << QString(60, QChar(0x2550)) + "\n";

    return steps.join("");
}

static void calculate_real_rate_fisher(FinancialCalculatorApp* app) {
    double i = app->get_float_from_line_edit(app->fisher_tma_nominal, true);
    double theta = app->get_float_from_line_edit(app->fisher_inflation, true);

    QString result_text = format_fisher_real_steps(i, theta);
    auto retranslate_fn = [i, theta]() -> QString {
        return format_fisher_real_steps(i, theta);
    };

    if (app->fisher_result) {
        app->fisher_result->append(result_text, nullptr, retranslate_fn);
    }
}

void FinancialCalculatorApp::calculate_fisher() {
    try {
        int calc_type = fisher_calc_type->currentIndex();

        if (fisher_inflation->text().trimmed().isEmpty()) {
            if (fisher_result) {
                fisher_result->append(QCoreApplication::translate("App", "Erro: Taxa de Inflação é obrigatória"), nullptr, []() {
                    return QCoreApplication::translate("App", "Erro: Taxa de Inflação é obrigatória");
                });
            }
            return;
        }

        if (calc_type == 0) {
            if (fisher_tma_real->text().trimmed().isEmpty()) {
                if (fisher_result) {
                    fisher_result->append(QCoreApplication::translate("App", "Erro: TMA Real é obrigatória"), nullptr, []() {
                        return QCoreApplication::translate("App", "Erro: TMA Real é obrigatória");
                    });
                }
                return;
            }
            calculate_nominal_rate(this);
        } else {
            if (fisher_tma_nominal->text().trimmed().isEmpty()) {
                if (fisher_result) {
                    fisher_result->append(QCoreApplication::translate("App", "Erro: TMA Nominal é obrigatória"), nullptr, []() {
                        return QCoreApplication::translate("App", "Erro: TMA Nominal é obrigatória");
                    });
                }
                return;
            }
            calculate_real_rate_fisher(this);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular Fisher: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (fisher_result) {
            fisher_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}

void FinancialCalculatorApp::_calculate_nominal_rate() {
    calculate_nominal_rate(this);
}

void FinancialCalculatorApp::_calculate_real_rate_fisher() {
    calculate_real_rate_fisher(this);
}
