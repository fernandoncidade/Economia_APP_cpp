#include "ui_25_export_pdf.hpp"
#include "ui_23_history_container.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/FontManager.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../utils/MathRenderer.hpp"

#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>

namespace PdfExport {

void export_to_pdf(QWidget* parent, QWidget* text_widget, const QString& suggested_name) {
    try {
        QString filename = QFileDialog::getSaveFileName(parent, "Salvar como PDF", suggested_name, "PDF Files (*.pdf)");
        if (filename.isEmpty()) {
            return;
        }

        if (!filename.endsWith(".pdf", Qt::CaseInsensitive)) {
            filename += ".pdf";
        }

        QTextDocument doc;
        if (auto* hc = dynamic_cast<HistoryContainer*>(text_widget)) {
            QString raw_text = hc->toRawText();
            if (raw_text.trimmed().isEmpty()) {
                LogManager::warning("Nenhum conteúdo para exportar");
                return;
            }

            QString font_css = FontManager::get_html_style();
            QString rich_body = TextFormat::to_rich_html(raw_text);
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
                "        pre.calc, pre {\n"
                "            white-space: pre-wrap;\n"
                "            overflow-wrap: break-word;\n"
                "            word-break: break-word;\n"
                "        }\n"
                "        sub { font-size: 75%; vertical-align: sub; }\n"
                "        sup { font-size: 75%; vertical-align: super; }\n"
                "    </style>\n"
                "</head>\n"
                "<body>\n"
                "    <pre class=\"calc\">%2</pre>\n"
                "</body>\n"
                "</html>\n"
            ).arg(font_css, rich_body);

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
                QString font_css = FontManager::get_html_style();
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
                    "        pre.calc, pre {\n"
                    "            white-space: pre-wrap;\n"
                    "            overflow-wrap: break-word;\n"
                    "            word-break: break-word;\n"
                    "        }\n"
                    "        sub { font-size: 75%; vertical-align: sub; }\n"
                    "        sup { font-size: 75%; vertical-align: super; }\n"
                    "    </style>\n"
                    "</head>\n"
                    "<body>\n"
                    "    <pre class=\"calc\">%2</pre>\n"
                    "</body>\n"
                    "</html>\n"
                ).arg(font_css, rich_body);
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
            headers << (hi ? hi->text().toHtmlEscaped() : QString("Col %1").arg(c + 1));
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
            "table { border-collapse: collapse; width: 100%; font-size: 10pt; table-layout: fixed; }\n"
            "th, td { border: 1px solid #444; padding: 4px 6px; text-align: left; vertical-align: top; word-break: break-word; overflow-wrap: break-word; }\n"
            "thead tr { background: #f0f0f0; }\n"
            "h1, h2 { margin: 12px 0 6px 0; }\n"
            "pre { white-space: pre-wrap; overflow-wrap: break-word; word-break: break-word; }\n"
            "</style>\n"
        ).arg(font_css);

        QString html = QString(
            "%1\n"
            "<h2>Tabela de Amortização</h2>\n"
            "<table>\n"
            "<thead><tr>\n"
            "<th>%2</th>\n"
            "</tr></thead>\n"
            "<tbody>\n"
            "%3\n"
            "</tbody></table>\n"
        ).arg(table_css, headers.join("</th><th>"), body_rows.join("\n"));

        return html;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar HTML da tabela de amortização: %1").arg(e.what()));
        return QString();
    }
}

void export_amortization_pdf(QWidget* parent, QWidget* calc_widget, QTableWidget* tw, const QString& suggested_name) {
    try {
        QString filename = QFileDialog::getSaveFileName(parent, "Salvar como PDF", suggested_name, "PDF Files (*.pdf)");
        if (filename.isEmpty()) {
            return;
        }

        if (!filename.endsWith(".pdf", Qt::CaseInsensitive)) {
            filename += ".pdf";
        }

        QString calc_text;
        if (auto* hc = dynamic_cast<HistoryContainer*>(calc_widget)) {
            calc_text = hc->toRawText().trimmed();
        } else if (auto* te = dynamic_cast<QTextEdit*>(calc_widget)) {
            calc_text = te->toPlainText().trimmed();
        }

        QString calc_html;
        if (!calc_text.isEmpty()) {
            QString rich_calc = TextFormat::to_rich_html(calc_text);
            if (rich_calc.contains("<table", Qt::CaseInsensitive)) {
                rich_calc.replace("<table", "</pre><table", Qt::CaseInsensitive);
                rich_calc.replace("</table>", "</table><pre class=\"calc\">", Qt::CaseInsensitive);
            }
            calc_html = QString("<h2>Cálculos</h2><pre class=\"calc\">%1</pre>").arg(rich_calc);
        }

        QString table_html = amort_table_to_html(tw);
        QString font_css = FontManager::get_html_style();

        QString full_html = QString(
            "<html>\n"
            "<head>\n"
            "    <meta charset=\"utf-8\">\n"
            "    %1\n"
            "    <style>\n"
            "        body { margin: 12px; }\n"
            "        pre.calc, pre { white-space: pre-wrap; overflow-wrap: break-word; word-break: break-word; }\n"
            "        sub { font-size: 75%; vertical-align: sub; }\n"
            "        sup { font-size: 75%; vertical-align: super; }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <h1>Amortização</h1>\n"
            "    %2\n"
            "    %3\n"
            "</body>\n"
            "</html>\n"
        ).arg(font_css, calc_html, table_html);

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

void FinancialCalculatorApp::export_to_pdf(QWidget* text_widget, const QString& suggested_name) {
    PdfExport::export_to_pdf(this, text_widget, suggested_name);
}

void FinancialCalculatorApp::export_amortization_pdf(const QString& suggested_name) {
    PdfExport::export_amortization_pdf(this, this->amort_result, this->amort_table, suggested_name);
}

QString FinancialCalculatorApp::amort_table_to_html() {
    return PdfExport::amort_table_to_html(this->amort_table);
}
