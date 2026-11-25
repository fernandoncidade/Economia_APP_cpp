#include "ui_25_export_pdf.hpp"
#include "ui_23_history_container.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/FontManager.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../utils/MathRenderer.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"

#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QCoreApplication>

namespace PdfExport {

QString localize_filename(const QString& name) {
    bool is_en = (GerenciadorTraducao::idioma_atual_global() == "en_US");
    if (!is_en) {
        if (name == "all_calculations.pdf" || name == "todos_calculos.pdf") return "todos_os_calculos.pdf";
        if (name == "interest.pdf") return "juros.pdf";
        if (name == "annuities.pdf") return "anuidades.pdf";
        if (name == "gradients.pdf" || name == "gradient.pdf") return "gradientes.pdf";
        if (name == "rate_equivalence.pdf" || name == "equivalencia_taxa.pdf") return "equivalencia_taxas.pdf";
        if (name == "real_nominal_rate.pdf") return "taxa_real_aparente.pdf";
        if (name == "amortization.pdf") return "amortizacao.pdf";
        if (name == "investment_analysis.pdf" || name == "investments.pdf" || name == "investimento.pdf") return "analise_investimentos.pdf";
        if (name == "depreciation.pdf") return "depreciacao.pdf";
        if (name == "effective_rate_irr.pdf") return "taxa_efetiva_tir.pdf";
        if (name == "minimum_attractive_rate.pdf" || name == "minimum_mar.pdf" || name == "tma_minima.pdf") return "retorno_minimo_tma.pdf";
        if (name == "fisher_equation.pdf" || name == "fisher.pdf") return "equacao_fisher.pdf";
        if (name == "npv_with_taxes.pdf") return "vpl_tributos.pdf";
        if (name == "euac_economic_life.pdf" || name == "caue.pdf") return "caue_vida_economica.pdf";
        return name;
    } else {
        if (name == "todos_calculos.pdf" || name == "todos_os_calculos.pdf") return "all_calculations.pdf";
        if (name == "juros.pdf") return "interest.pdf";
        if (name == "anuidades.pdf") return "annuities.pdf";
        if (name == "gradiente.pdf" || name == "gradientes.pdf") return "gradients.pdf";
        if (name == "equivalencia_taxa.pdf" || name == "equivalencia_taxas.pdf") return "rate_equivalence.pdf";
        if (name == "taxa_real_aparente.pdf") return "real_nominal_rate.pdf";
        if (name == "amortizacao.pdf") return "amortization.pdf";
        if (name == "investimento.pdf" || name == "analise_investimentos.pdf") return "investment_analysis.pdf";
        if (name == "depreciacao.pdf") return "depreciation.pdf";
        if (name == "taxa_efetiva_tir.pdf") return "effective_rate_irr.pdf";
        if (name == "tma_minima.pdf" || name == "retorno_minimo_tma.pdf") return "minimum_attractive_rate.pdf";
        if (name == "fisher.pdf" || name == "equacao_fisher.pdf") return "fisher_equation.pdf";
        if (name == "vpl_tributos.pdf") return "npv_with_taxes.pdf";
        if (name == "caue.pdf" || name == "caue_vida_economica.pdf") return "euac_economic_life.pdf";
        return name;
    }
}

void export_to_pdf(QWidget* parent, QWidget* text_widget, const QString& suggested_name, const QString& title) {
    try {
        QString actual_name = localize_filename(suggested_name);
        QString dialog_title = QCoreApplication::translate("App", "Salvar como PDF");
        QString filter_desc = QCoreApplication::translate("App", "Arquivos PDF (*.pdf)");
        QString filename = QFileDialog::getSaveFileName(parent, dialog_title, actual_name, filter_desc);
        if (filename.isEmpty()) {
            return;
        }

        if (!filename.endsWith(".pdf", Qt::CaseInsensitive)) {
            filename += ".pdf";
        }

        QTextDocument doc;
        QString font_css = FontManager::get_html_style();
        QString header_html = title.isEmpty() ? QString() : QString("<h1>%1</h1>\n").arg(title.toHtmlEscaped());

        if (auto* hc = dynamic_cast<HistoryContainer*>(text_widget)) {
            QString content = hc->toExportHtml();
            if (content.trimmed().isEmpty()) {
                LogManager::warning("Nenhum conteúdo para exportar");
                return;
            }

            QString html = QString(
                "<html>\n"
                "<head>\n"
                "    <meta charset=\"utf-8\">\n"
                "    %1\n"
                "    <style>\n"
                "        body { margin: 12px; }\n"
                "        h1 { margin: 12px 0 10px 0; }\n"
                "        h2 { margin: 12px 0 6px 0; }\n"
                "        pre.calc, pre {\n"
                "            white-space: pre-wrap;\n"
                "            overflow-wrap: break-word;\n"
                "            word-break: break-word;\n"
                "        }\n"
                "        sub { font-size: 75%; vertical-align: sub; }\n"
                "        sup { font-size: 75%; vertical-align: super; }\n"
                "        table { border-collapse: collapse; width: 100%; font-size: 10pt; margin: 10px 0; table-layout: fixed; }\n"
                "        th, td { border: 1px solid #444; padding: 4px 6px; text-align: left; vertical-align: top; word-break: break-word; overflow-wrap: break-word; }\n"
                "        thead tr { background: #f0f0f0; }\n"
                "    </style>\n"
                "</head>\n"
                "<body>\n"
                "    %2\n"
                "    %3\n"
                "</body>\n"
                "</html>\n"
            ).arg(font_css, header_html, content);

            doc.setHtml(html);
        } else if (auto* te = dynamic_cast<QTextEdit*>(text_widget)) {
            QString doc_html = te->toHtml();
            if (doc_html.contains("<table", Qt::CaseInsensitive) || doc_html.contains("<h1", Qt::CaseInsensitive) || doc_html.contains("<h2", Qt::CaseInsensitive)) {
                doc.setHtml(doc_html);
            } else {
                QString raw = te->toPlainText();
                if (raw.trimmed().isEmpty()) {
                    LogManager::warning("Nenhum conteúdo para exportar");
                    return;
                }
                QString rich_body = TextFormat::to_rich_html(raw);
                if (rich_body.contains("<table", Qt::CaseInsensitive)) {
                    rich_body.replace("<table", "</pre><table", Qt::CaseInsensitive);
                    rich_body.replace("</table>", "</table><pre class=\"calc\">", Qt::CaseInsensitive);
                }
                QString html = QString(
                    "<html>\n"
                    "<head>\n"
                    "    <meta charset=\"utf-8\">\n"
                    "    %1\n"
                    "    <style>\n"
                    "        body { margin: 12px; }\n"
                    "        h1 { margin: 12px 0 10px 0; }\n"
                    "        h2 { margin: 12px 0 6px 0; }\n"
                    "        pre.calc, pre {\n"
                    "            white-space: pre-wrap;\n"
                    "            overflow-wrap: break-word;\n"
                    "            word-break: break-word;\n"
                    "        }\n"
                    "        sub { font-size: 75%; vertical-align: sub; }\n"
                    "        sup { font-size: 75%; vertical-align: super; }\n"
                    "        table { border-collapse: collapse; width: 100%; font-size: 10pt; margin: 10px 0; table-layout: fixed; }\n"
                    "        th, td { border: 1px solid #444; padding: 4px 6px; text-align: left; vertical-align: top; word-break: break-word; overflow-wrap: break-word; }\n"
                    "        thead tr { background: #f0f0f0; }\n"
                    "    </style>\n"
                    "</head>\n"
                    "<body>\n"
                    "    %2\n"
                    "    <pre class=\"calc\">%3</pre>\n"
                    "</body>\n"
                    "</html>\n"
                ).arg(font_css, header_html, rich_body);
                doc.setHtml(html);
            }
        }

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);

        MathRenderer::apply_radicals(&doc);
        doc.print(&printer);

        LogManager::info(QString("PDF exportado com sucesso: %1").arg(filename));
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao exportar para PDF: %1").arg(e.what()));
    }
}

