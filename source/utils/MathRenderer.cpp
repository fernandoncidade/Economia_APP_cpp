/**
 * @file MathRenderer.cpp
 * @brief Implementação do renderizador matemático nativo em Qt6 para equações e radicais.
 * Desenha radicais matemáticos como objetos vetoriais puros via QTextObjectInterface e QPainter.
 */

#include "MathRenderer.hpp"
#include "FontManager.hpp"
#include "TextFormat.hpp"

#include <QAbstractTextDocumentLayout>
#include <QGuiApplication>
#include <QPalette>
#include <QPainterPath>
#include <QTextCursor>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>
#include <cmath>

namespace MathRenderer {

RadicalObjectHandler::RadicalObjectHandler(QObject* parent)
    : QObject(parent) {
}

static QChar decode_uni_sub(QChar c) {
    switch (c.unicode()) {
        case 0x2080: return '0';
        case 0x2081: return '1';
        case 0x2082: return '2';
        case 0x2083: return '3';
        case 0x2084: return '4';
        case 0x2085: return '5';
        case 0x2086: return '6';
        case 0x2087: return '7';
        case 0x2088: return '8';
        case 0x2089: return '9';
        case 0x2090: return 'a';
        case 0x1D66: return 'b';
        case 0x1D04: return 'c';
        case 0x1D05: return 'd';
        case 0x2091: return 'e';
        case 0x1D07: return 'f';
        case 0x0262: return 'g';
        case 0x2095: return 'h';
        case 0x1D62: return 'i';
        case 0x2C7C: return 'j';
        case 0x2096: return 'k';
        case 0x2097: return 'l';
        case 0x2098: return 'm';
        case 0x2099: return 'n';
        case 0x2092: return 'o';
        case 0x2093: return 'p';
        case 0x1D69: return 'q';
        case 0x1D63: return 'r';
        case 0x209E: return 's';
        case 0x209F: return 't';
        case 0x1D64: return 'u';
        case 0x1D65: return 'v';
        case 0x1D21: return 'w';
        case 0x028F: return 'y';
        case 0x1D22: return 'z';
        case 0x208A: return '+';
        case 0x208B: return '-';
        case 0x208C: return '=';
        case 0x208D: return '(';
        case 0x208E: return ')';
        default: return QChar();
    }
}

static QChar decode_uni_sup(QChar c) {
    switch (c.unicode()) {
        case 0x2070: return '0';
        case 0x00B9: return '1';
        case 0x00B2: return '2';
        case 0x00B3: return '3';
        case 0x2074: return '4';
        case 0x2075: return '5';
        case 0x2076: return '6';
        case 0x2077: return '7';
        case 0x2078: return '8';
        case 0x2079: return '9';
        case 0x207A: return '+';
        case 0x207B: return '-';
        case 0x207C: return '=';
        case 0x207D: return '(';
        case 0x207E: return ')';
        case 0x1D43: return 'a';
        case 0x1D47: return 'b';
        case 0x1D9C: return 'c';
        case 0x1D48: return 'd';
        case 0x1D49: return 'e';
        case 0x1DA0: return 'f';
        case 0x1D4D: return 'g';
        case 0x02B0: return 'h';
        case 0x2071: return 'i';
        case 0x02B2: return 'j';
        case 0x1D4F: return 'k';
        case 0x02E1: return 'l';
        case 0x1D50: return 'm';
        case 0x207F: return 'n';
        case 0x1D52: return 'o';
        case 0x1D56: return 'p';
        case 0x02B3: return 'r';
        case 0x02E2: return 's';
        case 0x1D57: return 't';
        case 0x1D58: return 'u';
        case 0x1D5B: return 'v';
        case 0x02B7: return 'w';
        case 0x02E3: return 'x';
        case 0x02B8: return 'y';
        case 0x1DBB: return 'z';
        default: return QChar();
    }
}

static QList<RadicalObjectHandler::TextRun> parse_math_text_runs(const QString& raw) {
    if (raw.isEmpty()) return {};

    QString s = raw;
    // Normalize pre-composed financial formulas & indices
    s.replace("iₚₑᵣᵢₒᴅₒ", "i_{período}");
    s.replace("iₚₑᵣᵢₒdₒ", "i_{período}");
    s.replace("i_período", "i_{período}");
    s.replace("i_periodo", "i_{período}");
    s.replace("i_{perioDo}", "i_{período}");
    s.replace("i_perioDo", "i_{período}");
    s.replace("iperioDo", "i_{período}");
    s.replace("iperiodo", "i_{período}");

    s.replace("iₚₑᵣᵢₒᵈ", "i_{period}");
    s.replace("iₚₑᵣᵢₒᴅ", "i_{period}");
    s.replace("i_period", "i_{period}");
    s.replace("iperiod", "i_{period}");

    s.replace("iₐₙᵤₐₗ", "i_{anual}");
    s.replace("i_anual", "i_{anual}");
    s.replace("ianual", "i_{anual}");

    s.replace("iₐₙₙᵤₐₗ", "i_{annual}");
    s.replace("i_annual", "i_{annual}");
    s.replace("iannual", "i_{annual}");

    s.replace("TIRₘ", "TIR_{m}");
    s.replace("TIR_m", "TIR_{m}");
    s.replace("iₑᵩ", "i_{eq}");
    s.replace("iₑq", "i_{eq}");
    s.replace("i_eq", "i_{eq}");

    s.replace(QRegularExpression(R"(<sub>(.*?)</sub>)", QRegularExpression::CaseInsensitiveOption), "_{\\1}");
    s.replace(QRegularExpression(R"(<sup>(.*?)</sup>)", QRegularExpression::CaseInsensitiveOption), "^{\\1}");

    QList<RadicalObjectHandler::TextRun> runs;
    int idx = 0;
    while (idx < s.size()) {
        if (s[idx] == '_' && idx + 1 < s.size()) {
            if (s[idx + 1] == '{') {
                int close = s.indexOf('}', idx + 2);
                if (close != -1) {
                    QString sub_text = s.mid(idx + 2, close - (idx + 2));
                    runs.append({sub_text, RadicalObjectHandler::ScriptType::Subscript});
                    idx = close + 1;
                    continue;
                }
            } else if (s[idx + 1].isLetterOrNumber()) {
                runs.append({QString(s[idx + 1]), RadicalObjectHandler::ScriptType::Subscript});
                idx += 2;
                continue;
            }
        }

        if (s[idx] == '^' && idx + 1 < s.size()) {
            if (s[idx + 1] == '{') {
                int close = s.indexOf('}', idx + 2);
                if (close != -1) {
                    QString sup_text = s.mid(idx + 2, close - (idx + 2));
                    runs.append({sup_text, RadicalObjectHandler::ScriptType::Superscript});
                    idx = close + 1;
                    continue;
                }
            } else if (s[idx + 1] == '(') {
                int close = s.indexOf(')', idx + 2);
                if (close != -1) {
                    QString sup_text = QString("(%1)").arg(s.mid(idx + 2, close - (idx + 2)));
                    runs.append({sup_text, RadicalObjectHandler::ScriptType::Superscript});
                    idx = close + 1;
                    continue;
                }
            } else if (s[idx + 1].isLetterOrNumber()) {
                runs.append({QString(s[idx + 1]), RadicalObjectHandler::ScriptType::Superscript});
                idx += 2;
                continue;
            }
        }

        QChar c_sub = decode_uni_sub(s[idx]);
        if (!c_sub.isNull()) {
            QString sub_acc;
            while (idx < s.size()) {
                QChar cs = decode_uni_sub(s[idx]);
                if (cs.isNull()) break;
                sub_acc.append(cs);
                ++idx;
            }
            runs.append({sub_acc, RadicalObjectHandler::ScriptType::Subscript});
            continue;
        }

        QChar c_sup = decode_uni_sup(s[idx]);
        if (!c_sup.isNull()) {
            QString sup_acc;
            while (idx < s.size()) {
                QChar cs = decode_uni_sup(s[idx]);
                if (cs.isNull()) break;
                sup_acc.append(cs);
                ++idx;
            }
            runs.append({sup_acc, RadicalObjectHandler::ScriptType::Superscript});
            continue;
        }

        QString norm_acc;
        while (idx < s.size()) {
            if (s[idx] == '_' || s[idx] == '^' || !decode_uni_sub(s[idx]).isNull() || !decode_uni_sup(s[idx]).isNull()) {
                break;
            }
            norm_acc.append(s[idx]);
            ++idx;
        }
        if (!norm_acc.isEmpty()) {
            runs.append({norm_acc, RadicalObjectHandler::ScriptType::Normal});
        }
    }

    QList<RadicalObjectHandler::TextRun> merged;
    for (const auto& r : runs) {
        if (!merged.isEmpty() && merged.last().type == r.type) {
            merged.last().text += r.text;
        } else {
            merged.append(r);
        }
    }
    return merged;
}

static double measure_math_text(const QList<RadicalObjectHandler::TextRun>& runs, const QFont& base_font) {
    QFont sub_font = base_font;
    double pt = base_font.pointSizeF() > 0 ? base_font.pointSizeF() : (base_font.pointSize() > 0 ? base_font.pointSize() : 10.0);
    sub_font.setPointSizeF(std::max(6.0, pt * 0.72));

    QFontMetrics fm(base_font);
    QFontMetrics sfm(sub_font);

    double total_w = 0.0;
    for (const auto& run : runs) {
        if (run.type == RadicalObjectHandler::ScriptType::Normal) {
            total_w += fm.horizontalAdvance(run.text);
        } else {
            total_w += sfm.horizontalAdvance(run.text);
        }
    }
    return total_w;
}

static void draw_math_text(QPainter* painter, double x, double baseline_y, const QList<RadicalObjectHandler::TextRun>& runs, const QFont& base_font) {
    QFont sub_font = base_font;
    double pt = base_font.pointSizeF() > 0 ? base_font.pointSizeF() : (base_font.pointSize() > 0 ? base_font.pointSize() : 10.0);
    sub_font.setPointSizeF(std::max(6.0, pt * 0.72));

    QFontMetrics fm(base_font);
    QFontMetrics sfm(sub_font);

    double cur_x = x;
    for (const auto& run : runs) {
        if (run.type == RadicalObjectHandler::ScriptType::Normal) {
            painter->setFont(base_font);
            painter->drawText(QPointF(cur_x, baseline_y), run.text);
            cur_x += fm.horizontalAdvance(run.text);
        } else if (run.type == RadicalObjectHandler::ScriptType::Subscript) {
            painter->setFont(sub_font);
            double sub_y = baseline_y + (fm.ascent() * 0.28);
            painter->drawText(QPointF(cur_x, sub_y), run.text);
            cur_x += sfm.horizontalAdvance(run.text);
        } else if (run.type == RadicalObjectHandler::ScriptType::Superscript) {
            painter->setFont(sub_font);
            double sup_y = baseline_y - (fm.ascent() * 0.35);
            painter->drawText(QPointF(cur_x, sup_y), run.text);
            cur_x += sfm.horizontalAdvance(run.text);
        }
    }
    painter->setFont(base_font);
}

RadicalObjectHandler::LayoutMetrics RadicalObjectHandler::compute_metrics(
    const QTextFormat &format,
    const QFont &base_font)
{
    LayoutMetrics m;

    QFont effectiveFont = base_font;
    if (format.hasProperty(PropFontFamily)) {
        effectiveFont.setFamily(format.property(PropFontFamily).toString());
    }
    if (format.hasProperty(PropFontSize)) {
        double sz = format.property(PropFontSize).toDouble();
        if (sz > 0) effectiveFont.setPointSizeF(sz);
    }

    QFontMetrics fm(effectiveFont);

    QString prefix = format.property(PropPrefix).toString();
    QString suffix = format.property(PropSuffix).toString();
    QString index = format.property(PropIndex).toString().trimmed();
    QString num = format.property(PropNumerator).toString();
    QString den = format.property(PropDenominator).toString();
    QString radicand = format.property(PropRadicand).toString();

    m.is_fraction = format.property(PropIsFraction).toBool() || (!num.isEmpty() && !den.isEmpty());

    double pt = effectiveFont.pointSizeF() > 0 ? effectiveFont.pointSizeF() : (effectiveFont.pointSize() > 0 ? effectiveFont.pointSize() : 10.0);
    
    if (format.hasProperty(PropPenWidth)) {
        m.bar_thickness = format.property(PropPenWidth).toDouble();
    } else {
        m.bar_thickness = std::clamp(pt / 7.5, 1.4, 3.0);
    }

    m.gap = std::clamp(pt * 0.25, 2.0, 5.0);
    m.pad_left = std::clamp(pt * 0.4, 3.5, 8.0);
    m.pad_right = std::clamp(pt * 0.4, 3.5, 8.0);
    m.pad_top = std::clamp(pt * 0.25, 2.0, 5.0);
    m.pad_bottom = std::clamp(pt * 0.25, 2.0, 5.0);

    m.prefix_runs = parse_math_text_runs(prefix);
    m.suffix_runs = parse_math_text_runs(suffix);
    m.prefix_w = measure_math_text(m.prefix_runs, effectiveFont);
    m.suffix_w = measure_math_text(m.suffix_runs, effectiveFont);

    if (!index.isEmpty()) {
        QFont index_font = effectiveFont;
        index_font.setPointSizeF(std::max(6.0, pt * 0.7));
        QFontMetrics ifm(index_font);
        m.index_w = ifm.horizontalAdvance(index);
        m.index_h = ifm.height();
        m.index_reserve_w = m.index_w + 3.0;
    } else {
        m.index_reserve_w = 4.0;
    }

    if (m.is_fraction) {
        m.num_runs = parse_math_text_runs(num);
        m.den_runs = parse_math_text_runs(den);
        m.num_w = measure_math_text(m.num_runs, effectiveFont);
        m.num_h = fm.height();
        m.den_w = measure_math_text(m.den_runs, effectiveFont);
        m.den_h = fm.height();

        double frac_content_w = std::max(m.num_w, m.den_w) + (pt * 1.2);
        m.rad_box_w = frac_content_w + m.pad_left + m.pad_right;

        double frac_content_h = m.num_h + m.gap + m.bar_thickness + m.gap + m.den_h;
        m.rad_box_h = frac_content_h + m.pad_top + m.pad_bottom;
    } else {
        m.rad_runs = parse_math_text_runs(radicand);
        m.rad_w = measure_math_text(m.rad_runs, effectiveFont);
        m.rad_h = fm.height();

        m.rad_box_w = m.rad_w + m.pad_left + m.pad_right + (pt * 0.5);
        m.rad_box_h = m.rad_h + m.pad_top + m.pad_bottom + (pt * 0.2);
    }

    m.radical_hook_w = std::clamp(m.rad_box_h * 0.28, 11.0, 24.0);

    m.total_w = m.prefix_w + m.index_reserve_w + m.radical_hook_w + m.rad_box_w + m.suffix_w + 6.0;
    m.total_h = m.rad_box_h + 6.0;

    return m;
}

QSizeF RadicalObjectHandler::intrinsicSize(
    QTextDocument *doc,
    int posInDocument,
    const QTextFormat &format)
{
    Q_UNUSED(posInDocument);
    QFont f = doc ? doc->defaultFont() : FontManager::get_font();
    LayoutMetrics m = compute_metrics(format, f);
    return QSizeF(m.total_w, m.total_h);
}

void RadicalObjectHandler::drawObject(
    QPainter *painter,
    const QRectF &rect,
    QTextDocument *doc,
    int posInDocument,
    const QTextFormat &format)
{
    Q_UNUSED(posInDocument);
    QFont base_font = doc ? doc->defaultFont() : FontManager::get_font();
    LayoutMetrics m = compute_metrics(format, base_font);

    QFont effectiveFont = base_font;
    if (format.hasProperty(PropFontFamily)) {
        effectiveFont.setFamily(format.property(PropFontFamily).toString());
    }
    if (format.hasProperty(PropFontSize)) {
        double sz = format.property(PropFontSize).toDouble();
        if (sz > 0) effectiveFont.setPointSizeF(sz);
    }

    QString prefix = format.property(PropPrefix).toString();
    QString suffix = format.property(PropSuffix).toString();
    QString index = format.property(PropIndex).toString().trimmed();
    QString num = format.property(PropNumerator).toString();
    QString den = format.property(PropDenominator).toString();
    QString radicand = format.property(PropRadicand).toString();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    // Determine text and stroke color
    QColor textColor;
    if (format.hasProperty(PropColor)) {
        textColor = QColor(format.property(PropColor).toString());
    } else if (painter->device() && painter->device()->devType() == QInternal::Printer) {
        textColor = Qt::black;
    } else {
        textColor = painter->pen().color();
        if (textColor.alpha() == 0 || textColor == Qt::transparent) {
            textColor = qApp ? qApp->palette().color(QPalette::Text) : Qt::black;
        }
    }

    QPen pen(textColor, m.bar_thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->setFont(effectiveFont);
    QFontMetrics fm(effectiveFont);

    double x = rect.left();
    double y_top = rect.top() + 3.0;
    double y_bottom = y_top + m.rad_box_h;
    double center_y = y_top + (m.rad_box_h / 2.0);

    // 1. Draw Prefix (aligned with center baseline)
    if (!prefix.isEmpty()) {
        double prefix_baseline = center_y + (fm.ascent() / 2.0) - (fm.descent() / 2.0);
        draw_math_text(painter, x, prefix_baseline, m.prefix_runs, effectiveFont);
    }
    x += m.prefix_w;

    // 2. Draw Index (degree of the root, e.g. "n", "5")
    double hook_x = x + m.index_reserve_w;
    if (!index.isEmpty()) {
        QFont index_font = effectiveFont;
        double pt = effectiveFont.pointSizeF() > 0 ? effectiveFont.pointSizeF() : (effectiveFont.pointSize() > 0 ? effectiveFont.pointSize() : 10.0);
        index_font.setPointSizeF(std::max(6.0, pt * 0.7));
        painter->setFont(index_font);
        QFontMetrics ifm(index_font);

        double idx_x = hook_x - m.index_w - 1.0;
        double idx_y = y_top + (m.rad_box_h * 0.38) + (ifm.ascent() / 2.0);
        painter->drawText(QPointF(idx_x, idx_y), index);
        painter->setFont(effectiveFont);
    }

    // 3. Draw Radical Symbol Path (Hook + Vinculum Overline)
    QPainterPath radicalPath;
    double p0_x = hook_x - 3.0;
    double p0_y = y_bottom - (m.rad_box_h * 0.45);

    double p1_x = hook_x + (m.radical_hook_w * 0.35);
    double p1_y = y_bottom;

    double p2_x = hook_x + m.radical_hook_w;
    double p2_y = y_top;

    double p3_x = p2_x + m.rad_box_w;
    double p3_y = y_top;

    radicalPath.moveTo(p0_x, p0_y);
    radicalPath.lineTo(p1_x, p1_y);
    radicalPath.lineTo(p2_x, p2_y);
    radicalPath.lineTo(p3_x, p3_y);
    radicalPath.lineTo(p3_x, p3_y + std::min(3.0, m.rad_box_h * 0.08));

    painter->strokePath(radicalPath, pen);

    // 4. Draw Radicand Content (Fraction or Scalar)
    double rad_content_x = p2_x + m.pad_left;
    if (m.is_fraction) {
        double frac_w = m.rad_box_w - m.pad_left - m.pad_right;

        // Numerator
        double num_w = m.num_w;
        double num_x = rad_content_x + (frac_w - num_w) / 2.0;
        double num_baseline = y_top + m.pad_top + fm.ascent();
        draw_math_text(painter, num_x, num_baseline, m.num_runs, effectiveFont);

        // Fraction Bar
        double frac_bar_y = y_top + m.pad_top + m.num_h + m.gap + (m.bar_thickness / 2.0);
        painter->drawLine(QPointF(rad_content_x, frac_bar_y), QPointF(rad_content_x + frac_w, frac_bar_y));

        // Denominator
        double den_w = m.den_w;
        double den_x = rad_content_x + (frac_w - den_w) / 2.0;
        double den_y = frac_bar_y + (m.bar_thickness / 2.0) + m.gap;
        double den_baseline = den_y + fm.ascent();
        draw_math_text(painter, den_x, den_baseline, m.den_runs, effectiveFont);
    } else {
        double rad_baseline = center_y + (fm.ascent() / 2.0) - (fm.descent() / 2.0);
        draw_math_text(painter, rad_content_x, rad_baseline, m.rad_runs, effectiveFont);
    }

    // 5. Draw Suffix (aligned with center baseline)
    if (!suffix.isEmpty()) {
        double suffix_x = p3_x + 4.0;
        double suffix_baseline = center_y + (fm.ascent() / 2.0) - (fm.descent() / 2.0);
        draw_math_text(painter, suffix_x, suffix_baseline, m.suffix_runs, effectiveFont);
    }

    painter->restore();
}

QString encode_radical(const RadicalSpec& spec) {
    QJsonObject obj;
    obj["pfx"] = spec.prefix;
    obj["idx"] = spec.index;
    obj["num"] = spec.numerator;
    obj["den"] = spec.denominator;
    obj["rad"] = spec.radicand;
    obj["suf"] = spec.suffix;
    obj["frac"] = !spec.denominator.isEmpty();
    obj["pen"] = spec.penWidth;

    if (spec.font.has_value()) {
        obj["font_fam"] = spec.font->family();
        obj["font_size"] = spec.font->pointSizeF();
    }
    if (spec.color.has_value()) {
        obj["color"] = spec.color->name();
    }

    QByteArray jsonBytes = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return QString("@@RADICAL:%1@@").arg(QString::fromLatin1(jsonBytes.toBase64()));
}

void register_handler(QTextDocument *doc) {
    if (!doc) return;
    auto* layout = doc->documentLayout();
    if (!layout) return;
    if (!layout->handlerForObject(RadicalFormatType)) {
        layout->registerHandler(RadicalFormatType, new RadicalObjectHandler(doc));
    }
}

void apply_radicals(QTextDocument *doc) {
    if (!doc) return;
    register_handler(doc);

    // 1. Process structured Base64 encoded radical markers
    static const QRegularExpression radicalRegex(R"(@@RADICAL:([A-Za-z0-9+/=]+)@@)");
    QTextCursor cursor(doc);
    while (true) {
        cursor = doc->find(radicalRegex, cursor);
        if (cursor.isNull()) break;

        QString b64 = cursor.selectedText();
        b64 = b64.mid(10, b64.length() - 12);
        QByteArray jsonBytes = QByteArray::fromBase64(b64.toLatin1());
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
        if (!jsonDoc.isObject()) continue;

        QJsonObject obj = jsonDoc.object();
        QTextCharFormat fmt;
        fmt.setObjectType(RadicalFormatType);
        fmt.setVerticalAlignment(QTextCharFormat::AlignMiddle);

        if (obj.contains("pfx")) fmt.setProperty(PropPrefix, obj.value("pfx").toString());
        if (obj.contains("idx")) fmt.setProperty(PropIndex, obj.value("idx").toString());
        if (obj.contains("num")) fmt.setProperty(PropNumerator, obj.value("num").toString());
        if (obj.contains("den")) fmt.setProperty(PropDenominator, obj.value("den").toString());
        if (obj.contains("rad")) fmt.setProperty(PropRadicand, obj.value("rad").toString());
        if (obj.contains("suf")) fmt.setProperty(PropSuffix, obj.value("suf").toString());
        if (obj.contains("frac")) fmt.setProperty(PropIsFraction, obj.value("frac").toBool());
        if (obj.contains("font_fam")) fmt.setProperty(PropFontFamily, obj.value("font_fam").toString());
        if (obj.contains("font_size")) fmt.setProperty(PropFontSize, obj.value("font_size").toDouble());
        if (obj.contains("color")) fmt.setProperty(PropColor, obj.value("color").toString());
        if (obj.contains("pen")) fmt.setProperty(PropPenWidth, obj.value("pen").toDouble());

        cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
    }

    // 2. Process LaTeX style \sqrt[n]{x} or \sqrt{x} if present
    static const QRegularExpression latexRegex(R"(\\sqrt(?:\[([^\]]*)\])?\{([^}]+)\})");
    cursor = QTextCursor(doc);
    while (true) {
        cursor = doc->find(latexRegex, cursor);
        if (cursor.isNull()) break;

        QRegularExpressionMatch match = latexRegex.match(cursor.selectedText());
        if (!match.hasMatch()) continue;

        QString idx = match.captured(1).trimmed();
        QString body = match.captured(2).trimmed();

        QTextCharFormat fmt;
        fmt.setObjectType(RadicalFormatType);
        fmt.setVerticalAlignment(QTextCharFormat::AlignMiddle);
        fmt.setProperty(PropIndex, idx);

        static const QRegularExpression fracRegex(R"(\\frac\{([^}]+)\}\{([^}]+)\})");
        QRegularExpressionMatch fracMatch = fracRegex.match(body);
        if (fracMatch.hasMatch()) {
            fmt.setProperty(PropIsFraction, true);
            fmt.setProperty(PropNumerator, fracMatch.captured(1).trimmed());
            fmt.setProperty(PropDenominator, fracMatch.captured(2).trimmed());
        } else if (body.contains('/')) {
            int slash = body.indexOf('/');
            fmt.setProperty(PropIsFraction, true);
            fmt.setProperty(PropNumerator, body.left(slash).trimmed());
            fmt.setProperty(PropDenominator, body.mid(slash + 1).trimmed());
        } else {
            fmt.setProperty(PropIsFraction, false);
            fmt.setProperty(PropRadicand, body);
        }

        cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
    }

    // 3. Process Unicode radical expressions: e.g. ⁿ√(a / b) or √5 or √(1 + 4)
    static const QRegularExpression uniRadRegex(
        R"((?:([⁰¹²³⁴⁵⁶⁷⁸⁹ⁿA-Za-z0-9]+)\s*)?√\s*(?:\(([^)]+)\)|([0-9A-Za-z.,]+)))"
    );
    cursor = QTextCursor(doc);
    while (true) {
        cursor = doc->find(uniRadRegex, cursor);
        if (cursor.isNull()) break;

        QRegularExpressionMatch match = uniRadRegex.match(cursor.selectedText());
        if (!match.hasMatch()) continue;

        QString rawIdx = match.captured(1);
        QString idx = TextFormat::from_unicode_superscripts(rawIdx).trimmed();
        QString body = match.captured(2).isEmpty() ? match.captured(3) : match.captured(2);
        body = body.trimmed();

        QTextCharFormat fmt;
        fmt.setObjectType(RadicalFormatType);
        fmt.setVerticalAlignment(QTextCharFormat::AlignMiddle);
        fmt.setProperty(PropIndex, idx);

        if (body.contains('/')) {
            int slash = body.indexOf('/');
            fmt.setProperty(PropIsFraction, true);
            fmt.setProperty(PropNumerator, body.left(slash).trimmed());
            fmt.setProperty(PropDenominator, body.mid(slash + 1).trimmed());
        } else {
            fmt.setProperty(PropIsFraction, false);
            fmt.setProperty(PropRadicand, body);
        }

        cursor.insertText(QString(QChar::ObjectReplacementCharacter), fmt);
    }
}

QString to_plain_text(const QString &text) {
    static const QRegularExpression radicalRegex(R"(@@RADICAL:([A-Za-z0-9+/=]+)@@)");
    QString result = text;
    QRegularExpressionMatchIterator it = radicalRegex.globalMatch(text);
    QList<QRegularExpressionMatch> matches;
    while (it.hasNext()) {
        matches.append(it.next());
    }

    for (int i = matches.size() - 1; i >= 0; --i) {
        const auto& match = matches[i];
        QByteArray jsonBytes = QByteArray::fromBase64(match.captured(1).toLatin1());
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
        if (!jsonDoc.isObject()) continue;

        QJsonObject obj = jsonDoc.object();
        QString pfx = obj.value("pfx").toString();
        QString idx = obj.value("idx").toString().trimmed();
        QString suf = obj.value("suf").toString();
        bool is_frac = obj.value("frac").toBool();

        QString rad_str;
        if (is_frac) {
            rad_str = QString("(%1 / %2)").arg(obj.value("num").toString(), obj.value("den").toString());
        } else {
            rad_str = obj.value("rad").toString();
        }

        QString root_sym = idx.isEmpty() ? QString(QChar(0x221A)) : (TextFormat::to_superscript(idx) + QChar(0x221A));
        QString plain = pfx + root_sym + rad_str + suf;
        result.replace(match.capturedStart(), match.capturedLength(), plain);
    }
    return result;
}

QString render_radical_fraction_html(
    const QString& index,
    const QString& numerator,
    const QString& denominator,
    const QString& prefix,
    const QString& suffix,
    const std::optional<QFont>& font,
    const std::optional<QColor>& color
) {
    RadicalSpec spec;
    spec.index = index;
    spec.numerator = numerator;
    spec.denominator = denominator;
    spec.prefix = prefix;
    spec.suffix = suffix;
    spec.font = font;
    spec.color = color;
    return encode_radical(spec);
}

QString render_radical_single_html(
    const QString& index,
    const QString& radicand,
    const QString& prefix,
    const QString& suffix,
    const std::optional<QFont>& font,
    const std::optional<QColor>& color
) {
    RadicalSpec spec;
    spec.index = index;
    spec.radicand = radicand;
    spec.prefix = prefix;
    spec.suffix = suffix;
    spec.font = font;
    spec.color = color;
    return encode_radical(spec);
}

QString render_radical_html(const RadicalSpec& spec) {
    return encode_radical(spec);
}

RenderResult render_radical(const RadicalSpec& spec) {
    QString token = encode_radical(spec);
    return {QImage(), 0, 0, token};
}

} // namespace MathRenderer
