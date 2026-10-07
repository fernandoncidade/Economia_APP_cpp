#include "TextFormat.hpp"
#include "MathRenderer.hpp"
#include <QRegularExpression>
#include <QMap>
#include <cmath>

namespace TextFormat {

static const QMap<QChar, QString>& get_sub_map() {
    static const QMap<QChar, QString> sub_map = {
        {'0', "₀"}, {'1', "₁"}, {'2', "₂"}, {'3', "₃"}, {'4', "₄"},
        {'5', "₅"}, {'6', "₆"}, {'7', "₇"}, {'8', "₈"}, {'9', "₉"},
        {'a', "ₐ"}, {'b', "ᵦ"}, {'c', "ᴄ"}, {'d', "ᴅ"}, {'e', "ₑ"}, {'f', "ꜰ"},
        {'g', "ɢ"}, {'h', "ₕ"}, {'i', "ᵢ"}, {'j', "ⱼ"}, {'k', "ₖ"}, {'l', "ₗ"},
        {'m', "ₘ"}, {'n', "ₙ"}, {'o', "ₒ"}, {'p', "ₚ"}, {'q', "q"},
        {'r', "ᵣ"}, {'s', "ₛ"}, {'t', "ₜ"}, {'u', "ᵤ"}, {'v', "ᵥ"}, {'w', "ᴡ"},
        {'x', "ₓ"}, {'y', "ʏ"}, {'z', "ᴢ"},
        {'A', "ₐ"}, {'B', "ᵦ"}, {'C', "ᴄ"}, {'D', "ᴅ"}, {'E', "ₑ"}, {'F', "ꜰ"},
        {'G', "ɢ"}, {'H', "ₕ"}, {'I', "ᵢ"}, {'J', "ⱼ"}, {'K', "ₖ"}, {'L', "ₗ"},
        {'M', "ₘ"}, {'N', "ₙ"}, {'O', "ₒ"}, {'P', "ₚ"}, {'Q', "q"},
        {'R', "ᵣ"}, {'S', "ₛ"}, {'T', "ₜ"}, {'U', "ᵤ"}, {'V', "ᵥ"}, {'W', "ᴡ"},
        {'X', "ₓ"}, {'Y', "ʏ"}, {'Z', "ᴢ"},
        {'+', "₊"}, {'-', "₋"}, {'=', "₌"}, {'(', "₍"}, {')', "₎"},
        {'/', "/"}, {'.', "."}, {',', ","},
        // Accented characters for Portuguese financial subscripts
        {QChar(0x00ED), "ᵢ"}, {QChar(0x00CD), "ᵢ"}, // í, Í
        {QChar(0x00E9), "ₑ"}, {QChar(0x00C9), "ₑ"}, // é, É
        {QChar(0x00E1), "ₐ"}, {QChar(0x00C1), "ₐ"}, // á, Á
        {QChar(0x00F3), "ₒ"}, {QChar(0x00D3), "ₒ"}, // ó, Ó
        {QChar(0x00FA), "ᵤ"}, {QChar(0x00DA), "ᵤ"}, // ú, Ú
        {QChar(0x00EA), "ₑ"}, {QChar(0x00CA), "ₑ"}, // ê, Ê
        {QChar(0x00E3), "ₐ"}, {QChar(0x00C3), "ₐ"}, // ã, Ã
        {QChar(0x00F5), "ₒ"}, {QChar(0x00D5), "ₒ"}, // õ, Õ
        {QChar(0x00E7), "ᴄ"}, {QChar(0x00C7), "ᴄ"}, // ç, Ç
        // Superscript to subscript
        {QChar(0x2070), "₀"}, {QChar(0x00B9), "₁"}, {QChar(0x00B2), "₂"}, {QChar(0x00B3), "₃"},
        {QChar(0x2074), "₄"}, {QChar(0x2075), "₅"}, {QChar(0x2076), "₆"}, {QChar(0x2077), "₇"},
        {QChar(0x2078), "₈"}, {QChar(0x2079), "₉"},
        {QChar(0x207A), "₊"}, {QChar(0x207B), "₋"}, {QChar(0x207C), "₌"},
        {QChar(0x207D), "₍"}, {QChar(0x207E), "₎"},
        {QChar(0x1D43), "ₐ"}, {QChar(0x1D49), "ₑ"}, {QChar(0x207F), "ₙ"},
        {QChar(0x1D50), "ₒ"}, {QChar(0x02E2), "ₛ"}, {QChar(0x1D57), "ₜ"},
        {QChar(0x02E3), "ₓ"}
    };
    return sub_map;
}

static const QMap<QChar, QString>& get_super_map() {
    static const QMap<QChar, QString> super_map = {
        {'0', "⁰"}, {'1', "¹"}, {'2', "²"}, {'3', "³"}, {'4', "⁴"},
        {'5', "⁵"}, {'6', "⁶"}, {'7', "⁷"}, {'8', "⁸"}, {'9', "⁹"},
        {'+', "⁺"}, {'-', "⁻"}, {'=', "⁼"}, {'(', "⁽"}, {')', "⁾"},
        {'.', "·"}, {',', "·"}, {'/', "/"},
        {'a', "ᵃ"}, {'b', "ᵇ"}, {'c', "ᶜ"}, {'d', "ᵈ"}, {'e', "ᵉ"}, {'f', "ᶠ"},
        {'g', "ᵍ"}, {'h', "ʰ"}, {'i', "ⁱ"}, {'j', "ʲ"}, {'k', "ᵏ"}, {'l', "ˡ"},
        {'m', "ᵐ"}, {'n', "ⁿ"}, {'o', "ᵒ"}, {'p', "ᵖ"}, {'r', "ʳ"}, {'s', "ˢ"},
        {'t', "ᵗ"}, {'u', "ᵘ"}, {'v', "ᵛ"}, {'w', "ʷ"}, {'x', "ˣ"}, {'y', "ʸ"}, {'z', "ᶻ"},
        {'A', "ᵃ"}, {'B', "ᵇ"}, {'C', "ᶜ"}, {'D', "ᵈ"}, {'E', "ᵉ"}, {'F', "ᶠ"},
        {'G', "ᵍ"}, {'H', "ʰ"}, {'I', "ⁱ"}, {'J', "ʲ"}, {'K', "ᵏ"}, {'L', "ˡ"},
        {'M', "ᵐ"}, {'N', "ⁿ"}, {'O', "ᵒ"}, {'P', "ᵖ"}, {'R', "ʳ"}, {'S', "ˢ"},
        {'T', "ᵗ"}, {'U', "ᵘ"}, {'V', "ᵛ"}, {'W', "ʷ"}, {'X', "ˣ"}, {'Y', "ʸ"}, {'Z', "ᶻ"},
        // Accented characters for Portuguese financial superscripts
        {QChar(0x00ED), "ⁱ"}, {QChar(0x00CD), "ⁱ"}, // í, Í
        {QChar(0x00E9), "ᵉ"}, {QChar(0x00C9), "ᵉ"}, // é, É
        {QChar(0x00E1), "ᵃ"}, {QChar(0x00C1), "ᵃ"}, // á, Á
        {QChar(0x00F3), "ᵒ"}, {QChar(0x00D3), "ᵒ"}, // ó, Ó
        {QChar(0x00FA), "ᵘ"}, {QChar(0x00DA), "ᵘ"}, // ú, Ú
        {QChar(0x00EA), "ᵉ"}, {QChar(0x00CA), "ᵉ"}, // ê, Ê
        {QChar(0x00E3), "ᵃ"}, {QChar(0x00C3), "ᵃ"}, // ã, Ã
        {QChar(0x00F5), "ᵒ"}, {QChar(0x00D5), "ᵒ"}, // õ, Õ
        {QChar(0x00E7), "ᶜ"}, {QChar(0x00C7), "ᶜ"}  // ç, Ç
    };
    return super_map;
}

QString to_subscript(const QString& text) {
    const auto& map = get_sub_map();
    QString result;
    for (const QChar& c : text) {
        if (map.contains(c)) {
            result += map[c];
        } else {
            result += c;
        }
    }
    return result;
}

QString to_superscript(const QString& text) {
    const auto& map = get_super_map();
    QString result;
    for (const QChar& c : text) {
        if (map.contains(c)) {
            result += map[c];
        } else {
            result += c;
        }
    }
    return result;
}

QString to_superscript_parens(const QString& text) {
    return QString("⁽%1⁾").arg(to_superscript(text));
}

QString to_html_subscripts(const QString& text) {
    static const QRegularExpression regex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])_(\{[^}]+\}|\([^)]+\)|[A-Za-z0-9+\-*/.,]+)(?![A-Za-z0-9_]))");
    QString result = text;
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }
    for (int i = matches.size() - 1; i >= 0; --i) {
        const auto& match = matches[i];
        QString base = match.captured(1);
        QString idx = match.captured(2);
        if (idx.startsWith('{') && idx.endsWith('}')) {
            idx = idx.mid(1, idx.length() - 2);
        }
        QString replacement = QString("%1<sub>%2</sub>").arg(base, idx);
        result.replace(match.capturedStart(), match.capturedLength(), replacement);
    }
    return result;
}

