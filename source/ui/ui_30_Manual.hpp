#ifndef UI_30_MANUAL_HPP
#define UI_30_MANUAL_HPP

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QPair>

namespace source::ui::Manual {

struct ManualDetails {
    QString summary;
    QStringList paragraphs;
    QStringList bullets;
};

struct ManualSection {
    QString id;
    QString title;
    QStringList paragraphs;
    QStringList bullets;
    QList<ManualDetails> details;
};

struct ManualBlock {
    QString kind;
    QString text;
    QString section_id;
};

struct ManualTextWithPositions {
    QString text;
    QMap<QString, int> positions;
    QStringList order;
};

QString normalize_language(const QString &lang = QString());
QString get_manual_title(const QString &lang = QString());
QString to_unicode_bold(const QString &text);

QList<ManualSection> get_manual_document(const QString &lang = QString());
QPair<QList<ManualBlock>, QStringList> get_manual_blocks(const QString &lang = QString());
QString get_manual_text(const QString &lang = QString());
ManualTextWithPositions get_manual_text_with_positions(const QString &lang = QString());

} // namespace source::ui::Manual

#endif // UI_30_MANUAL_HPP