QString amort_table_to_html(QTableWidget* tw) {
    try {
        if (!tw) return QString();

        int cols = tw->columnCount();
        int rows = tw->rowCount();

        QStringList headers;
        for (int c = 0; c < cols; ++c) {
            auto* hi = tw->horizontalHeaderItem(c);
            QString hText = hi ? hi->text() : QString("Col %1").arg(c + 1);
            if (hi && !hi->data(Qt::UserRole).toString().isEmpty()) {
                hText = QCoreApplication::translate("App", hi->data(Qt::UserRole).toString().toUtf8().constData());
            } else if (hi) {
                hText = QCoreApplication::translate("App", hText.toUtf8().constData());
            }
            headers << hText.toHtmlEscaped();
        }

        QStringList body_rows;
        for (int r = 0; r < rows; ++r) {
            QString row_str = "<tr>";
            for (int c = 0; c < cols; ++c) {
                auto* it = tw->item(r, c);
                row_str += QString("<td>%1</td>").arg(it ? it->text().toHtmlEscaped() : "");
            }
            row_str += "</tr>";
            body_rows << row_str;
        }

        QString font_css = FontManager::get_html_style();
        QString table_css = QString(
            "%1\n"
            "<style>\n"
            "table { border-collapse: collapse; width: 100%; font-size: 10pt; table-layout: fixed; margin: 10px 0; }\n"
            "th, td { border: 1px solid #444; padding: 4px 6px; text-align: left; vertical-align: top; word-break: break-word; overflow-wrap: break-word; }\n"
            "thead tr { background: #f0f0f0; }\n"
            "h1, h2 { margin: 12px 0 6px 0; }\n"
            "pre { white-space: pre-wrap; overflow-wrap: break-word; word-break: break-word; }\n"
            "</style>\n"
        ).arg(font_css);

        QString h2_title = QCoreApplication::translate("App", "Tabela de Amortização");
        QString html = QString(
            "%1\n"
            "<h2>%2</h2>\n"
            "<table>\n"
            "<thead><tr>\n"
            "<th>%3</th>\n"
            "</tr></thead>\n"
            "<tbody>\n"
            "%4\n"
            "</tbody></table>\n"
        ).arg(table_css, h2_title.toHtmlEscaped(), headers.join("</th><th>"), body_rows.join("\n"));

        return html;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar HTML da tabela de amortização: %1").arg(e.what()));
        return QString();
    }
}

