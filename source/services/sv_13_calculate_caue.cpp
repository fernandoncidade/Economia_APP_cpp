#include "source/services/sv_13_calculate_caue.hpp"
#include "source/fca_01_FinancialCalculatorAPP.hpp"
#include "source/utils/LogManager.hpp"
#include "source/utils/TextFormat.hpp"
#include "source/ui/ui_23_history_container.hpp"

#include <QCoreApplication>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QBrush>
#include <cmath>
#include <vector>
#include <algorithm>

struct CaueItem {
    int ano;
    double vr;
    double com;
    double caue;
};

static QString format_caue_steps(double p, double tma, int max_years,
                                const std::vector<CaueItem>& dados,
                                const std::vector<CaueItem>& resultados,
                                int menor_ano) {
    auto tr = [](const char* sourceText) {
        return QCoreApplication::translate("App", sourceText);
    };

    QString s_caue = tr("CAUE");
    QString s_vp = tr("VP");
    QString s_vp_custos = tr("VP(Custos)");
    QString s_vp_revenda = tr("VP(Revenda)");

    QStringList steps;
    steps.append(QString("═").repeated(70) + "\n");
    steps.append(tr("CÁLCULO DE CAUE E VIDA ECONÔMICA DO ATIVO") + "\n");
    steps.append(QString("═").repeated(70) + "\n\n");

    steps.append(tr("DADOS DO EXERCÍCIO:") + "\n");
    steps.append(QString("─").repeated(70) + "\n");
    steps.append(QString("  • %1: R$ %2\n").arg(tr("Custo de Aquisição (P)"), TextFormat::format_currency(p)));
    steps.append(QString("  • %1: %2% %3\n").arg(tr("Taxa Mínima de Atratividade (TMA)"), TextFormat::format_currency(tma * 100.0, 2), tr("ao ano")));
    steps.append(QString("  • %1: %2 %3\n\n").arg(tr("Período máximo de análise"), QString::number(max_years), tr("anos")));

    steps.append(tr("Valores de Revenda (VRₙ) e Custos de Operação (Comₙ):") + "\n\n");
    steps.append(QString("  %1 | %2 | %3\n")
        .arg(tr("Ano (n)"), 8)
        .arg(tr("VRₙ (R$)"), 15)
        .arg(tr("Comₙ (R$)"), 15));
    steps.append("  " + QString("─").repeated(45) + "\n");
    for (const auto& d : dados) {
        steps.append(QString("  %1 | %2 | %3\n")
            .arg(QString::number(d.ano), 8)
            .arg(TextFormat::format_currency(d.vr), 15)
            .arg(TextFormat::format_currency(d.com), 15));
    }

    steps.append("\n");
    steps.append(QString("═").repeated(70) + "\n");
    steps.append(tr("METODOLOGIA DE CÁLCULO") + "\n");
    steps.append(QString("═").repeated(70) + "\n\n");

    steps.append(tr("O objetivo é determinar a vida econômica do ativo, que é o período que resulta no menor Custo Anual Uniforme Equivalente (CAUE).") + "\n\n");

    steps.append(tr("Fórmula do CAUE:") + "\n");
    steps.append(QString("  %1%2 = %3%4 × (A/P; i; n)\n\n")
        .arg(s_caue, TextFormat::to_subscript("n"), s_vp, TextFormat::to_subscript("n")));

    steps.append(tr("Onde o VPₙ (Valor Presente total dos custos) é:") + "\n");
    steps.append(QString("  %1%2 = P + %3%4 - %5%6\n\n")
        .arg(s_vp, TextFormat::to_subscript("n"),
             s_vp_custos, TextFormat::to_subscript("n"),
             s_vp_revenda, TextFormat::to_subscript("n")));

    for (const auto& d : dados) {
        int n = d.ano;

        steps.append(QString("═").repeated(70) + "\n");
        steps.append(QString("%1 %2 (n=%3)\n").arg(tr("ANO"), QString::number(n), QString::number(n)));
        steps.append(QString("═").repeated(70) + "\n\n");

        // 1. VP dos Custos
        double vp_com_total = 0.0;
        steps.append(QString("1. %1\n").arg(tr("VP DOS CUSTOS DE OPERAÇÃO (ACUMULADO)")));
        steps.append(QString("─").repeated(70) + "\n\n");

        for (int j = 1; j <= n; ++j) {
            double pf_factor = 1.0 / std::pow(1.0 + tma, j);
            double vp_com_j = dados[j - 1].com * pf_factor;
            vp_com_total += vp_com_j;

            steps.append(QString("  %1 %2:\n").arg(tr("Ano"), QString::number(j)));
            steps.append(QString("    (P/F; %1%; %2) = 1 / (1 + %3)%4\n")
                .arg(TextFormat::format_currency(tma * 100.0, 2))
                .arg(QString::number(j))
                .arg(TextFormat::format_currency(tma, 6))
                .arg(TextFormat::to_superscript(j)));
            steps.append(QString("    (P/F; %1%; %2) = %3\n")
                .arg(TextFormat::format_currency(tma * 100.0, 2))
                .arg(QString::number(j))
                .arg(TextFormat::format_currency(pf_factor, 6)));
            steps.append(QString("    %1(Com%2) = %3 × %4\n")
                .arg(s_vp)
                .arg(TextFormat::to_subscript(j))
                .arg(TextFormat::format_currency(dados[j - 1].com))
                .arg(TextFormat::format_currency(pf_factor, 6)));
            steps.append(QString("    %1(Com%2) = R$ %3\n\n")
                .arg(s_vp)
                .arg(TextFormat::to_subscript(j))
                .arg(TextFormat::format_currency(vp_com_j)));
        }

        steps.append(QString("  %1%2 = R$ %3\n\n")
            .arg(s_vp_custos, TextFormat::to_subscript(n), TextFormat::format_currency(vp_com_total)));

        // 2. VP do Valor de Revenda
        steps.append(QString("2. %1\n").arg(tr("VP DO VALOR DE REVENDA")));
        steps.append(QString("─").repeated(70) + "\n\n");

        double pf_revenda = 1.0 / std::pow(1.0 + tma, n);
        double vp_revenda = d.vr * pf_revenda;

        steps.append(QString("  (P/F; %1%; %2) = 1 / (1 + %3)%4\n")
            .arg(TextFormat::format_currency(tma * 100.0, 2))
            .arg(QString::number(n))
            .arg(TextFormat::format_currency(tma, 6))
            .arg(TextFormat::to_superscript(n)));
        steps.append(QString("  (P/F; %1%; %2) = %3\n")
            .arg(TextFormat::format_currency(tma * 100.0, 2))
            .arg(QString::number(n))
            .arg(TextFormat::format_currency(pf_revenda, 6)));
        steps.append(QString("  %1(VR%2) = %3 × %4\n")
            .arg(s_vp)
            .arg(TextFormat::to_subscript(n))
            .arg(TextFormat::format_currency(d.vr))
            .arg(TextFormat::format_currency(pf_revenda, 6)));
        steps.append(QString("  %1(VR%2) = R$ %3\n\n")
            .arg(s_vp)
            .arg(TextFormat::to_subscript(n))
            .arg(TextFormat::format_currency(vp_revenda)));

        // 3. VP Total
        steps.append(QString("3. %1\n").arg(tr("VP TOTAL")));
        steps.append(QString("─").repeated(70) + "\n\n");

        double vp_total = p + vp_com_total - vp_revenda;

        steps.append(QString("  %1%2 = P + %3%4 - %5%6\n")
            .arg(s_vp, TextFormat::to_subscript(n),
                 s_vp_custos, TextFormat::to_subscript(n),
                 s_vp_revenda, TextFormat::to_subscript(n)));
        steps.append(QString("  %1%2 = %3 + %4 - %5\n")
            .arg(s_vp, TextFormat::to_subscript(n),
                 TextFormat::format_currency(p),
                 TextFormat::format_currency(vp_com_total),
                 TextFormat::format_currency(vp_revenda)));
        steps.append(QString("  %1%2 = R$ %3\n\n")
            .arg(s_vp, TextFormat::to_subscript(n),
                 TextFormat::format_currency(vp_total)));

        // 4. CAUE
        steps.append(QString("4. %1\n").arg(tr("CÁLCULO DO CAUE")));
        steps.append(QString("─").repeated(70) + "\n\n");

        double pow_val = std::pow(1.0 + tma, n);
        double ap_num = tma * pow_val;
        double ap_den = pow_val - 1.0;
        double ap_factor = (ap_den != 0.0) ? (ap_num / ap_den) : 0.0;
        double caue = vp_total * ap_factor;

        steps.append(tr("Fator (A/P):") + "\n");
        auto [f1, f2, f3] = TextFormat::format_fraction(
            QString("i × (1+i)%1").arg(TextFormat::to_superscript(n)),
            QString("(1+i)%1 - 1").arg(TextFormat::to_superscript(n)),
            QString("  (A/P; %1%; %2) = ").arg(TextFormat::format_currency(tma * 100.0, 2)).arg(n)
        );
        steps.append(f1 + "\n");
        steps.append(f2 + "\n");
        steps.append(f3 + "\n\n");

        steps.append(QString("  (1 + i)%1 = (1 + %2)%3\n")
            .arg(TextFormat::to_superscript(n))
            .arg(TextFormat::format_currency(tma, 6))
            .arg(TextFormat::to_superscript(n)));
        steps.append(QString("  (1 + i)%1 = %2\n\n")
            .arg(TextFormat::to_superscript(n))
            .arg(TextFormat::format_currency(pow_val, 6)));

        steps.append(QString("  %1: %2 × %3 = %4\n")
            .arg(tr("Numerador"))
            .arg(TextFormat::format_currency(tma, 6))
            .arg(TextFormat::format_currency(pow_val, 6))
            .arg(TextFormat::format_currency(ap_num, 6)));
        steps.append(QString("  %1: %2 - 1 = %3\n\n")
            .arg(tr("Denominador"))
            .arg(TextFormat::format_currency(pow_val, 6))
            .arg(TextFormat::format_currency(ap_den, 6)));

        steps.append(QString("  (A/P; %1%; %2) = %3 / %4\n")
            .arg(TextFormat::format_currency(tma * 100.0, 2))
            .arg(QString::number(n))
            .arg(TextFormat::format_currency(ap_num, 6))
            .arg(TextFormat::format_currency(ap_den, 6)));
        steps.append(QString("  (A/P; %1%; %2) = %3\n\n")
            .arg(TextFormat::format_currency(tma * 100.0, 2))
            .arg(QString::number(n))
            .arg(TextFormat::format_currency(ap_factor, 6)));

        steps.append(QString("  %1%2 = %3%4 × (A/P; %5%; %6)\n")
            .arg(s_caue, TextFormat::to_subscript(n),
                 s_vp, TextFormat::to_subscript(n),
                 TextFormat::format_currency(tma * 100.0, 2),
                 QString::number(n)));
        steps.append(QString("  %1%2 = %3 × %4\n")
            .arg(s_caue, TextFormat::to_subscript(n),
                 TextFormat::format_currency(vp_total),
                 TextFormat::format_currency(ap_factor, 6)));
        steps.append(QString("  %1%2 = R$ %3\n\n")
            .arg(s_caue, TextFormat::to_subscript(n),
                 TextFormat::format_currency(caue, 2)));
    }

    // Tabela resumo (texto)
    steps.append(QString("═").repeated(70) + "\n");
    steps.append(tr("TABELA DE RESULTADOS") + "\n");
    steps.append(QString("═").repeated(70) + "\n\n");

    steps.append(QString("  %1 | %2 | %3 | %4\n")
        .arg(tr("Ano (n)"), 8)
        .arg(tr("VRₙ (R$)"), 15)
        .arg(tr("Comₙ (R$)"), 15)
        .arg(tr("CAUEₙ (R$)"), 15));
    steps.append("  " + QString("─").repeated(60) + "\n");

    for (const auto& r : resultados) {
        QString marker = (r.ano == menor_ano) ? QString(" ← ") + tr("MENOR CAUE") : QString("");
        steps.append(QString("  %1 | %2 | %3 | %4%5\n")
            .arg(QString::number(r.ano), 8)
            .arg(TextFormat::format_currency(r.vr), 15)
            .arg(TextFormat::format_currency(r.com), 15)
            .arg(TextFormat::format_currency(r.caue, 2), 15)
            .arg(marker));
    }
    steps.append("\n");

    return steps.join("");
}

