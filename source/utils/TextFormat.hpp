#ifndef TEXT_FORMAT_HPP
#define TEXT_FORMAT_HPP

#include <QString>
#include <QStringList>
#include <array>

namespace TextFormat {

QString to_subscript(const QString& text);
inline QString to_subscript(long long val) { return to_subscript(QString::number(val)); }
inline QString to_subscript(int val) { return to_subscript(QString::number(val)); }

QString to_superscript(const QString& text);
inline QString to_superscript(long long val) { return to_superscript(QString::number(val)); }
inline QString to_superscript(int val) { return to_superscript(QString::number(val)); }

QString to_superscript_parens(const QString& text);
inline QString to_superscript_parens(long long val) { return to_superscript_parens(QString::number(val)); }
inline QString to_superscript_parens(int val) { return to_superscript_parens(QString::number(val)); }

QString to_html_subscripts(const QString& text);
QString to_unicode_subscripts(const QString& text);
QString to_unicode_superscripts(const QString& text);
QString from_unicode_subscripts(const QString& text);
QString from_unicode_superscripts(const QString& text);
QString format_sub_superscripts(const QString& text);
QString to_rich_html(const QString& text);

QString format_currency(double value, int decimals = 2);

std::array<QString, 3> format_fraction(const QString& numer_str, const QString& denom_str, const QString& prefix = "");
QString format_fraction_inline(const QString& numer_str, const QString& denom_str);

#include <vector>

QString format_equation_steps(const QStringList& steps);

std::vector<QString> format_radical_fraction(
    const QString& index_str,
    const QString& numer_str,
    const QString& denom_str,
    const QString& prefix = "",
    const QString& suffix = ""
);

std::vector<QString> format_radical_single(
    const QString& index_str,
    const QString& val_str,
    const QString& prefix = "",
    const QString& suffix = ""
);

// Renderização matemática nativa em Qt6 (HTML/CSS) para equações com radicais
QString render_radical_fraction_html(
    const QString& index_str,
    const QString& numer_str,
    const QString& denom_str,
    const QString& prefix = "",
    const QString& suffix = ""
);

QString render_radical_single_html(
    const QString& index_str,
    const QString& val_str,
    const QString& prefix = "",
    const QString& suffix = ""
);

} // namespace TextFormat

#endif // TEXT_FORMAT_HPP