QString to_unicode_subscripts(const QString& text) {
    static const QRegularExpression regex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])_(\{[^}]+\}|\([^)]+\)|[A-Za-z0-9+\-*/.,]+)(?![A-Za-z0-9_]))");
    QString result = text;
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }
    for (int i = matches.size() - 1; i >= 0; --i) {
        const auto& match = matches[i];
        QString base = match.captured(1);
        QString idx = match.captured(2);
        if (idx.startsWith('{') && idx.endsWith('}')) {
            idx = idx.mid(1, idx.length() - 2);
        }
        QString replacement = base + to_subscript(idx);
        result.replace(match.capturedStart(), match.capturedLength(), replacement);
    }
    return result;
}

QString to_unicode_superscripts(const QString& text) {
    static const QRegularExpression regex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])\^(\{[^}]+\}|\([^)]+\)|[A-Za-z0-9+\-*/.,]+)(?![A-Za-z0-9^]))");
    QString result = text;
    QRegularExpressionMatchIterator it = regex.globalMatch(text);
    
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }
    for (int i = matches.size() - 1; i >= 0; --i) {
        const auto& match = matches[i];
        QString base = match.captured(1);
        QString idx = match.captured(2);
        if (idx.startsWith('{') && idx.endsWith('}')) {
            idx = idx.mid(1, idx.length() - 2);
        }
        QString replacement = base + to_superscript(idx);
        result.replace(match.capturedStart(), match.capturedLength(), replacement);
    }
    return result;
}

