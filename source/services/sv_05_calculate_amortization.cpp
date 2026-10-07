#include "sv_05_calculate_amortization.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <cmath>
#include <QCoreApplication>
#include <QTableWidgetItem>
#include <QString>
#include <QStringList>

void calculate_amortization(FinancialCalculatorApp* app) {
    if (app) {
        app->calculate_amortization();
    }
}

void FinancialCalculatorApp::calculate_amortization() {
    try {
        double p = get_float_from_line_edit(amort_p);
        double i = get_float_from_line_edit(amort_i, true);
        int n = static_cast<int>(get_float_from_line_edit(amort_n));

        auto var_with_sub = [](const QString& var_name, const QString& idx) -> QString {
            return var_name + TextFormat::to_subscript(idx);
        };

        if (amort_table) {
            amort_table->setRowCount(n + 1);
            amort_table->setItem(0, 0, new QTableWidgetItem("0"));
            for (int col = 1; col < 4; ++col) {
                amort_table->setItem(0, col, new QTableWidgetItem("-"));
            }
            amort_table->setItem(0, 4, new QTableWidgetItem(TextFormat::format_currency(p, 2)));
        }

        // 0 = Price, 1 = SAC, 2 = SAM, 3 = Americano, 4 = Hamburguês
        int system_index = amort_system->currentIndex();

        QString J_k = var_with_sub("J", "k");
        QString A_k = var_with_sub("A", "k");
        QString SD_k = var_with_sub("SD", "k");
        QString SD_k_1 = var_with_sub("SD", "k-1");
        QString PMT_k = var_with_sub("PMT", "k");

        QStringList steps;

        if (system_index == 1) { // SAC
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SISTEMA SAC - AMORTIZAÇÃO CONSTANTE") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmulas:") + "\n";
            auto f = TextFormat::format_fraction("P", "n", QString("  %1: %2 = ").arg(QCoreApplication::translate("App", "Amortização (constante)"), A_k));
            steps << f[0] + "\n";
            steps << f[1] + "\n";
            steps << f[2] + "\n";
            steps << QString("  %1:                   %2 = %3 × i\n").arg(QCoreApplication::translate("App", "Juros"), J_k, SD_k_1);
            steps << QString("  %1:               %2 = %3 + %4\n").arg(QCoreApplication::translate("App", "Prestação"), PMT_k, A_k, J_k);
            steps << QString("  %1:           %2 = %3 - %4\n\n").arg(QCoreApplication::translate("App", "Saldo Devedor"), SD_k, SD_k_1, A_k);

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            double amort_const = (n != 0) ? (p / n) : 0.0;
            steps << QCoreApplication::translate("App", "Cálculo da amortização constante:") + "\n";
            auto a_frac = TextFormat::format_fraction(TextFormat::format_currency(p), TextFormat::format_currency(n, 0), "  A = ");
            steps << a_frac[0] + "\n";
            steps << a_frac[1] + "\n";
            steps << a_frac[2] + "\n";
            steps << QString("  A = R$ %1\n\n").arg(TextFormat::format_currency(amort_const));

            double juros1 = p * i;
            double prest1 = amort_const + juros1;
            double saldo1 = p - amort_const;

            QString J_1 = var_with_sub("J", "1");
            QString PMT_1 = var_with_sub("PMT", "1");
            QString SD_0 = var_with_sub("SD", "0");
            QString SD_1 = var_with_sub("SD", "1");

            steps << QCoreApplication::translate("App", "Exemplo - Período 1:") + "\n";
            steps << QString("  %1 = R$ %2\n").arg(SD_0, TextFormat::format_currency(p));
            steps << QString("  %1 = %2 × i = %3 × %4 = R$ %5\n").arg(J_1, SD_0, TextFormat::format_currency(p), TextFormat::format_currency(i, 6), TextFormat::format_currency(juros1));
            steps << QString("  %1 = A + %2 = %3 + %4 = R$ %5\n").arg(PMT_1, J_1, TextFormat::format_currency(amort_const), TextFormat::format_currency(juros1), TextFormat::format_currency(prest1));
            steps << QString("  %1 = %2 - A = %3 - %4 = R$ %5\n\n").arg(SD_1, SD_0, TextFormat::format_currency(p), TextFormat::format_currency(amort_const), TextFormat::format_currency(saldo1));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "Tabela completa gerada abaixo") + "\n";
            steps << QString(60, QChar(0x2500)) + "\n";

            if (amort_result) {
                amort_result->append(steps.join(""));
            }
            generate_sac_table(p, i, n);

        } else if (system_index == 0) { // Sistema Francês (Price)
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SISTEMA FRANCÊS (PRICE) - PRESTAÇÃO CONSTANTE") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Fórmulas:") + "\n";
            QString n_super = TextFormat::to_superscript("n");
            auto f = TextFormat::format_fraction(QString("i × (1 + i)%1").arg(n_super), QString("(1 + i)%1 - 1").arg(n_super), QString("  %1 = ").arg(QCoreApplication::translate("App", "Fator (A/P)")));
            steps << f[0] + "\n";
            steps << f[1] + "\n";
            steps << f[2] + "\n";
            steps << QString("  %1:       PMT = P × Fator(A/P)\n").arg(QCoreApplication::translate("App", "Prestação"));
            steps << QString("  %1:           %2 = %3 × i\n").arg(QCoreApplication::translate("App", "Juros"), J_k, SD_k_1);
            steps << QString("  %1:     %2 = PMT - %3\n").arg(QCoreApplication::translate("App", "Amortização"), A_k, J_k);
            steps << QString("  %1:   %2 = %3 - %4\n\n").arg(QCoreApplication::translate("App", "Saldo Devedor"), SD_k, SD_k_1, A_k);

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            QString n_super_val = TextFormat::to_superscript(static_cast<int>(n));
            double pow_val = std::pow(1.0 + i, n);
            double num = i * pow_val;
            double den = pow_val - 1.0;
            double factor = (den != 0.0) ? (num / den) : 0.0;
            double prest = p * factor;

            steps << QCoreApplication::translate("App", "Cálculo do fator (A/P):") + "\n";
            steps << QString("  (1 + i)%1 = (1 + %2)%3\n").arg(n_super, TextFormat::format_currency(i, 6), n_super_val);
            steps << QString("  (1 + i)%1 = %2\n\n").arg(n_super, TextFormat::format_currency(pow_val, 6));

            steps << "  " + QCoreApplication::translate("App", "Numerador:") + "\n";
            steps << QString("    i × (1+i)%1 = %2 × %3\n").arg(n_super, TextFormat::format_currency(i, 6), TextFormat::format_currency(pow_val, 6));
            steps << QString("                = %1\n\n").arg(TextFormat::format_currency(num, 6));

            steps << "  " + QCoreApplication::translate("App", "Denominador:") + "\n";
            steps << QString("    (1+i)%1 - 1 = %2 - 1\n").arg(n_super, TextFormat::format_currency(pow_val, 6));
            steps << QString("                = %1\n\n").arg(TextFormat::format_currency(den, 6));

            auto nf = TextFormat::format_fraction(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), QString("  %1 = ").arg(QCoreApplication::translate("App", "Fator (A/P)")));
            steps << nf[0] + "\n";
            steps << nf[1] + "\n";
            steps << nf[2] + QString(" = %1\n\n").arg(TextFormat::format_currency(factor, 6));

            steps << QCoreApplication::translate("App", "Cálculo da prestação constante:") + "\n";
            steps << "  PMT = P × Fator(A/P)\n";
            steps << QString("  PMT = %1 × %2\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(factor, 6));
            steps << QString("  PMT = R$ %1\n\n").arg(TextFormat::format_currency(prest));

            double juros1 = p * i;
            double amort1 = prest - juros1;
            double saldo1 = p - amort1;

            QString J_1 = var_with_sub("J", "1");
            QString A_1 = var_with_sub("A", "1");
            QString SD_0 = var_with_sub("SD", "0");
            QString SD_1 = var_with_sub("SD", "1");

            steps << QCoreApplication::translate("App", "Exemplo - Período 1:") + "\n";
            steps << QString("  %1 = R$ %2\n").arg(SD_0, TextFormat::format_currency(p));
            steps << QString("  %1 = %2 × i = %3 × %4 = R$ %5\n").arg(J_1, SD_0, TextFormat::format_currency(p), TextFormat::format_currency(i, 6), TextFormat::format_currency(juros1));
            steps << QString("  %1 = PMT - %2 = %3 - %4 = R$ %5\n").arg(A_1, J_1, TextFormat::format_currency(prest), TextFormat::format_currency(juros1), TextFormat::format_currency(amort1));
            steps << QString("  %1 = %2 - %3 = %4 - %5 = R$ %6\n\n").arg(SD_1, SD_0, A_1, TextFormat::format_currency(p), TextFormat::format_currency(amort1), TextFormat::format_currency(saldo1));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "Tabela completa gerada abaixo") + "\n";
            steps << QString(60, QChar(0x2500)) + "\n";

            if (amort_result) {
                amort_result->append(steps.join(""));
            }
            generate_price_table(p, i, n);

        } else if (system_index == 2) { // SAM
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SISTEMA MISTO (SAM) - MÉDIA ENTRE SAC E PRICE") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Procedimento:") + "\n";
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Para cada período k:"));
            QString PMT_SAC = var_with_sub("PMT", "SAC");
            QString J_SAC = var_with_sub("J", "SAC");
            QString A_SAC = var_with_sub("A", "SAC");
            QString SD_SAC = var_with_sub("SD", "SAC");
            QString PMT_PRICE = var_with_sub("PMT", "PRICE");
            QString J_PRICE = var_with_sub("J", "PRICE");
            QString A_PRICE = var_with_sub("A", "PRICE");
            QString SD_PRICE = var_with_sub("SD", "PRICE");

            steps << QString("  1) %1: %2, %3, %4, %5\n").arg(QCoreApplication::translate("App", "Calcular valores do SAC"), PMT_SAC, J_SAC, A_SAC, SD_SAC);
            steps << QString("  2) %1: %2, %3, %4, %5\n").arg(QCoreApplication::translate("App", "Calcular valores do PRICE"), PMT_PRICE, J_PRICE, A_PRICE, SD_PRICE);
            steps << QString("  3) %1\n\n").arg(QCoreApplication::translate("App", "Tirar a média aritmética de cada componente"));

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            // SAC
            double amort_const = (n != 0) ? (p / n) : 0.0;
            double sac_juros1 = p * i;
            double sac_prest1 = amort_const + sac_juros1;
            double sac_saldo1 = p - amort_const;

            // PRICE
            double pow_val = std::pow(1.0 + i, n);
            double num = i * pow_val;
            double den = pow_val - 1.0;
            double factor = (den != 0.0) ? (num / den) : 0.0;
            double price_prest = p * factor;
            double price_juros1 = p * i;
            double price_amort1 = price_prest - price_juros1;
            double price_saldo1 = p - price_amort1;

            // SAM
            double sam_prest1 = (sac_prest1 + price_prest) / 2.0;
            double sam_juros1 = (sac_juros1 + price_juros1) / 2.0;
            double sam_amort1 = (amort_const + price_amort1) / 2.0;
            double sam_saldo1 = (sac_saldo1 + price_saldo1) / 2.0;

            QString J_1 = var_with_sub("J", "1");
            QString PMT_1 = var_with_sub("PMT", "1");
            QString A_1 = var_with_sub("A", "1");
            QString SD_1 = var_with_sub("SD", "1");

            steps << QCoreApplication::translate("App", "Exemplo - Período 1:") + "\n\n";

            steps << "  SAC:\n";
            auto a_frac = TextFormat::format_fraction(TextFormat::format_currency(p), TextFormat::format_currency(n, 0), "    A = ");
            steps << a_frac[0] + "\n";
            steps << a_frac[1] + "\n";
            steps << a_frac[2] + QString(" = R$ %1\n").arg(TextFormat::format_currency(amort_const));
            steps << QString("    %1 = %2 × %3 = R$ %4\n").arg(J_1, TextFormat::format_currency(p), TextFormat::format_currency(i, 6), TextFormat::format_currency(sac_juros1));
            steps << QString("    %1 = %2 + %3 = R$ %4\n").arg(PMT_1, TextFormat::format_currency(amort_const), TextFormat::format_currency(sac_juros1), TextFormat::format_currency(sac_prest1));
            steps << QString("    %1 = R$ %2\n\n").arg(SD_1, TextFormat::format_currency(sac_saldo1));

            steps << "  PRICE:\n";
            auto f_frac = TextFormat::format_fraction(TextFormat::format_currency(num, 6), TextFormat::format_currency(den, 6), QString("    %1 = ").arg(QCoreApplication::translate("App", "Fator")));
            steps << f_frac[0] + "\n";
            steps << f_frac[1] + "\n";
            steps << f_frac[2] + QString(" = %1\n").arg(TextFormat::format_currency(factor, 6));
            steps << QString("    PMT = %1 × %2 = R$ %3\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(factor, 6), TextFormat::format_currency(price_prest));
            steps << QString("    %1 = %2 × %3 = R$ %4\n").arg(J_1, TextFormat::format_currency(p), TextFormat::format_currency(i, 6), TextFormat::format_currency(price_juros1));
            steps << QString("    %1 = %2 - %3 = R$ %4\n").arg(A_1, TextFormat::format_currency(price_prest), TextFormat::format_currency(price_juros1), TextFormat::format_currency(price_amort1));
            steps << QString("    %1 = R$ %2\n\n").arg(SD_1, TextFormat::format_currency(price_saldo1));

            steps << QString("  SAM (%1):\n").arg(QCoreApplication::translate("App", "Médias"));
            steps << QString("    %1 = (%2 + %3) / 2 = R$ %4\n").arg(PMT_1, TextFormat::format_currency(sac_prest1), TextFormat::format_currency(price_prest), TextFormat::format_currency(sam_prest1));
            steps << QString("    %1   = (%2 + %3) / 2 = R$ %4\n").arg(J_1, TextFormat::format_currency(sac_juros1), TextFormat::format_currency(price_juros1), TextFormat::format_currency(sam_juros1));
            steps << QString("    %1   = (%2 + %3) / 2 = R$ %4\n").arg(A_1, TextFormat::format_currency(amort_const), TextFormat::format_currency(price_amort1), TextFormat::format_currency(sam_amort1));
            steps << QString("    %1  = (%2 + %3) / 2 = R$ %4\n\n").arg(SD_1, TextFormat::format_currency(sac_saldo1), TextFormat::format_currency(price_saldo1), TextFormat::format_currency(sam_saldo1));

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "Tabela completa gerada abaixo") + "\n";
            steps << QString(60, QChar(0x2500)) + "\n";

            if (amort_result) {
                amort_result->append(steps.join(""));
            }
            generate_sam_table(p, i, n);

        } else if (system_index == 3) { // Sistema Americano
            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SISTEMA AMERICANO") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Características:") + "\n";
            steps << QString("  • %1\n").arg(QCoreApplication::translate("App", "Períodos intermediários (k < n): Pagamento apenas de juros"));
            steps << QString("  • %1\n").arg(QCoreApplication::translate("App", "Amortização: Zero para k < n"));
            steps << QString("  • %1\n").arg(QCoreApplication::translate("App", "Saldo Devedor: Permanece igual a P até o último período"));
            steps << QString("  • %1\n\n").arg(QCoreApplication::translate("App", "Período final (k = n): Pagamento de juros + amortização total"));

            QString J_n = var_with_sub("J", "n");
            QString A_n = var_with_sub("A", "n");
            QString PMT_n = var_with_sub("PMT", "n");
            QString SD_n = var_with_sub("SD", "n");

            steps << QCoreApplication::translate("App", "Fórmulas:") + "\n";
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Para k < n:"));
            steps << QString("    %1 = P × i\n").arg(J_k);
            steps << QString("    %1 = 0\n").arg(A_k);
            steps << QString("    %1 = %2\n").arg(PMT_k, J_k);
            steps << QString("    %1 = P\n\n").arg(SD_k);
            steps << QString("  %1\n").arg(QCoreApplication::translate("App", "Para k = n:"));
            steps << QString("    %1 = P × i\n").arg(J_n);
            steps << QString("    %1 = P\n").arg(A_n);
            steps << QString("    %1 = %2 + %3 = P × (1 + i)\n").arg(PMT_n, J_n, A_n);
            steps << QString("    %1 = 0\n\n").arg(SD_n);

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)      = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)           = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2\n\n").arg(QCoreApplication::translate("App", "Períodos"), TextFormat::format_currency(n, 0));

            double juros_periodo = p * i;
            double prest_intermediaria = juros_periodo;
            double prest_final = p * (1.0 + i);

            steps << QCoreApplication::translate("App", "Cálculos:") + "\n";
            steps << QString("  %1: J = P × i = %2 × %3\n").arg(QCoreApplication::translate("App", "Juros por período"), TextFormat::format_currency(p), TextFormat::format_currency(i, 6));
            steps << QString("  %1: J = R$ %2\n\n").arg(QCoreApplication::translate("App", "Juros por período"), TextFormat::format_currency(juros_periodo));

            steps << QString("  %1 (k < n): PMT = R$ %2\n").arg(QCoreApplication::translate("App", "Prestação intermediária"), TextFormat::format_currency(prest_intermediaria));
            steps << QString("  %1 (k = n): PMT = P + J = %2 + %3\n").arg(QCoreApplication::translate("App", "Prestação final"), TextFormat::format_currency(p), TextFormat::format_currency(juros_periodo));
            steps << QString("  %1: PMT = R$ %2\n\n").arg(QCoreApplication::translate("App", "Prestação final"), TextFormat::format_currency(prest_final));

            if (n >= 6) {
                QString SD_6 = var_with_sub("SD", "6");
                steps << QCoreApplication::translate("App", "Exemplo - Saldo Devedor após Período 6:") + "\n";
                steps << QString("  %1%2, %3\n").arg(QCoreApplication::translate("App", "Como k=6 < n=")).arg(n).arg(QCoreApplication::translate("App", "o saldo devedor permanece inalterado"));
                steps << QString("  %1 = P = R$ %2\n\n").arg(SD_6, TextFormat::format_currency(p));
            }

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "Tabela completa gerada abaixo") + "\n";
            steps << QString(60, QChar(0x2500)) + "\n";

            if (amort_result) {
                amort_result->append(steps.join(""));
            }
            generate_american_table(p, i, n);

        } else if (system_index == 4) { // Sistema Hamburguês
            QString carencia_text = amort_carencia ? amort_carencia->text().trimmed() : QString();
            int carencia = carencia_text.isEmpty() ? 0 : carencia_text.toInt();
            bool capitalizar = amort_juros_capitalizados && amort_juros_capitalizados->isChecked();

            if (carencia >= n) {
                QString error_msg = QCoreApplication::translate("App", "Erro: O período de carência deve ser menor que o prazo total.");
                if (amort_result) amort_result->append(error_msg);
                if (amort_table) {
                    amort_table->setRowCount(1);
                    amort_table->setSpan(0, 0, 1, 5);
                    amort_table->setItem(0, 0, new QTableWidgetItem(error_msg));
                }
                return;
            }

            steps << QString(60, QChar(0x2550)) + "\n";
            steps << QCoreApplication::translate("App", "SISTEMA HAMBURGUÊS (SAC COM CARÊNCIA)") + "\n";
            steps << QString(60, QChar(0x2550)) + "\n\n";

            steps << QCoreApplication::translate("App", "Características:") + "\n";
            steps << QString("  • %1\n").arg(QCoreApplication::translate("App", "Período de Carência: Sem amortização do principal"));
            if (capitalizar) {
                steps << QString("  • %1\n").arg(QCoreApplication::translate("App", "Juros na Carência: Capitalizados (incorporados ao saldo)"));
            } else {
                steps << QString("  • %1\n").arg(QCoreApplication::translate("App", "Juros na Carência: Pagos mensalmente"));
            }
            steps << QString("  • %1\n\n").arg(QCoreApplication::translate("App", "Período de Amortização: SAC sobre o saldo devedor"));

            steps << QCoreApplication::translate("App", "Dados do problema:") + "\n";
            steps << QString("  P (%1)         = R$ %2\n").arg(QCoreApplication::translate("App", "Principal"), TextFormat::format_currency(p));
            steps << QString("  i (%1)              = %2% %3\n").arg(QCoreApplication::translate("App", "Taxa"), TextFormat::format_currency(i * 100.0, 2), QCoreApplication::translate("App", "ao período"));
            steps << QString("  n (%1)       = %2 %3\n").arg(QCoreApplication::translate("App", "Prazo total"), TextFormat::format_currency(n, 0), QCoreApplication::translate("App", "períodos"));
            steps << QString("  %1              = %2 %3\n").arg(QCoreApplication::translate("App", "Carência"), TextFormat::format_currency(carencia, 0), QCoreApplication::translate("App", "períodos"));
            steps << QString("  %1           = %2 %3\n\n").arg(QCoreApplication::translate("App", "Amortização"), TextFormat::format_currency(n - carencia, 0), QCoreApplication::translate("App", "períodos"));

            QString SD_carencia = var_with_sub("SD", QString::number(carencia));
            double saldo_pos_carencia = 0.0;

            if (capitalizar) {
                double pow_carencia = std::pow(1.0 + i, carencia);
                saldo_pos_carencia = p * pow_carencia;

                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "FASE 1: PERÍODO DE CARÊNCIA (JUROS CAPITALIZADOS)") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                QString car_super = TextFormat::to_superscript(carencia);
                steps << QCoreApplication::translate("App", "Saldo ao final da carência:") + "\n";
                steps << QString("  %1 = P × (1 + i)%2\n").arg(SD_carencia, car_super);
                steps << QString("  %1 = %2 × (1 + %3)%4\n").arg(SD_carencia, TextFormat::format_currency(p), TextFormat::format_currency(i, 6), car_super);
                steps << QString("  %1 = %2 × %3\n").arg(SD_carencia, TextFormat::format_currency(p), TextFormat::format_currency(pow_carencia, 6));
                steps << QString("  %1 = R$ %2\n\n").arg(SD_carencia, TextFormat::format_currency(saldo_pos_carencia));
            } else {
                saldo_pos_carencia = p;
                double juros_carencia = p * i;

                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "FASE 1: PERÍODO DE CARÊNCIA (JUROS PAGOS)") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QCoreApplication::translate("App", "Juros pagos mensalmente:") + "\n";
                steps << QString("  J = P × i = %1 × %2\n").arg(TextFormat::format_currency(p), TextFormat::format_currency(i, 6));
                steps << QString("  J = R$ %1\n\n").arg(TextFormat::format_currency(juros_carencia));
                steps << QString("  %1: SD = R$ %2\n\n").arg(QCoreApplication::translate("App", "Saldo devedor permanece constante"), TextFormat::format_currency(saldo_pos_carencia));
            }

            int n_amort = n - carencia;
            double amort_const = (n_amort > 0) ? (saldo_pos_carencia / n_amort) : 0.0;

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "FASE 2: PERÍODO DE AMORTIZAÇÃO (SAC)") + "\n";
            steps << QString(60, QChar(0x2500)) + "\n\n";

            steps << QCoreApplication::translate("App", "Amortização constante:") + "\n";
            auto a_frac = TextFormat::format_fraction(TextFormat::format_currency(saldo_pos_carencia), TextFormat::format_currency(n_amort, 0), "  A = ");
            steps << a_frac[0] + "\n";
            steps << a_frac[1] + "\n";
            steps << a_frac[2] + "\n";
            steps << QString("  A = R$ %1\n\n").arg(TextFormat::format_currency(amort_const));

            int periodo_6 = carencia + 1;
            double prest_6 = 0.0;
            if (n >= periodo_6) {
                double juros_6 = saldo_pos_carencia * i;
                prest_6 = amort_const + juros_6;

                QString J_6 = var_with_sub("J", QString::number(periodo_6));
                QString PMT_6 = var_with_sub("PMT", QString::number(periodo_6));

                steps << QString("%1 %2 (%3):\n").arg(QCoreApplication::translate("App", "Exemplo - Período")).arg(periodo_6).arg(QCoreApplication::translate("App", "primeiro da amortização"));
                steps << QString("  %1 = R$ %2\n").arg(SD_carencia, TextFormat::format_currency(saldo_pos_carencia));
                steps << QString("  %1 = %2 × i = %3 × %4\n").arg(J_6, SD_carencia, TextFormat::format_currency(saldo_pos_carencia), TextFormat::format_currency(i, 6));
                steps << QString("  %1 = R$ %2\n").arg(J_6, TextFormat::format_currency(juros_6));
                steps << QString("  %1 = A + %2 = %3 + %4\n").arg(PMT_6, J_6, TextFormat::format_currency(amort_const), TextFormat::format_currency(juros_6));
                steps << QString("  %1 = R$ %2\n\n").arg(PMT_6, TextFormat::format_currency(prest_6));
            }

            if (carencia == 5 && n == 10) {
                double saldo_alt = p;
                double amort_alt = (n_amort > 0) ? (saldo_alt / n_amort) : 0.0;
                double juros_alt = saldo_alt * i;
                double prest_alt = amort_alt + juros_alt;
                double diferenca = prest_6 - prest_alt;

                steps << QString(60, QChar(0x2500)) + "\n";
                steps << QCoreApplication::translate("App", "COMPARAÇÃO DE CENÁRIOS") + "\n";
                steps << QString(60, QChar(0x2500)) + "\n\n";

                steps << QString("%1: R$ %2\n").arg(QCoreApplication::translate("App", "Prestação no período 6 (juros capitalizados)"), TextFormat::format_currency(prest_6));
                steps << QString("%1: R$ %2\n").arg(QCoreApplication::translate("App", "Prestação no período 6 (juros pagos)"), TextFormat::format_currency(prest_alt));
                steps << QString("%1: R$ %2\n\n").arg(QCoreApplication::translate("App", "Diferença"), TextFormat::format_currency(diferenca));
            }

            steps << QString(60, QChar(0x2500)) + "\n";
            steps << QCoreApplication::translate("App", "Tabela completa gerada abaixo") + "\n";
            steps << QString(60, QChar(0x2500)) + "\n";

            if (amort_result) {
                amort_result->append(steps.join(""));
            }
            generate_hamburgues_table(p, i, n, carencia, capitalizar);
        }

    } catch (const std::exception& e) {
        LogManager::instance().log(QString("Erro ao gerar tabela de amortização: %1").arg(e.what()), LogManager::LogLevel::ERR);
        if (amort_table) {
            amort_table->setRowCount(1);
            amort_table->setSpan(0, 0, 1, 5);
            amort_table->setItem(0, 0, new QTableWidgetItem(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro ao gerar tabela"), e.what())));
        }
        if (amort_result) {
            amort_result->append(QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), e.what()));
        }
    }
}