void calculate_caue(FinancialCalculatorApp* self) {
    if (!self) return;
    Logger logger = LogManager::get_logger();

    try {
        auto tr = [](const char* sourceText) {
            return QCoreApplication::translate("App", sourceText);
        };

        // Ler dados principais
        double p = self->get_float_from_line_edit(self->caue_initial_cost);
        double tma = self->get_float_from_line_edit(self->caue_tma, true);
        int max_years = static_cast<int>(self->get_float_from_line_edit(self->caue_max_years));

        if (max_years <= 0) {
            auto retrans = []() -> QString {
                return QCoreApplication::translate("App", "Erro: Período máximo de análise deve ser maior que zero.");
            };
            self->caue_result->append(retrans(), nullptr, retrans);
            return;
        }

        // Ler dados da tabela
        std::vector<CaueItem> dados;
        for (int n = 0; n < max_years; ++n) {
            QTableWidgetItem* vr_item = self->caue_input_table ? self->caue_input_table->item(n, 1) : nullptr;
            QTableWidgetItem* com_item = self->caue_input_table ? self->caue_input_table->item(n, 2) : nullptr;

            if (!vr_item || !com_item || vr_item->text().trimmed().isEmpty() || com_item->text().trimmed().isEmpty()) {
                int row = n + 1;
                auto retrans = [row]() -> QString {
                    return QString(QCoreApplication::translate("App", "Erro: Dados incompletos na linha %1")).arg(row);
                };
                self->caue_result->append(retrans(), nullptr, retrans);
                return;
            }

            bool ok1 = false, ok2 = false;
            QString s1 = vr_item->text().trimmed();
            s1.remove('.').replace(',', '.');
            double vr_n = s1.toDouble(&ok1);

            QString s2 = com_item->text().trimmed();
            s2.remove('.').replace(',', '.');
            double com_n = s2.toDouble(&ok2);

            if (!ok1 || !ok2) {
                int row = n + 1;
                auto retrans = [row]() -> QString {
                    return QString(QCoreApplication::translate("App", "Erro: Valores inválidos na linha %1")).arg(row);
                };
                self->caue_result->append(retrans(), nullptr, retrans);
                return;
            }

            dados.push_back({n + 1, vr_n, com_n, 0.0});
        }

        // Cálculos para cada ano
        std::vector<CaueItem> resultados;
        for (const auto& d : dados) {
            int n = d.ano;
            double vp_com_total = 0.0;
            for (int j = 1; j <= n; ++j) {
                double pf_factor = 1.0 / std::pow(1.0 + tma, j);
                vp_com_total += dados[j - 1].com * pf_factor;
            }
            double pf_revenda = 1.0 / std::pow(1.0 + tma, n);
            double vp_revenda = d.vr * pf_revenda;
            double vp_total = p + vp_com_total - vp_revenda;

            double pow_val = std::pow(1.0 + tma, n);
            double ap_num = tma * pow_val;
            double ap_den = pow_val - 1.0;
            double ap_factor = (ap_den != 0.0) ? (ap_num / ap_den) : 0.0;
            double caue = vp_total * ap_factor;

            resultados.push_back({n, d.vr, d.com, caue});
        }

        auto min_it = std::min_element(resultados.begin(), resultados.end(), [](const CaueItem& a, const CaueItem& b) {
            return a.caue < b.caue;
        });
        int menor_ano = (min_it != resultados.end()) ? min_it->ano : -1;

        QString steps_text = format_caue_steps(p, tma, max_years, dados, resultados, menor_ano);

        // Criar tabela QTableWidget que será inserida dentro da entrada do HistoryContainer
        QTableWidget* table_widget = nullptr;
        try {
            table_widget = new QTableWidget();
            table_widget->setColumnCount(4);
            QStringList header_keys = {
                "Ano (n)",
                "VRₙ (R$)",
                "Comₙ (R$)",
                "CAUEₙ (R$)"
            };
            for (int c = 0; c < 4; ++c) {
                auto* hi = new QTableWidgetItem(tr(header_keys[c].toUtf8().constData()));
                hi->setData(Qt::UserRole, header_keys[c]);
                table_widget->setHorizontalHeaderItem(c, hi);
            }
            table_widget->setRowCount(static_cast<int>(resultados.size()));
            if (table_widget->horizontalHeader()) {
                table_widget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
                table_widget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
                table_widget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
                table_widget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeMode::Stretch);
            }
            table_widget->setFixedHeight(140);

            for (int r_idx = 0; r_idx < static_cast<int>(resultados.size()); ++r_idx) {
                const auto& r = resultados[r_idx];
                auto* it_n = new QTableWidgetItem(QString::number(r.ano));
                auto* it_vr = new QTableWidgetItem(TextFormat::format_currency(r.vr));
                auto* it_com = new QTableWidgetItem(TextFormat::format_currency(r.com));
                auto* it_caue = new QTableWidgetItem(TextFormat::format_currency(r.caue, 2));

                for (auto* it : {it_n, it_vr, it_com, it_caue}) {
                    it->setFlags(it->flags() & ~Qt::ItemFlag::ItemIsEditable);
                }

                table_widget->setItem(r_idx, 0, it_n);
                table_widget->setItem(r_idx, 1, it_vr);
                table_widget->setItem(r_idx, 2, it_com);
                table_widget->setItem(r_idx, 3, it_caue);
            }

            for (int r_idx = 0; r_idx < static_cast<int>(resultados.size()); ++r_idx) {
                if (resultados[r_idx].ano == menor_ano) {
                    for (int c = 0; c < table_widget->columnCount(); ++c) {
                        auto* item = table_widget->item(r_idx, c);
                        if (item) {
                            item->setBackground(QBrush());
                        }
                    }
                }
            }
        } catch (const std::exception& e_tbl) {
            logger.error(QString("Erro ao criar tabela de resultados CAUE para inserir no HistoryContainer: %1").arg(e_tbl.what()));
            table_widget = nullptr;
        }

        // Inserir texto + tabela dentro do HistoryContainer com retranslate_fn
        try {
            auto retranslate_fn = [p, tma, max_years, dados, resultados, menor_ano]() -> QString {
                return format_caue_steps(p, tma, max_years, dados, resultados, menor_ano);
            };
            self->caue_result->append(steps_text, table_widget, retranslate_fn);
        } catch (const std::exception& e_append) {
            logger.error(QString("Erro ao inserir resultado CAUE no HistoryContainer: %1").arg(e_append.what()));
            try {
                self->caue_result->append(steps_text);
            } catch (...) {}
        }

        // Também preencher, para compatibilidade, a caue_output_table (atributo disponível mas não mostrado no layout)
        try {
            if (self->caue_output_table) {
                QTableWidget* tbl = self->caue_output_table;
                tbl->setRowCount(static_cast<int>(resultados.size()));
                for (int r = 0; r < static_cast<int>(resultados.size()); ++r) {
                    const auto& row = resultados[r];
                    auto* it_n = new QTableWidgetItem(QString::number(row.ano));
                    auto* it_vr = new QTableWidgetItem(TextFormat::format_currency(row.vr));
                    auto* it_com = new QTableWidgetItem(TextFormat::format_currency(row.com));
                    auto* it_caue = new QTableWidgetItem(TextFormat::format_currency(row.caue, 2));

                    for (auto* it : {it_n, it_vr, it_com, it_caue}) {
                        it->setFlags(it->flags() & ~Qt::ItemFlag::ItemIsEditable);
                    }

                    tbl->setItem(r, 0, it_n);
                    tbl->setItem(r, 1, it_vr);
                    tbl->setItem(r, 2, it_com);
                    tbl->setItem(r, 3, it_caue);
                }

                for (int r = 0; r < tbl->rowCount(); ++r) {
                    for (int c = 0; c < tbl->columnCount(); ++c) {
                        auto* item = tbl->item(r, c);
                        if (item) {
                            item->setBackground(QBrush());
                        }
                    }
                }
            }
        } catch (const std::exception& e_tbl) {
            logger.error(QString("Erro ao preencher tabela de resultados CAUE (atributo oculto): %1").arg(e_tbl.what()));
        }

    } catch (const std::exception& e) {
        logger.error(QString("Erro ao calcular CAUE: %1").arg(e.what()));
        try {
            QString err = e.what();
            auto retrans = [err]() -> QString {
                return QString("%1: %2").arg(QCoreApplication::translate("App", "Erro"), err);
            };
            self->caue_result->append(retrans(), nullptr, retrans);
        } catch (...) {}
    }
}

void FinancialCalculatorApp::calculate_caue() {
    ::calculate_caue(this);
}