QString from_unicode_subscripts(const QString& text) {
    static QMap<QString, QChar> reverse_map;
    static bool initialized = false;
    if (!initialized) {
        const auto& sub_map = get_sub_map();
        for (auto it = sub_map.constBegin(); it != sub_map.constEnd(); ++it) {
            if (QString(it.key()) != it.value()) {
                reverse_map[it.value()] = it.key();
            }
        }
        initialized = true;
    }

    QString result;
    result.reserve(text.size());
    int i = 0;
    while (i < text.size()) {
        bool matched = false;
        for (int len = qMin(4, text.size() - i); len >= 1; --len) {
            QString candidate = text.mid(i, len);
            if (reverse_map.contains(candidate)) {
                result += reverse_map[candidate];
                i += len;
                matched = true;
                break;
            }
        }
        if (!matched) {
            result += text[i];
            ++i;
        }
    }
    return result;
}

QString from_unicode_superscripts(const QString& text) {
    static QMap<QString, QChar> reverse_super_map;
    static bool initialized = false;
    if (!initialized) {
        const auto& super_map = get_super_map();
        for (auto it = super_map.constBegin(); it != super_map.constEnd(); ++it) {
            if (QString(it.key()) != it.value()) {
                reverse_super_map[it.value()] = it.key();
            }
        }
        initialized = true;
    }

    QString result;
    result.reserve(text.size());
    int i = 0;
    while (i < text.size()) {
        bool matched = false;
        for (int len = qMin(4, text.size() - i); len >= 1; --len) {
            QString candidate = text.mid(i, len);
            if (reverse_super_map.contains(candidate)) {
                result += reverse_super_map[candidate];
                i += len;
                matched = true;
                break;
            }
        }
        if (!matched) {
            result += text[i];
            ++i;
        }
    }
    return result;
}

QString format_sub_superscripts(const QString& text) {
    return to_unicode_superscripts(to_unicode_subscripts(text));
}

