#include "ui_20_generate_caue_input_table.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"

#include <QTableWidgetItem>
#include <QCoreApplication>

void FinancialCalculatorApp::generate_caue_input_table() {
    auto tr_app = [](const char* text) {
        return QCoreApplication::translate("App", text);
    };

    try {
        if (!caue_max_years || caue_max_years->text().trimmed().isEmpty()) {
            if (caue_result) {
                auto retrans = []() -> QString {
                    return QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), QCoreApplication::translate("App", "Por favor, informe o número máximo de anos."));
                };
                caue_result->append(retrans(), nullptr, retrans);
            }
            return;
        }

        int max_years = static_cast<int>(get_float_from_line_edit(caue_max_years));

        if (max_years <= 0) {
            if (caue_result) {
                auto retrans = []() -> QString {
                    return QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), QCoreApplication::translate("App", "O número de anos deve ser maior que zero."));
                };
                caue_result->append(retrans(), nullptr, retrans);
            }
            return;
        }

        caue_input_table->setRowCount(max_years);

        QList<double> default_vr = {40000.0, 32000.0, 25000.0, 22000.0, 20000.0};
        QList<double> default_com = {10000.0, 10500.0, 11000.0, 11500.0, 12000.0};

        for (int n = 1; n <= max_years; ++n) {
            auto* item_year = new QTableWidgetItem(QString::number(n));
            item_year->setFlags(item_year->flags() & ~Qt::ItemIsEditable);
            caue_input_table->setItem(n - 1, 0, item_year);

            QString vr_text;
            QString com_text;
            if (max_years == 5 && n <= default_vr.size()) {
                vr_text = TextFormat::format_currency(default_vr[n - 1], 2);
                com_text = TextFormat::format_currency(default_com[n - 1], 2);
            }

            caue_input_table->setItem(n - 1, 1, new QTableWidgetItem(vr_text));
            caue_input_table->setItem(n - 1, 2, new QTableWidgetItem(com_text));
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar tabela de entrada CAUE: %1").arg(e.what()), true);
        if (caue_result) {
            QString err = e.what();
            auto retrans = [err]() -> QString {
                return QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), err);
            };
            caue_result->append(retrans(), nullptr, retrans);
        }
    }
}
