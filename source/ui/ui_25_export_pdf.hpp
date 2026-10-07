#ifndef UI_25_EXPORT_PDF_HPP
#define UI_25_EXPORT_PDF_HPP

#include <QString>

class QWidget;
class QTableWidget;

namespace PdfExport {

void export_to_pdf(QWidget* parent, QWidget* text_widget, const QString& suggested_name = "export.pdf");
QString amort_table_to_html(QTableWidget* tw);
void export_amortization_pdf(QWidget* parent, QWidget* calc_widget, QTableWidget* tw, const QString& suggested_name = "amortizacao.pdf");

} // namespace PdfExport

#endif // UI_25_EXPORT_PDF_HPP