QString format_currency(double value, int decimals) {
    if (std::isnan(value) || std::isinf(value)) {
        return QString::number(value);
    }
    // Format: 1,234.56 -> swap commas and dots -> 1.234,56
    QString s = QString("%L1").arg(value, 0, 'f', decimals);
    // Standard Qt %L1 with Brazilian locale or manual replacement:
    // Python code:
    // s = f"{value:,.{decimals}f}".replace(",", "T").replace(".", ",").replace("T", ".")
    // Let's do exact Python logic:
    bool negative = value < 0;
    double abs_val = std::abs(value);
    QString raw = QString::number(abs_val, 'f', decimals);
    QStringList parts = raw.split('.');
    QString int_part = parts[0];
    QString dec_part = parts.size() > 1 ? parts[1] : "";

    QString with_thousands;
    int len = int_part.length();
    for (int i = 0; i < len; ++i) {
        if (i > 0 && (len - i) % 3 == 0) {
            with_thousands += '.';
        }
        with_thousands += int_part[i];
    }
    QString res = negative ? "-" : "";
    res += with_thousands;
    if (decimals > 0) {
        res += ',' + dec_part;
    }
    return res;
}

std::array<QString, 3> format_fraction(const QString& numer_str, const QString& denom_str, const QString& prefix) {
    int width = std::max({static_cast<int>(numer_str.length()), static_cast<int>(denom_str.length()), 3});
    QString pad(prefix.length(), ' ');
    
    // Centering helper
    auto center_str = [](const QString& s, int w) {
        int total_pad = w - s.length();
        if (total_pad <= 0) return s;
        int left_pad = total_pad / 2;
        int right_pad = total_pad - left_pad;
        return QString(left_pad, ' ') + s + QString(right_pad, ' ');
    };

    QString numer_line = pad + center_str(numer_str, width);
    QString divider_line = prefix + QString(width, QChar(0x2500)); // "─"
    QString denom_line = pad + center_str(denom_str, width);

    return {numer_line, divider_line, denom_line};
}

QString format_fraction_inline(const QString& numer_str, const QString& denom_str) {
    return QString("%1 / %2").arg(numer_str, denom_str);
}

QString format_equation_steps(const QStringList& steps) {
    QStringList formatted;
    for (const QString& step : steps) {
        formatted << "  " + step;
    }
    return formatted.join("\n");
}

std::vector<QString> format_radical_fraction(
    const QString& index_str,
    const QString& numer_str,
    const QString& denom_str,
    const QString& prefix,
    const QString& suffix)
{
    int w = std::max({static_cast<int>(numer_str.length()), static_cast<int>(denom_str.length()), 5}) + 2;
    auto center_str = [](const QString& s, int width) {
        int total = width - s.length();
        if (total <= 0) return s;
        int left = total / 2;
        int right = total - left;
        return QString(left, ' ') + s + QString(right, ' ');
    };

    QString numer_c = center_str(numer_str, w);
    QString denom_c = center_str(denom_str, w);

    QString bar_top = QString(QChar(0x250C)) + QString(w + 1, QChar(0x2500)); // ┌ + ─*(w+1)
    QString bar_div = QString(w, QChar(0x2500)); // ─*w

    int p_len = prefix.length();
    QString p_spaces(p_len, ' ');

    QString idx = index_str;
    if (idx.length() < 3) {
        idx = QString(3 - idx.length(), ' ') + idx;
    }

    // Line 0: top vinculum
    QString l0 = p_spaces + "      " + bar_top;
    // Line 1: index + diagonal + numer
    QString l1 = p_spaces + " " + idx + "  /  " + numer_c;
    // Line 2: prefix + radical bottom + divider + suffix
    QString l2 = prefix + "  \\/  " + bar_div + suffix;
    // Line 3: denom
    QString l3 = p_spaces + "      " + " " + denom_c;

    return {l0, l1, l2, l3};
}

std::vector<QString> format_radical_single(
    const QString& index_str,
    const QString& val_str,
    const QString& prefix,
    const QString& suffix)
{
    int w = val_str.length() + 2;
    QString bar_top = QString(QChar(0x250C)) + QString(w, QChar(0x2500));

    int p_len = prefix.length();
    QString p_spaces(p_len, ' ');

    QString idx = index_str;
    if (idx.length() < 3) {
        idx = QString(3 - idx.length(), ' ') + idx;
    }

    QString l0 = p_spaces + "    " + bar_top;
    QString l1 = prefix + idx + QChar(0x221A) + " " + val_str + suffix;

    return {l0, l1};
}

QString render_radical_fraction_html(
    const QString& index_str,
    const QString& numer_str,
    const QString& denom_str,
    const QString& prefix,
    const QString& suffix)
{
    return MathRenderer::render_radical_fraction_html(index_str, numer_str, denom_str, prefix, suffix);
}

