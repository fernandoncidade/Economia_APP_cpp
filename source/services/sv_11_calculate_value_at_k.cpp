#include "sv_11_calculate_value_at_k.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <algorithm>
#include <QCoreApplication>
#include <QString>
#include <QStringList>

void calculate_value_at_k(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_value_at_k();
    }
}

void FinancialCalculatorApp::calculate_value_at_k() {
    try {
        double p_total = get_float_from_line_edit(amort_p);
        double i = get_float_from_line_edit(amort_i, true);
        int n = static_cast<int>(get_float_from_line_edit(amort_n));

        double entrada = get_float_from_line_edit(amort_e, false, 0.0);
        int carencia = static_cast<int>(get_float_from_line_edit(amort_carencia, false, 0.0));

        if (!amort_k || amort_k->text().trimmed().isEmpty()) {
            if (amort_result) amort_result->append(QCoreApplication::translate("App", "Erro: É necessário informar o período k desejado."));
            return;
        }

        int k = 0;
        try {
            k = static_cast<int>(get_float_from_line_edit(amort_k));
        } catch (...) {
            if (amort_result) amort_result->append(QCoreApplication::translate("App", "Erro: É necessário informar o período k desejado."));
            return;
        }

        bool capitalizar = amort_juros_capitalizados && amort_juros_capitalizados->isChecked();

        if (n <= 0) {
            if (amort_result) amort_result->append(QCoreApplication::translate("App", "Erro: Prazo (n) deve ser maior que zero."));
            return;
        }

        if (k < 1 || k > n) {
            if (amort_result) amort_result->append(QCoreApplication::translate("App", "Erro: O período k deve estar entre 1 e n."));
            return;
        }

        if (carencia < 0 || carencia >= n) {
            if (amort_result) amort_result->append(QCoreApplication::translate("App", "Erro: O período de carência deve ser menor que n e não negativo."));
            return;
        }

        double p_fin = std::max(0.0, p_total - entrada);
        if (p_fin <= 0.0) {
            if (amort_result) amort_result->append(QCoreApplication::translate("App", "Atenção: Entrada (E) igual ou maior que o principal resulta em financiamento zero ou negativo."));
            return;
        }

        int system_index = amort_system->currentIndex();

        QString sys_name;
        if (system_index == 0) {
            sys_name = QCoreApplication::translate("App", "Sistema Francês (Price)");
        } else if (system_index == 1) {
            sys_name = QCoreApplication::translate("App", "Sistema de Amortização Constante (SAC)");
        } else if (system_index == 4) {
            sys_name = QCoreApplication::translate("App", "Sistema Hamburguês (SAC com Carência)");
        } else {
            sys_name = QCoreApplication::translate("App", "Sistema não suportado para este cálculo pontual");
        }

        auto var_with_sub = [](const QString& var_name, const QString& idx) -> QString {
            return var_name + TextFormat::to_subscript(idx);
        };

        auto sup_index = [](const QString& idx) -> QString {
            QString s = idx;
            s.remove('_');
            return TextFormat::to_superscript(s);
        };

        QString P_fin_lbl = var_with_sub("P", "fin");
        QString P_base_lbl = var_with_sub("P", "base");

        QStringList lines;
        lines << QString(60, QChar(0x2550)) + "\n";
        lines << QString("%1 — %2\n").arg(QCoreApplication::translate("App", "CÁLCULO NO PERÍODO k"), sys_name);
        lines << QString(60, QChar(0x2550)) + "\n\n";

        lines << QCoreApplication::translate("App", "Dados:") + "\n";
        lines << QString("  %1 %2\n").arg(QCoreApplication::translate("App", "P (Principal) = R$"), TextFormat::format_currency(p_total));
        if (entrada > 0.0) {
            lines << QString("  %1 %2\n").arg(QCoreApplication::translate("App", "E (Entrada) = R$"), TextFormat::format_currency(entrada));
            lines << QString("  %1 (%2) = R$ %3\n").arg(P_fin_lbl, QCoreApplication::translate("App", "P - E"), TextFormat::format_currency(p_fin));
        } else {
            lines << QString("  %1\n").arg(QCoreApplication::translate("App", "E (Entrada) = R$ 0,00 (sem entrada)"));
            lines << QString("  %1 = R$ %2\n").arg(P_fin_lbl, TextFormat::format_currency(p_fin));
        }

        lines << QString("  %1 %2% %3\n").arg(QCoreApplication::translate("App", "i (taxa) ="), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
        lines << QString("  %1 %2\n").arg(QCoreApplication::translate("App", "n (períodos) =")).arg(n);

        if (carencia > 0) {
            lines << QString("  %1 %2\n").arg(QCoreApplication::translate("App", "carência (c) =")).arg(carencia);
            lines << QString("  %1 %2\n").arg(QCoreApplication::translate("App", "Capitalizar juros na carência?"), capitalizar ? "Sim" : "Não");
        } else {
            lines << QString("  %1\n").arg(QCoreApplication::translate("App", "carência (c) = 0 (sem carência)"));
        }

        lines << QString("  %1 %2\n\n").arg(QCoreApplication::translate("App", "k (período) =")).arg(k);

        // Price
        if (system_index == 0) {
            if (k <= carencia) {
                if (capitalizar) {
                    lines << QCoreApplication::translate("App", "Durante a carência com capitalização: não há amortização.") + "\n";
                } else {
                    lines << QCoreApplication::translate("App", "Durante a carência com juros pagos: não há amortização.") + "\n";
                }
                lines << QString(60, QChar(0x2500)) + "\n";
                lines << QString("%1: a%2 = R$ %3\n").arg(QCoreApplication::translate("App", "Resultado"), TextFormat::to_subscript(k), TextFormat::format_currency(0.0));
                if (amort_result) amort_result->append(lines.join(""));
                return;
            }

            double p_base = 0.0;
            if (carencia > 0) {
                if (capitalizar) {
                    p_base = p_fin * std::pow(1.0 + i, carencia);
                    lines << QCoreApplication::translate("App", "Saldo após carência (juros capitalizados):") + "\n";
                    QString car_super = TextFormat::to_superscript(carencia);
                    lines << QString("  %1 = %2 × (1 + i)%3\n").arg(P_base_lbl, P_fin_lbl, car_super);
                    lines << QString("  %1 = %2 × (1 + %3)%4\n").arg(P_base_lbl, TextFormat::format_currency(p_fin), TextFormat::format_currency(i, 6), car_super);
                    lines << QString("  %1 = R$ %2\n\n").arg(P_base_lbl, TextFormat::format_currency(p_base));
                } else {
                    p_base = p_fin;
                    lines << QCoreApplication::translate("App", "Saldo após carência (juros pagos):") + "\n";
                    lines << QString("  %1 = %2 = R$ %3\n\n").arg(P_base_lbl, P_fin_lbl, TextFormat::format_currency(p_base));
                }
            } else {
                p_base = p_fin;
                lines << QCoreApplication::translate("App", "Sem carência, iniciando amortização imediatamente:") + "\n";
                lines << QString("  %1 = %2 = R$ %3\n\n").arg(P_base_lbl, P_fin_lbl, TextFormat::format_currency(p_base));
            }

            int n_amort = n - carencia;
            int m = k - carencia;
            double ak = 0.0;

            if (std::abs(i) < 1e-15) {
                double pmt = p_base / n_amort;
                double a1 = pmt;
                ak = a1;
                lines << QCoreApplication::translate("App", "Taxa zero: prestação e amortização constantes na fase de amortização.") + "\n";
                lines << QString("  PMT = %1 / %2 = %3 / %4\n").arg(P_base_lbl, var_with_sub("n", "amort"), TextFormat::format_currency(p_base)).arg(n_amort);
                lines << QString("  PMT = R$ %1\n").arg(TextFormat::format_currency(pmt));
                lines << QString("  a₁ = PMT = R$ %1\n").arg(TextFormat::format_currency(a1));
                lines << QString("  a%1 = a₁ = R$ %2\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(ak));
            } else {
                double pow_val = std::pow(1.0 + i, n_amort);
                double num = i * pow_val;
                double den = pow_val - 1.0;
                double fator = (den != 0.0) ? (num / den) : 0.0;

                double pmt = p_base * fator;
                double a1 = pmt - p_base * i;
                ak = a1 * std::pow(1.0 + i, m - 1);

                QString n_amort_sup = sup_index("n_amort");
                QString numer_str = QString("i × (1+i)%1").arg(n_amort_sup);
                QString denom_str = QString("(1+i)%1 - 1").arg(n_amort_sup);

                lines << QCoreApplication::translate("App", "Fator (A/P) na fase de amortização:") + "\n";
                auto f = TextFormat::format_fraction(numer_str, denom_str, "  (A/P) = ");
                lines << f[0] + "\n" + f[1] + "\n" + f[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(fator, 6));

                QString n_amort_sup_num = TextFormat::to_superscript(n_amort);
                QString numer_str_valores = QString("%1 × (1+%2)%3").arg(TextFormat::format_currency(i, 6), TextFormat::format_currency(i, 6), n_amort_sup_num);
                QString denom_str_valores = QString("(1+%1)%2 - 1").arg(TextFormat::format_currency(i, 6), n_amort_sup_num);

                lines << QCoreApplication::translate("App", "Substituindo os valores:") + "\n";
                auto f_val = TextFormat::format_fraction(numer_str_valores, denom_str_valores, "  (A/P) = ");
                lines << f_val[0] + "\n" + f_val[1] + "\n" + f_val[2] + "\n\n";

                lines << QCoreApplication::translate("App", "Cálculo intermediário:") + "\n";
                lines << QString("  %1 = %2 × (1+%3)%4\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(i, 6), TextFormat::format_currency(i, 6), n_amort_sup_num);
                lines << QString("  %1 = %2 × %3%4\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(i, 6), TextFormat::format_currency(1.0 + i, 6), n_amort_sup_num);
                lines << QString("  %1 = %2 × %3\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_val, 6));
                lines << QString("  %1 = %2\n\n").arg(QCoreApplication::translate("App", "Numerador"), TextFormat::format_currency(num, 6));

                lines << QString("  %1 = (1+%2)%3 - 1\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(i, 6), n_amort_sup_num);
                lines << QString("  %1 = %2%3 - 1\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(1.0 + i, 6), n_amort_sup_num);
                lines << QString("  %1 = %2 - 1\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(pow_val, 6));
                lines << QString("  %1 = %2\n\n").arg(QCoreApplication::translate("App", "Denominador"), TextFormat::format_currency(den, 6));

                lines << QCoreApplication::translate("App", "Cálculo final:") + "\n";
                auto f_final = TextFormat::format_fraction(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), "  (A/P) = ");
                lines << f_final[0] + "\n" + f_final[1] + "\n" + f_final[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(fator, 6));

                lines << QCoreApplication::translate("App", "Prestação constante:") + "\n";
                lines << QString("  PMT = %1 × (A/P) = %2 × %3\n").arg(P_base_lbl, TextFormat::format_currency(p_base), TextFormat::format_currency(fator, 6));
                lines << QString("  PMT = R$ %1\n\n").arg(TextFormat::format_currency(pmt));
                lines << QCoreApplication::translate("App", "Amortização inicial e na k-ésima:") + "\n";
                lines << QString("  a₁ = PMT - %1 × i = %2 - %3 × %4\n").arg(P_base_lbl, TextFormat::format_currency(pmt), TextFormat::format_currency(p_base), TextFormat::format_currency(i, 6));
                lines << QString("  a₁ = R$ %1\n").arg(TextFormat::format_currency(a1));
                lines << QString("  a%1 = a₁ × (1+i)%2 = %3 × (1+%4)%5\n").arg(TextFormat::to_subscript(k), TextFormat::to_superscript(m - 1), TextFormat::format_currency(a1), TextFormat::format_currency(i, 6), TextFormat::to_superscript(m - 1));
                lines << QString("  a%1 = R$ %2\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(ak));
            }

            lines << QString(60, QChar(0x2500)) + "\n";
            lines << QString("%1: a%2 = R$ %3\n").arg(QCoreApplication::translate("App", "Resultado"), TextFormat::to_subscript(k), TextFormat::format_currency(ak));
            if (amort_result) amort_result->append(lines.join(""));
            return;
        }

        // SAC ou Hamburguês
        if (system_index == 1 || system_index == 4) {
            if (k <= carencia) {
                double pmt_k = 0.0;
                if (capitalizar) {
                    lines << QCoreApplication::translate("App", "Durante a carência com capitalização: não há pagamento (PMT_k = 0).") + "\n";
                } else {
                    pmt_k = p_fin * i;
                    lines << QCoreApplication::translate("App", "Durante a carência com juros pagos: prestação igual aos juros.") + "\n";
                    lines << QString("  PMT%1 = J = %2 × i = %3 × %4 = R$ %5\n").arg(TextFormat::to_subscript(k), P_fin_lbl, TextFormat::format_currency(p_fin), TextFormat::format_currency(i, 6), TextFormat::format_currency(pmt_k));
                }
                lines << QString(60, QChar(0x2500)) + "\n";
                lines << QString("%1: PMT%2 = R$ %3\n").arg(QCoreApplication::translate("App", "Resultado"), TextFormat::to_subscript(k), TextFormat::format_currency(pmt_k));
                if (amort_result) amort_result->append(lines.join(""));
                return;
            }

            double saldo_pos_car = 0.0;
            if (carencia > 0) {
                if (capitalizar) {
                    saldo_pos_car = p_fin * std::pow(1.0 + i, carencia);
                    lines << QCoreApplication::translate("App", "Saldo após carência (juros capitalizados):") + "\n";
                    lines << QString("  SD%1 = %2 × (1+i)%3 = R$ %4\n\n").arg(TextFormat::to_subscript(carencia), P_fin_lbl, TextFormat::to_superscript(carencia), TextFormat::format_currency(saldo_pos_car));
                } else {
                    saldo_pos_car = p_fin;
                    lines << QCoreApplication::translate("App", "Saldo após carência (juros pagos):") + "\n";
                    lines << QString("  SD%1 = %2 = R$ %3\n\n").arg(TextFormat::to_subscript(carencia), P_fin_lbl, TextFormat::format_currency(saldo_pos_car));
                }
            } else {
                saldo_pos_car = p_fin;
                lines << QCoreApplication::translate("App", "Sem carência, iniciando amortização imediatamente:") + "\n";
                lines << QString("  SD%1 = %2 = R$ %3\n\n").arg(TextFormat::to_subscript(0), P_fin_lbl, TextFormat::format_currency(saldo_pos_car));
            }

            int n_amort = n - carencia;
            double a = (n_amort > 0) ? (saldo_pos_car / n_amort) : 0.0;
            int m = k - carencia;
            double juros_m = i * (saldo_pos_car - a * (m - 1));
            double pmt_k = a + juros_m;

            lines << QCoreApplication::translate("App", "Amortização constante na fase de amortização (SAC):") + "\n";
            if (carencia > 0) {
                lines << QString("  a = SD%1 / %2 = %3 / %4\n").arg(TextFormat::to_subscript(carencia), var_with_sub("n", "amort"), TextFormat::format_currency(saldo_pos_car)).arg(n_amort);
            } else {
                lines << QString("  a = %1 / n = %2 / %3\n").arg(P_fin_lbl, TextFormat::format_currency(saldo_pos_car)).arg(n_amort);
            }
            lines << QString("  a = R$ %1\n\n").arg(TextFormat::format_currency(a));

            lines << QCoreApplication::translate("App", "Juros no período k:") + "\n";
            if (carencia > 0) {
                lines << QString("  J%1 = i × (SD%2 - a × (%3-1))\n").arg(TextFormat::to_subscript(k), TextFormat::to_subscript(carencia)).arg(m);
            } else {
                lines << QString("  J%1 = i × (%2 - a × (%3-1))\n").arg(TextFormat::to_subscript(k), P_fin_lbl).arg(m);
            }

            lines << QString("  J%1 = %2 × (%3 - %4 × %5) = R$ %6\n\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(i, 6), TextFormat::format_currency(saldo_pos_car), TextFormat::format_currency(a)).arg(m - 1).arg(TextFormat::format_currency(juros_m));
            lines << QCoreApplication::translate("App", "Prestação na k-ésima:") + "\n";
            lines << QString("  PMT%1 = a + J%2 = %3 + %4\n").arg(TextFormat::to_subscript(k), TextFormat::to_subscript(k), TextFormat::format_currency(a), TextFormat::format_currency(juros_m));
            lines << QString("  PMT%1 = R$ %2\n").arg(TextFormat::to_subscript(k), TextFormat::format_currency(pmt_k));

            lines << QString(60, QChar(0x2500)) + "\n";
            lines << QString("%1: PMT%2 = R$ %3\n").arg(QCoreApplication::translate("App", "Resultado"), TextFormat::to_subscript(k), TextFormat::format_currency(pmt_k));
            if (amort_result) amort_result->append(lines.join(""));
            return;
        }

        if (amort_result) {
            amort_result->append(QCoreApplication::translate("App", "Sistema selecionado não suporta cálculo pontual de k."));
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro em calculate_value_at_k: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (amort_result) {
            amort_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
