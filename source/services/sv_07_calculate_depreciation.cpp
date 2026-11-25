#include "sv_07_calculate_depreciation.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <numeric>
#include <stdexcept>
#include <algorithm>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_depreciation(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_depreciation();
    }
}

void FinancialCalculatorApp::calculate_depreciation() {
    try {
        if (!deprec_p || deprec_p->text().trimmed().isEmpty()) {
            throw std::runtime_error(QCoreApplication::translate("App", "Erro: Valor de Aquisição (P) é obrigatório.").toStdString());
        }
        if (!deprec_n || deprec_n->text().trimmed().isEmpty()) {
            throw std::runtime_error(QCoreApplication::translate("App", "Erro: Vida Útil (N) é obrigatória.").toStdString());
        }

        double p = get_float_from_line_edit(deprec_p);
        if (p <= 0.0) {
            throw std::runtime_error(QCoreApplication::translate("App", "Valor de aquisição deve ser positivo.").toStdString());
        }

        double vre = (deprec_vre && !deprec_vre->text().trimmed().isEmpty()) ? get_float_from_line_edit(deprec_vre) : 0.0;
        int n = static_cast<int>(get_float_from_line_edit(deprec_n));
        if (n <= 0) {
            throw std::runtime_error(QCoreApplication::translate("App", "Vida útil deve ser positiva.").toStdString());
        }

        // 0 = Linear, 1 = Soma Decrescente, 2 = Soma Crescente, 3 = Saldo Declinante
        int method_index = deprec_method ? deprec_method->currentIndex() : 0;

        QString k_text = deprec_k ? deprec_k->text().trimmed() : QString();
        bool has_k = !k_text.isEmpty();
        int k = has_k ? k_text.toInt() : 0;

        if (has_k && (k < 1 || k > n)) {
            throw std::runtime_error(QCoreApplication::translate("App", "O ano 'k' deve estar entre 1 e a Vida Útil (N).").toStdString());
        }

        QStringList steps;

        // ============================================================
        // 0: MÉTODO LINEAR
        // ============================================================
        if (method_index == 0) {
            double dr_anual = (p - vre) / n;

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "DEPRECIAÇÃO - MÉTODO LINEAR") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula:") + "\n";
            auto f = TextFormat::format_fraction("(P - VRE)", "N", "  DR = ");
            steps << f[0] + "\n" << f[1] + "\n" << f[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Valor inicial"), TextFormat::format_currency(p));
            steps << QString("  VRE (%1)   = R$ %2\n").arg(QCoreApplication::translate("App", "Valor residual"), TextFormat::format_currency(vre));
            steps << QString("  N (%1)          = %2 %3\n\n").arg(QCoreApplication::translate("App", "Vida útil"), TextFormat::format_currency(n, 0), QCoreApplication::translate("App", "anos"));

            steps << QCoreApplication::translate("App", "Cálculo da depreciação anual:") + "\n";
            auto cf = TextFormat::format_fraction(QString("(%1 - %2)").arg(TextFormat::format_currency(p), TextFormat::format_currency(vre)), TextFormat::format_currency(n, 0), "  DR = ");
            steps << cf[0] + "\n" << cf[1] + "\n" << cf[2] + QString(" = R$ %1\n\n").arg(TextFormat::format_currency(dr_anual));

            double vc_k = 0.0;
            if (has_k) {
                double deprec_acum = dr_anual * k;
                vc_k = p - deprec_acum;

                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QString("%1 k = %2\n").arg(QCoreApplication::translate("App", "Análise no ano")).arg(k);
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QString("  %1:      DR = R$ %2\n").arg(QCoreApplication::translate("App", "Depreciação anual"), TextFormat::format_currency(dr_anual));
                steps << QString("  %1: DR%2 = %3 × %4 = R$ %5\n").arg(QCoreApplication::translate("App", "Depreciação acumulada"), TextFormat::to_subscript("acum")).arg(k).arg(TextFormat::format_currency(dr_anual), TextFormat::format_currency(deprec_acum));
                steps << QString("  %1: VC%2 = P - DR%3 = %4 - %5 = R$ %6\n\n").arg(QCoreApplication::translate("App", "Valor Contábil"), TextFormat::to_subscript(k), TextFormat::to_subscript("acum"), TextFormat::format_currency(p), TextFormat::format_currency(deprec_acum), TextFormat::format_currency(vc_k));
            } else {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "Tabela de Depreciação Completa") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QString("%1 | %2 | %3 | %4 | %5\n")
                         .arg(QCoreApplication::translate("App", "Ano"), 4)
                         .arg(QCoreApplication::translate("App", "Fator"), 10)
                         .arg(QCoreApplication::translate("App", "DR (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "DR Acum (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "VC (R$)"), 15);
                steps << QString(80, QChar(0x2500)) + "\n";

                double deprec_acum = 0.0;
                for (int j = 1; j <= n; ++j) {
                    deprec_acum += dr_anual;
                    double vc_j = p - deprec_acum;
                    steps << QString("%1 | %2 | %3 | %4 | %5\n")
                             .arg(j, 4)
                             .arg(QString("1/%1").arg(n), 10)
                             .arg(TextFormat::format_currency(dr_anual), 15)
                             .arg(TextFormat::format_currency(deprec_acum), 15)
                             .arg(TextFormat::format_currency(vc_j), 15);
                }
                steps << QString(80, QChar(0x2500)) + "\n\n";
            }

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QString("%1 R$ %2\n").arg(QCoreApplication::translate("App", "RESPOSTA: Depreciação anual ="), TextFormat::format_currency(dr_anual));
            if (has_k) {
                steps << QString("          %1 %2 = R$ %3\n").arg(QCoreApplication::translate("App", "Valor Contábil no ano")).arg(k).arg(TextFormat::format_currency(vc_k));
            }
            steps << QString(60, QChar(0x2500)) + "\n";

        // ============================================================
        // 1: SOMA DOS DÍGITOS - DECRESCENTE
        // ============================================================
        } else if (method_index == 1) {
            double soma_digitos = (n * (n + 1.0)) / 2.0;

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "DEPRECIAÇÃO - SOMA DOS DÍGITOS (DECRESCENTE)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula da depreciação no ano n:") + "\n";
            auto f = TextFormat::format_fraction("(N - n + 1)", "S", QString("  DR%1 = ").arg(TextFormat::to_subscript("n")));
            steps << f[0] + " × (P - VRE)\n" << f[1] + "\n" << f[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Valor inicial"), TextFormat::format_currency(p));
            steps << QString("  VRE (%1)   = R$ %2\n").arg(QCoreApplication::translate("App", "Valor residual"), TextFormat::format_currency(vre));
            steps << QString("  N (%1)          = %2 %3\n\n").arg(QCoreApplication::translate("App", "Vida útil"), TextFormat::format_currency(n, 0), QCoreApplication::translate("App", "anos"));

            steps << QCoreApplication::translate("App", "Cálculo da Soma dos Dígitos:") + "\n";
            auto s_frac = TextFormat::format_fraction("N × (N + 1)", "2", "  S = ");
            steps << s_frac[0] + "\n" << s_frac[1] + "\n" << s_frac[2] + "\n";
            steps << QString("  S = %1 × %2 / 2\n").arg(TextFormat::format_currency(n, 0), TextFormat::format_currency(n + 1, 0));
            steps << QString("  S = %1\n\n").arg(TextFormat::format_currency(soma_digitos, 0));

            double depreciavel = p - vre;

            if (has_k) {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QString("%1 k = %2\n").arg(QCoreApplication::translate("App", "Análise no ano")).arg(k);
                steps << QString(60, QChar(0x2500)) + "\n\n";

                double dr_k = ((n - k + 1.0) / soma_digitos) * depreciavel;
                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Depreciação no ano")).arg(k);
                auto k_frac = TextFormat::format_fraction(QString("(%1 - %2 + 1)").arg(n).arg(k), TextFormat::format_currency(soma_digitos, 0), QString("  DR%1 = ").arg(TextFormat::to_subscript(k)));
                steps << k_frac[0] + QString(" × %1\n").arg(TextFormat::format_currency(depreciavel));
                steps << k_frac[1] + "\n" << k_frac[2] + "\n";
                steps << QString("  DR%1 = R$ %2\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(dr_k));

                double soma_parcial = 0.0;
                QStringList formula_parts;
                for (int j = 1; j <= k; ++j) {
                    soma_parcial += (n - j + 1);
                    formula_parts << QString("(%1-%2+1)").arg(n).arg(j);
                }
                double deprec_acum = (soma_parcial / soma_digitos) * depreciavel;

                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Depreciação acumulada até o ano")).arg(k);
                steps << QString("  %1: %2 = %3\n").arg(QCoreApplication::translate("App", "Soma dos fatores"), formula_parts.join(" + "), TextFormat::format_currency(soma_parcial, 0));

                auto acum_frac = TextFormat::format_fraction(TextFormat::format_currency(soma_parcial, 0), TextFormat::format_currency(soma_digitos, 0), QString("  DR%1 = ").arg(TextFormat::to_subscript("acum")));
                steps << acum_frac[0] + QString(" × %1\n").arg(TextFormat::format_currency(depreciavel));
                steps << acum_frac[1] + "\n" << acum_frac[2] + "\n";
                steps << QString("  DR%1 = R$ %2\n\n").arg(TextFormat::to_subscript("acum"), TextFormat::format_currency(deprec_acum));

                double vc_k = p - deprec_acum;
                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Valor Contábil (Valor Real) ao final do ano")).arg(k);
                steps << QString("  VR%1 = P - DR%2\n").arg(TextFormat::to_subscript(k), TextFormat::to_subscript("acum"));
                steps << QString("  VR%1 = %2 - %3\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(p), TextFormat::format_currency(deprec_acum));
                steps << QString("  VR%1 = R$ %2\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(vc_k));

                steps << QString(60, QChar(0x2550)) + "\n";
                steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
                steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "Depreciação no ano")).arg(k).arg(TextFormat::format_currency(dr_k));
                steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Depreciação acumulada"), TextFormat::format_currency(deprec_acum));
                steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "Valor Real ao final do ano")).arg(k).arg(TextFormat::format_currency(vc_k));
                steps << QString(60, QChar(0x2550)) + "\n";
            } else {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "Tabela de Depreciação Completa") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QString("%1 | %2 | %3 | %4 | %5\n")
                         .arg(QCoreApplication::translate("App", "Ano"), 4)
                         .arg(QCoreApplication::translate("App", "Fator"), 10)
                         .arg(QCoreApplication::translate("App", "DR (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "DR Acum (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "VC (R$)"), 15);
                steps << QString(80, QChar(0x2500)) + "\n";

                double deprec_acum = 0.0;
                for (int j = 1; j <= n; ++j) {
                    int fator = n - j + 1;
                    double dr_j = (fator / soma_digitos) * depreciavel;
                    deprec_acum += dr_j;
                    double vc_j = p - deprec_acum;

                    steps << QString("%1 | %2 | %3 | %4 | %5\n")
                             .arg(j, 4)
                             .arg(fator, 10)
                             .arg(TextFormat::format_currency(dr_j), 15)
                             .arg(TextFormat::format_currency(deprec_acum), 15)
                             .arg(TextFormat::format_currency(vc_j), 15);
                }
                steps << QString(80, QChar(0x2500)) + "\n";
            }

        // ============================================================
        // 2: SOMA DOS DÍGITOS - CRESCENTE
        // ============================================================
        } else if (method_index == 2) {
            double soma_digitos = (n * (n + 1.0)) / 2.0;

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "DEPRECIAÇÃO - SOMA DOS DÍGITOS (CRESCENTE)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula da depreciação no ano n:") + "\n";
            auto f = TextFormat::format_fraction("n", "S", QString("  DR%1 = ").arg(TextFormat::to_subscript("n")));
            steps << f[0] + " × (P - VRE)\n" << f[1] + "\n" << f[2] + "\n\n";

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Valor inicial"), TextFormat::format_currency(p));
            steps << QString("  VRE (%1)   = R$ %2\n").arg(QCoreApplication::translate("App", "Valor residual"), TextFormat::format_currency(vre));
            steps << QString("  N (%1)          = %2 %3\n\n").arg(QCoreApplication::translate("App", "Vida útil"), TextFormat::format_currency(n, 0), QCoreApplication::translate("App", "anos"));

            steps << QCoreApplication::translate("App", "Cálculo da Soma dos Dígitos:") + "\n";
            auto s_frac = TextFormat::format_fraction("N × (N + 1)", "2", "  S = ");
            steps << s_frac[0] + "\n" << s_frac[1] + "\n" << s_frac[2] + "\n";
            steps << QString("  S = %1 × %2 / 2\n").arg(TextFormat::format_currency(n, 0), TextFormat::format_currency(n + 1, 0));
            steps << QString("  S = %1\n\n").arg(TextFormat::format_currency(soma_digitos, 0));

            double depreciavel = p - vre;

            if (has_k) {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QString("%1 k = %2\n").arg(QCoreApplication::translate("App", "Análise no ano")).arg(k);
                steps << QString(60, QChar(0x2500)) + "\n\n";

                double dr_k = (k / soma_digitos) * depreciavel;
                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Depreciação no ano")).arg(k);
                auto k_frac = TextFormat::format_fraction(QString::number(k), TextFormat::format_currency(soma_digitos, 0), QString("  DR%1 = ").arg(TextFormat::to_subscript(k)));
                steps << k_frac[0] + QString(" × %1\n").arg(TextFormat::format_currency(depreciavel));
                steps << k_frac[1] + "\n" << k_frac[2] + "\n";
                steps << QString("  DR%1 = %2 / %3 × %4\n").arg(TextFormat::to_subscript(k)).arg(k).arg(TextFormat::format_currency(soma_digitos, 0), TextFormat::format_currency(depreciavel));
                steps << QString("  DR%1 = R$ %2\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(dr_k));

                double soma_parcial = 0.0;
                QStringList formula_parts;
                for (int j = 1; j <= k; ++j) {
                    soma_parcial += j;
                    formula_parts << QString::number(j);
                }
                double deprec_acum = (soma_parcial / soma_digitos) * depreciavel;

                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Depreciação acumulada até o ano")).arg(k);
                steps << QString("  %1: %2 = %3\n").arg(QCoreApplication::translate("App", "Soma dos fatores"), formula_parts.join(" + "), TextFormat::format_currency(soma_parcial, 0));

                auto acum_frac = TextFormat::format_fraction(TextFormat::format_currency(soma_parcial, 0), TextFormat::format_currency(soma_digitos, 0), QString("  DR%1 = ").arg(TextFormat::to_subscript("acum")));
                steps << acum_frac[0] + QString(" × %1\n").arg(TextFormat::format_currency(depreciavel));
                steps << acum_frac[1] + "\n" << acum_frac[2] + "\n";
                steps << QString("  DR%1 = R$ %2\n\n").arg(TextFormat::to_subscript("acum"), TextFormat::format_currency(deprec_acum));

                double vc_k = p - deprec_acum;
                steps << QString("%1 %2:\n").arg(QCoreApplication::translate("App", "Valor Contábil (Valor Real) ao final do ano")).arg(k);
                steps << QString("  VR%1 = P - DR%2\n").arg(TextFormat::to_subscript(k), TextFormat::to_subscript("acum"));
                steps << QString("  VR%1 = %2 - %3\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(p), TextFormat::format_currency(deprec_acum));
                steps << QString("  VR%1 = R$ %2\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(vc_k));

                steps << QString(60, QChar(0x2550)) + "\n";
                steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
                steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "Depreciação no ano")).arg(k).arg(TextFormat::format_currency(dr_k));
                steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Depreciação acumulada"), TextFormat::format_currency(deprec_acum));
                steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "Valor Real ao final do ano")).arg(k).arg(TextFormat::format_currency(vc_k));
                steps << QString(60, QChar(0x2550)) + "\n";
            } else {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "Tabela de Depreciação Completa") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QString("%1 | %2 | %3 | %4 | %5\n")
                         .arg(QCoreApplication::translate("App", "Ano"), 4)
                         .arg(QCoreApplication::translate("App", "Fator"), 10)
                         .arg(QCoreApplication::translate("App", "DR (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "DR Acum (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "VC (R$)"), 15);
                steps << QString(80, QChar(0x2500)) + "\n";

                double deprec_acum = 0.0;
                for (int j = 1; j <= n; ++j) {
                    int fator = j;
                    double dr_j = (fator / soma_digitos) * depreciavel;
                    deprec_acum += dr_j;
                    double vc_j = p - deprec_acum;

                    steps << QString("%1 | %2 | %3 | %4 | %5\n")
                             .arg(j, 4)
                             .arg(fator, 10)
                             .arg(TextFormat::format_currency(dr_j), 15)
                             .arg(TextFormat::format_currency(deprec_acum), 15)
                             .arg(TextFormat::format_currency(vc_j), 15);
                }
                steps << QString(80, QChar(0x2500)) + "\n";
            }

        // ============================================================
        // 3: SALDO DECLINANTE
        // ============================================================
        } else if (method_index == 3) {
            if (vre < 0.0 || vre >= p) {
                throw std::runtime_error(QCoreApplication::translate("App", "Para o método do Saldo Declinante, o Valor Residual (VRE) deve ser positivo e menor que P.").toStdString());
            }

            double d = 0.0;
            if (vre > 0.0) {
                d = 1.0 - std::pow(vre / p, 1.0 / static_cast<double>(n));
            } else {
                d = 2.0 / static_cast<double>(n); // Taxa do Duplo Saldo Declinante
            }

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "DEPRECIAÇÃO - MÉTODO DO SALDO DECLINANTE") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmula da taxa constante de depreciação (d):") + "\n";
            if (vre > 0.0) {
                steps << "  d = 1 - (VRE / P)⁽¹/ᴺ⁾\n\n";
            } else {
                steps << "  d = 2 / N\n\n";
            }

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Valor inicial"), TextFormat::format_currency(p));
            steps << QString("  VRE (%1)   = R$ %2\n").arg(QCoreApplication::translate("App", "Valor residual"), TextFormat::format_currency(vre));
            steps << QString("  N (%1)          = %2 %3\n\n").arg(QCoreApplication::translate("App", "Vida útil"), TextFormat::format_currency(n, 0), QCoreApplication::translate("App", "anos"));

            steps << QCoreApplication::translate("App", "Cálculo da taxa constante (d):") + "\n";
            if (vre > 0.0) {
                steps << QString("  d = 1 - (%1 / %2)⁽¹/%3⁾\n").arg(TextFormat::format_currency(vre), TextFormat::format_currency(p), QString::number(n));
            } else {
                steps << QString("  d = 2 / %1\n").arg(QString::number(n));
            }
            steps << QString("  d = %1 (%2% %3)\n\n").arg(QString::number(d, 'f', 6)).arg(TextFormat::format_currency(d * 100.0, 4)).arg(QCoreApplication::translate("App", "ao ano"));

            if (has_k) {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QString("%1 k = %2\n").arg(QCoreApplication::translate("App", "Análise no ano")).arg(k);
                steps << QString(60, QChar(0x2500)) + "\n\n";

                double vc_ant = p;
                double dr_k = 0.0;
                double deprec_acum = 0.0;
                for (int j = 1; j <= k; ++j) {
                    dr_k = d * vc_ant;
                    if (vc_ant - dr_k < vre) {
                        dr_k = std::max(0.0, vc_ant - vre);
                    }
                    deprec_acum += dr_k;
                    vc_ant = p - deprec_acum;
                }
                double vc_k = vc_ant;

                steps << QString("  %1 %2:\n").arg(QCoreApplication::translate("App", "Depreciação no ano")).arg(k);
                steps << QString("  DR%1 = d × VC%2 = %3% × %4 = R$ %5\n\n")
                         .arg(TextFormat::to_subscript(k))
                         .arg(TextFormat::to_subscript(k - 1))
                         .arg(TextFormat::format_currency(d * 100.0, 4))
                         .arg(TextFormat::format_currency(p - (deprec_acum - dr_k)))
                         .arg(TextFormat::format_currency(dr_k));

                steps << QString("  %1: DR%2 = R$ %3\n")
                         .arg(QCoreApplication::translate("App", "Depreciação acumulada"))
                         .arg(TextFormat::to_subscript("acum"))
                         .arg(TextFormat::format_currency(deprec_acum));

                steps << QString("  %1 %2: VC%3 = P - DR%4 = %5 - %6 = R$ %7\n\n")
                         .arg(QCoreApplication::translate("App", "Valor Real ao final do ano"))
                         .arg(k)
                         .arg(TextFormat::to_subscript(k))
                         .arg(TextFormat::to_subscript("acum"))
                         .arg(TextFormat::format_currency(p))
                         .arg(TextFormat::format_currency(deprec_acum))
                         .arg(TextFormat::format_currency(vc_k));

                steps << QString(60, QChar(0x2550)) + "\n";
                steps << QCoreApplication::translate("App", "RESPOSTA:") + "\n";
                steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "Depreciação no ano")).arg(k).arg(TextFormat::format_currency(dr_k));
                steps << QString("  %1: R$ %2\n").arg(QCoreApplication::translate("App", "Depreciação acumulada"), TextFormat::format_currency(deprec_acum));
                steps << QString("  %1 %2: R$ %3\n").arg(QCoreApplication::translate("App", "Valor Real ao final do ano")).arg(k).arg(TextFormat::format_currency(vc_k));
                steps << QString(60, QChar(0x2550)) + "\n";
            } else {
                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "Tabela de Depreciação Completa") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QString("%1 | %2 | %3 | %4 | %5\n")
                         .arg(QCoreApplication::translate("App", "Ano"), 4)
                         .arg(QCoreApplication::translate("App", "Taxa"), 10)
                         .arg(QCoreApplication::translate("App", "DR (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "DR Acum (R$)"), 15)
                         .arg(QCoreApplication::translate("App", "VC (R$)"), 15);
                steps << QString(80, QChar(0x2500)) + "\n";

                double vc_ant = p;
                double deprec_acum = 0.0;
                for (int j = 1; j <= n; ++j) {
                    double dr_j = d * vc_ant;
                    if (vc_ant - dr_j < vre) {
                        dr_j = std::max(0.0, vc_ant - vre);
                    }
                    deprec_acum += dr_j;
                    double vc_j = p - deprec_acum;
                    vc_ant = vc_j;

                    steps << QString("%1 | %2 | %3 | %4 | %5\n")
                             .arg(j, 4)
                             .arg(QString("%1%").arg(TextFormat::format_currency(d * 100.0, 2)), 10)
                             .arg(TextFormat::format_currency(dr_j), 15)
                             .arg(TextFormat::format_currency(deprec_acum), 15)
                             .arg(TextFormat::format_currency(vc_j), 15);
                }
                steps << QString(80, QChar(0x2500)) + "\n";
            }
        }

        if (deprec_result) {
            deprec_result->append(steps.join(""));
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao calcular depreciação: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (deprec_result) {
            QString err_msg = QString::fromUtf8(e.what());
            if (!err_msg.startsWith(QCoreApplication::translate("App", "Erro"))) {
                err_msg = QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), err_msg);
            }
            deprec_result->append(err_msg);
        }
    }
}