QString render_radical_single_html(
    const QString& index_str,
    const QString& val_str,
    const QString& prefix,
    const QString& suffix)
{
    return MathRenderer::render_radical_single_html(index_str, val_str, prefix, suffix);
}

QString to_rich_html(const QString& text) {
    // Preserve existing HTML tags if any (including <img> tags)
    static const QRegularExpression tagRegex(R"(</?(?:sub|sup|b|i|u|span|pre|div|table|tr|td|th|thead|tbody|p|h[1-6]|font|br|img)[^>]*>)", QRegularExpression::CaseInsensitiveOption);
    
    QString result;
    int lastPos = 0;
    auto it = tagRegex.globalMatch(text);
    
    struct Chunk {
        QString str;
        bool isTag;
    };
    QList<Chunk> chunks;

    while (it.hasNext()) {
        auto m = it.next();
        if (m.capturedStart() > lastPos) {
            chunks.append({text.mid(lastPos, m.capturedStart() - lastPos), false});
        }
        chunks.append({m.captured(), true});
        lastPos = m.capturedEnd();
    }
    if (lastPos < text.length()) {
        chunks.append({text.mid(lastPos), false});
    }

    // Helper map for Unicode subscripts to ASCII characters
    static const QMap<QString, QString> uni_sub = {
        {"₀", "0"}, {"₁", "1"}, {"₂", "2"}, {"₃", "3"}, {"₄", "4"},
        {"₅", "5"}, {"₆", "6"}, {"₇", "7"}, {"₈", "8"}, {"₉", "9"},
        {"ₐ", "a"}, {"ᵦ", "b"}, {"ᴄ", "c"}, {"ᴅ", "d"}, {"ₑ", "e"}, {"ꜰ", "f"},
        {"ɢ", "g"}, {"ₕ", "h"}, {"ᵢ", "i"}, {"ⱼ", "j"}, {"ₖ", "k"}, {"ₗ", "l"},
        {"ₘ", "m"}, {"ₙ", "n"}, {"ₒ", "o"}, {"ₚ", "p"}, {"ᵩ", "q"},
        {"ᵣ", "r"}, {"ₛ", "s"}, {"ₜ", "t"}, {"ᵤ", "u"}, {"ᵥ", "v"}, {"ᴡ", "w"},
        {"ₓ", "x"}, {"ʏ", "y"}, {"ᴢ", "z"},
        {"₊", "+"}, {"₋", "-"}, {"₌", "="}
    };

    // Helper map for Unicode superscripts to ASCII characters
    static const QMap<QString, QString> uni_sup = {
        {"⁰", "0"}, {"¹", "1"}, {"²", "2"}, {"³", "3"}, {"⁴", "4"},
        {"⁵", "5"}, {"⁶", "6"}, {"⁷", "7"}, {"⁸", "8"}, {"⁹", "9"},
        {"ᵃ", "a"}, {"ᵇ", "b"}, {"ᶜ", "c"}, {"ᵈ", "d"}, {"ᵉ", "e"},
        {"ᶠ", "f"}, {"ᵍ", "g"}, {"ʰ", "h"}, {"ⁱ", "i"}, {"ʲ", "j"},
        {"ᵏ", "k"}, {"ˡ", "l"}, {"ᵐ", "m"}, {"ⁿ", "n"}, {"ᵒ", "o"},
        {"ᵖ", "p"}, {"ʳ", "r"}, {"ˢ", "s"}, {"ᵗ", "t"}, {"ᵘ", "u"},
        {"ᵛ", "v"}, {"ʷ", "w"}, {"ˣ", "x"}, {"ʸ", "y"}, {"ᶻ", "z"},
        {"⁺", "+"}, {"⁻", "-"}, {"⁼", "="}
    };

    auto decode_inner_exp = [&](const QString& inner_raw) -> QString {
        QString s = inner_raw;
        // First handle specific patterns like n_2, n_1, n2, n1, n², n¹, etc.
        s.replace("ⁿ₂", "n<sub>2</sub>");
        s.replace("ⁿ₁", "n<sub>1</sub>");
        s.replace("ⁿ²", "n<sub>2</sub>");
        s.replace("ⁿ¹", "n<sub>1</sub>");
        s.replace("n₂", "n<sub>2</sub>");
        s.replace("n₁", "n<sub>1</sub>");
        s.replace("n²", "n<sub>2</sub>");
        s.replace("n¹", "n<sub>1</sub>");

        // Convert any unicode super/sub in inner
        for (auto uit = uni_sup.constBegin(); uit != uni_sup.constEnd(); ++uit) {
            if (s.contains(uit.key())) {
                s.replace(uit.key(), uit.value());
            }
        }
        for (auto uit = uni_sub.constBegin(); uit != uni_sub.constEnd(); ++uit) {
            if (s.contains(uit.key())) {
                s.replace(uit.key(), QString("<sub>%1</sub>").arg(uit.value()));
            }
        }
        // Handle n_1, n_2, k-1, etc.
        static const QRegularExpression subInExp(R"(([A-Za-z])_([0-9A-Za-z]+))");
        s.replace(subInExp, R"(\1<sub>\2</sub>)");
        s.replace("</sub><sub>", "");
        return s;
    };

    for (const auto& ch : chunks) {
        if (ch.isTag) {
            result += ch.str;
            continue;
        }

        QString s = ch.str.toHtmlEscaped();

        // 1. Normalize financial equivalents:
        s.replace("iₑᵩ", "i<sub>eq</sub>");
        s.replace("iₑq", "i<sub>eq</sub>");
        s.replace("ₑᵩ", "<sub>eq</sub>");
        s.replace("ₑq", "<sub>eq</sub>");
        s.replace("i_eq", "i<sub>eq</sub>");
        s.replace("i_{eq}", "i<sub>eq</sub>");

        s.replace("iₚₑᵣᵢₒᴅₒ", "i<sub>período</sub>");
        s.replace("iₚₑᵣᵢₒdₒ", "i<sub>período</sub>");
        s.replace("i_período", "i<sub>período</sub>");
        s.replace("i_periodo", "i<sub>período</sub>");
        s.replace("i_{período}", "i<sub>período</sub>");
        s.replace("i_{periodo}", "i<sub>período</sub>");
        s.replace("i_{perioDo}", "i<sub>período</sub>");
        s.replace("i_perioDo", "i<sub>período</sub>");
        s.replace("iperioDo", "i<sub>período</sub>");
        s.replace("iperiodo", "i<sub>período</sub>");

        s.replace("iₚₑᵣᵢₒᵈ", "i<sub>period</sub>");
        s.replace("iₚₑᵣᵢₒᴅ", "i<sub>period</sub>");
        s.replace("i_period", "i<sub>period</sub>");
        s.replace("i_{period}", "i<sub>period</sub>");
        s.replace("iperiod", "i<sub>period</sub>");

        s.replace("iₐₙᵤₐₗ", "i<sub>anual</sub>");
        s.replace("i_anual", "i<sub>anual</sub>");
        s.replace("i_{anual}", "i<sub>anual</sub>");
        s.replace("ianual", "i<sub>anual</sub>");

        s.replace("iₐₙₙᵤₐₗ", "i<sub>annual</sub>");
        s.replace("i_annual", "i<sub>annual</sub>");
        s.replace("i_{annual}", "i<sub>annual</sub>");
        s.replace("iannual", "i<sub>annual</sub>");

        // 2. Handle specific combinations like (1 + i)⁽ⁿ₂/ⁿ₁⁾ or (1 + i)⁽ⁿ²/ⁿ¹⁾
        s.replace("⁽ⁿ₂/ⁿ₁⁾", "<sup>(n<sub>2</sub>/n<sub>1</sub>)</sup>");
        s.replace("⁽ⁿ²/ⁿ¹⁾", "<sup>(n<sub>2</sub>/n<sub>1</sub>)</sup>");

        // 3. Handle Unicode superscript parens: ⁽...⁾
        {
            static const QRegularExpression uniParenRegex(R"(⁽([^⁾]+)⁾)");
            auto mit = uniParenRegex.globalMatch(s);
            QList<QRegularExpressionMatch> matches;
            while (mit.hasNext()) matches.append(mit.next());
            for (int i = matches.size() - 1; i >= 0; --i) {
                const auto& m = matches[i];
                QString inner = decode_inner_exp(m.captured(1));
                QString rep = QString("<sup>(%1)</sup>").arg(inner);
                s.replace(m.capturedStart(), m.capturedLength(), rep);
            }
        }

        // 4. Handle exponents with parentheses: Base^(...)
        {
            static const QRegularExpression expParenRegex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])\^\(([^)]+)\))");
            auto mit = expParenRegex.globalMatch(s);
            QList<QRegularExpressionMatch> matches;
            while (mit.hasNext()) matches.append(mit.next());
            for (int i = matches.size() - 1; i >= 0; --i) {
                const auto& m = matches[i];
                QString base = m.captured(1);
                QString inner = decode_inner_exp(m.captured(2));
                QString rep = QString("%1<sup>(%2)</sup>").arg(base, inner);
                s.replace(m.capturedStart(), m.capturedLength(), rep);
            }
        }

        // 5. Handle exponents with braces: Base^{...}
        {
            static const QRegularExpression expBraceRegex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])\^\{([^}]+)\})");
            auto mit = expBraceRegex.globalMatch(s);
            QList<QRegularExpressionMatch> matches;
            while (mit.hasNext()) matches.append(mit.next());
            for (int i = matches.size() - 1; i >= 0; --i) {
                const auto& m = matches[i];
                QString base = m.captured(1);
                QString inner = decode_inner_exp(m.captured(2));
                QString rep = QString("%1<sup>%2</sup>").arg(base, inner);
                s.replace(m.capturedStart(), m.capturedLength(), rep);
            }
        }

        // 6. Handle simple exponents: Base^exp
        {
            static const QRegularExpression expSimpleRegex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])\^([A-Za-z0-9+\-*/.,]+))");
            auto mit = expSimpleRegex.globalMatch(s);
            QList<QRegularExpressionMatch> matches;
            while (mit.hasNext()) matches.append(mit.next());
            for (int i = matches.size() - 1; i >= 0; --i) {
                const auto& m = matches[i];
                QString base = m.captured(1);
                QString exp = m.captured(2);
                QString rep = QString("%1<sup>%2</sup>").arg(base, exp);
                s.replace(m.capturedStart(), m.capturedLength(), rep);
            }
        }

        // 7. Handle subscripts with braces: Base_{...}
        {
            static const QRegularExpression subBraceRegex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])_\{([^}]+)\})");
            auto mit = subBraceRegex.globalMatch(s);
            QList<QRegularExpressionMatch> matches;
            while (mit.hasNext()) matches.append(mit.next());
            for (int i = matches.size() - 1; i >= 0; --i) {
                const auto& m = matches[i];
                QString base = m.captured(1);
                QString sub = m.captured(2);
                QString rep = QString("%1<sub>%2</sub>").arg(base, sub);
                s.replace(m.capturedStart(), m.capturedLength(), rep);
            }
        }

        // 8. Handle simple subscripts: Base_sub
        {
            static const QRegularExpression subSimpleRegex(R"(([A-Za-zÀ-ÖØ-öø-ÿ0-9\)])_([A-Za-zÀ-ÖØ-öø-ÿ0-9+\-*/.,]+)(?![A-Za-zÀ-ÖØ-öø-ÿ0-9_]))");
            auto mit = subSimpleRegex.globalMatch(s);
            QList<QRegularExpressionMatch> matches;
            while (mit.hasNext()) matches.append(mit.next());
            for (int i = matches.size() - 1; i >= 0; --i) {
                const auto& m = matches[i];
                QString base = m.captured(1);
                QString sub = m.captured(2);
                QString rep = QString("%1<sub>%2</sub>").arg(base, sub);
                s.replace(m.capturedStart(), m.capturedLength(), rep);
            }
        }

        // 9. Replace remaining unicode subscripts
        for (auto uit = uni_sub.constBegin(); uit != uni_sub.constEnd(); ++uit) {
            if (s.contains(uit.key())) {
                s.replace(uit.key(), QString("<sub>%1</sub>").arg(uit.value()));
            }
        }

        // 10. Replace remaining unicode superscripts
        for (auto uit = uni_sup.constBegin(); uit != uni_sup.constEnd(); ++uit) {
            if (s.contains(uit.key())) {
                s.replace(uit.key(), QString("<sup>%1</sup>").arg(uit.value()));
            }
        }

        // 11. Cleanup adjacent redundant tags
        s.replace("</sub><sub>", "");
        s.replace("</sup><sup>", "");
        s.replace("<sub></sub>", "");
        s.replace("<sup></sup>", "");

        result += s;
    }

    return result;
}

} // namespace TextFormat
