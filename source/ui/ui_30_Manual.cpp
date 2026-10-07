#include "ui_30_Manual.hpp"
#include "ui_31_manual_pt_BR.hpp"
#include "ui_32_manual_en_US.hpp"

namespace source::ui::Manual {
namespace {

QString manual_intro_text(const QString &normalized_lang)
{
    return normalized_lang == QStringLiteral("en_US")
               ? manual_en_US::manual_intro_text()
               : manual_pt_BR::manual_intro_text();
}

QString manual_toc_title(const QString &normalized_lang)
{
    return normalized_lang == QStringLiteral("en_US")
               ? manual_en_US::manual_toc_title()
               : manual_pt_BR::manual_toc_title();
}

} // namespace

QString normalize_language(const QString &lang)
{
    if (lang.trimmed().isEmpty()) {
        return QStringLiteral("pt_BR");
    }

    const QString value = lang.trimmed().replace(QLatin1Char('-'), QLatin1Char('_')).toLower();
    if (value == QStringLiteral("pt") || value == QStringLiteral("pt_br")) {
        return QStringLiteral("pt_BR");
    }
    if (value == QStringLiteral("en") || value == QStringLiteral("en_us")) {
        return QStringLiteral("en_US");
    }
    return QStringLiteral("pt_BR");
}

QString get_manual_title(const QString &lang)
{
    const QString normalized = normalize_language(lang);
    if (normalized == QStringLiteral("en_US")) {
        return QStringLiteral("User Manual — Economia_APP");
    }
    return QStringLiteral("Manual de Utilização — Economia_APP");
}

QString to_unicode_bold(const QString &text)
{
    QString output;
    output.reserve(text.size());
    for (QChar ch : text) {
        const char32_t u = ch.unicode();
        if (u >= 'A' && u <= 'Z') {
            const char32_t codepoint = 0x1D400 + (u - 'A');
            output.append(QString::fromUcs4(&codepoint, 1));
        } else if (u >= 'a' && u <= 'z') {
            const char32_t codepoint = 0x1D41A + (u - 'a');
            output.append(QString::fromUcs4(&codepoint, 1));
        } else if (u >= '0' && u <= '9') {
            const char32_t codepoint = 0x1D7CE + (u - '0');
            output.append(QString::fromUcs4(&codepoint, 1));
        } else {
            output.append(ch);
        }
    }
    return output;
}

QList<ManualSection> get_manual_document(const QString &lang)
{
    const QString normalized = normalize_language(lang);
    if (normalized == QStringLiteral("en_US")) {
        return manual_en_US::get_manual_document();
    }
    return manual_pt_BR::get_manual_document();
}

QPair<QList<ManualBlock>, QStringList> get_manual_blocks(const QString &lang)
{
    const QString normalized = normalize_language(lang);
    const QList<ManualSection> sections = get_manual_document(normalized);
    QList<ManualBlock> blocks;
    QStringList order;

    const QString title = get_manual_title(normalized);
    blocks.append({QStringLiteral("line"), title, QString()});
    blocks.append({QStringLiteral("line"), QString(title.size(), QLatin1Char('=')), QString()});
    blocks.append({QStringLiteral("blank"), QString(), QString()});

    blocks.append({QStringLiteral("paragraph"), manual_intro_text(normalized), QString()});
    blocks.append({QStringLiteral("blank"), QString(), QString()});
    blocks.append({QStringLiteral("toc_title"), manual_toc_title(normalized), QString()});

    for (int i = 0; i < sections.size(); ++i) {
        blocks.append({QStringLiteral("toc_item"),
                       sections.at(i).title,
                       sections.at(i).id});
    }

    blocks.append({QStringLiteral("blank"), QString(), QString()});
    blocks.append({QStringLiteral("divider"), QString(60, QLatin1Char('-')), QString()});
    blocks.append({QStringLiteral("blank"), QString(), QString()});

    for (const ManualSection &section : sections) {
        order << section.id;
        blocks.append({QStringLiteral("section_title"), section.title, section.id});
        blocks.append({QStringLiteral("blank"), QString(), QString()});

        for (const QString &paragraph : section.paragraphs) {
            blocks.append({QStringLiteral("paragraph"), paragraph, QString()});
            blocks.append({QStringLiteral("blank"), QString(), QString()});
        }

        for (const QString &bullet : section.bullets) {
            blocks.append({QStringLiteral("bullet"), bullet, QString()});
        }
        if (!section.bullets.isEmpty()) {
            blocks.append({QStringLiteral("blank"), QString(), QString()});
        }

        for (const ManualDetails &detail : section.details) {
            blocks.append({QStringLiteral("detail_title"), detail.summary, QString()});
            blocks.append({QStringLiteral("blank"), QString(), QString()});

            for (const QString &paragraph : detail.paragraphs) {
                blocks.append({QStringLiteral("paragraph"), paragraph, QString()});
                blocks.append({QStringLiteral("blank"), QString(), QString()});
            }

            for (const QString &bullet : detail.bullets) {
                blocks.append({QStringLiteral("bullet"), bullet, QString()});
            }
            if (!detail.bullets.isEmpty()) {
                blocks.append({QStringLiteral("blank"), QString(), QString()});
            }
        }

        blocks.append({QStringLiteral("divider"), QString(60, QLatin1Char('-')), QString()});
        blocks.append({QStringLiteral("blank"), QString(), QString()});
    }

    return {blocks, order};
}

QString get_manual_text(const QString &lang)
{
    return get_manual_text_with_positions(lang).text;
}

ManualTextWithPositions get_manual_text_with_positions(const QString &lang)
{
    const QString normalized = normalize_language(lang);
    const QList<ManualSection> sections = get_manual_document(normalized);

    QStringList lines;
    QMap<QString, int> positions;
    QStringList order;

    auto add_line = [&lines](const QString &line = QString()) { lines << line; };
    auto current_offset = [&lines]() {
        int total = 0;
        for (const QString &line : lines) {
            total += line.size() + 1;
        }
        return total;
    };

    const QString title = get_manual_title(normalized);
    add_line(title);
    add_line(QString(title.size(), QLatin1Char('=')));
    add_line();
    add_line(manual_intro_text(normalized));
    add_line();
    add_line(manual_toc_title(normalized));
    add_line(QStringLiteral("----------"));
    for (int i = 0; i < sections.size(); ++i) {
        add_line(sections.at(i).title);
    }
    add_line();
    add_line(QString(60, QLatin1Char('-')));
    add_line();

    for (const ManualSection &section : sections) {
        positions.insert(section.id, current_offset());
        order << section.id;
        add_line(section.title);
        add_line(QString(section.title.size(), QLatin1Char('-')));
        add_line();

        for (const QString &paragraph : section.paragraphs) {
            add_line(paragraph);
            add_line();
        }

        for (const QString &bullet : section.bullets) {
            add_line(QStringLiteral("- %1").arg(bullet));
        }
        if (!section.bullets.isEmpty()) {
            add_line();
        }

        for (const ManualDetails &detail : section.details) {
            add_line(detail.summary);
            add_line();
            for (const QString &paragraph : detail.paragraphs) {
                add_line(paragraph);
                add_line();
            }
            for (const QString &bullet : detail.bullets) {
                add_line(QStringLiteral("  * %1").arg(bullet));
            }
            if (!detail.bullets.isEmpty()) {
                add_line();
            }
        }

        add_line(QString(60, QLatin1Char('-')));
        add_line();
    }

    return {lines.join(QLatin1Char('\n')), positions, order};
}

} // namespace source::ui::Manual
