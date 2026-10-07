#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <QString>
#include <QFont>
#include <QJsonObject>

struct FontConfig {
    QString family = "Courier New";
    int size = 10;
    bool bold = false;
    bool italic = false;
    bool underline = false;

    QJsonObject toJson() const;
    static FontConfig fromJson(const QJsonObject& json);
};

class FontManager {
public:
    static QString config_file_path();
    static FontConfig default_config();

    static FontConfig get_config();
    static bool save_config(const FontConfig& config);
    static QFont get_font();
    static QString get_html_style();
    static bool reset_to_default();

private:
    static void ensure_config_dir();
};

#endif // FONT_MANAGER_HPP