void export_amortization_pdf(QWidget* parent, QWidget* calc_widget, QTableWidget* tw, const QString& suggested_name) {
    try {
        QString actual_name = localize_filename(suggested_name);
        QString dialog_title = QCoreApplication::translate("App", "Salvar como PDF");
        QString filter_desc = QCoreApplication::translate("App", "Arquivos PDF (*.pdf)");
        QString filename = QFileDialog::getSaveFileName(parent, dialog_title, actual_name, filter_desc);
        if (filename.isEmpty()) {
            return;
        }

        if (!filename.endsWith(".pdf", Qt::CaseInsensitive)) {
            filename += ".pdf";
        }

        QString content_html;
        if (auto* hc = dynamic_cast<HistoryContainer*>(calc_widget)) {
            content_html = hc->toExportHtml();
        } else if (auto* te = dynamic_cast<QTextEdit*>(calc_widget)) {
            QString raw = te->toPlainText().trimmed();
            if (!raw.isEmpty()) {
                QString rich = TextFormat::to_rich_html(raw);
                content_html = QString("<pre class=\"calc\">%1</pre>").arg(rich);
            }
        }

        if (!content_html.contains("<table", Qt::CaseInsensitive) && tw) {
            content_html += "\n" + amort_table_to_html(tw);
        }

        QString font_css = FontManager::get_html_style();
        QString h1_title = QCoreApplication::translate("App", "Amortização");

        QString full_html = QString(
            "<html>\n"
            "<head>\n"
            "    <meta charset=\"utf-8\">\n"
            "    %1\n"
            "    <style>\n"
            "        body { margin: 12px; }\n"
            "        h1 { margin: 12px 0 10px 0; }\n"
            "        h2 { margin: 12px 0 6px 0; }\n"
            "        pre.calc, pre { white-space: pre-wrap; overflow-wrap: break-word; word-break: break-word; }\n"
            "        sub { font-size: 75%; vertical-align: sub; }\n"
            "        sup { font-size: 75%; vertical-align: super; }\n"
            "        table { border-collapse: collapse; width: 100%; font-size: 10pt; margin: 10px 0; table-layout: fixed; }\n"
            "        th, td { border: 1px solid #444; padding: 4px 6px; text-align: left; vertical-align: top; word-break: break-word; }\n"
            "        thead tr { background: #f0f0f0; }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>%2</h1>\n"
            "    %3\n"
            "</body>\n"
            "</html>\n"
        ).arg(font_css, h1_title.toHtmlEscaped(), content_html);

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);

        QTextDocument doc;
        doc.setHtml(full_html);
        MathRenderer::apply_radicals(&doc);
        doc.print(&printer);

        LogManager::info(QString("PDF de amortização exportado com sucesso: %1").arg(filename));
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao exportar amortização para PDF: %1").arg(e.what()));
    }
}

} // namespace PdfExport

void FinancialCalculatorApp::export_to_pdf(QWidget* text_widget, const QString& suggested_name, const QString& title) {
    PdfExport::export_to_pdf(this, text_widget, suggested_name, title);
}

void FinancialCalculatorApp::export_amortization_pdf(const QString& suggested_name) {
    PdfExport::export_amortization_pdf(this, this->amort_result, this->amort_table, suggested_name);
}

QString FinancialCalculatorApp::amort_table_to_html() {
    return PdfExport::amort_table_to_html(this->amort_table);
}
