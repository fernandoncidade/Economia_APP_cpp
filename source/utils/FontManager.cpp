#include "FontManager.hpp"
#include "CaminhoPersistenteUtils.hpp"
#include "LogManager.hpp"
#include <QDir>
#include <QFile>
#include <QJsonDocument>

QJsonObject FontConfig::toJson() const {
    QJsonObject obj;
    obj["family"] = family;
    obj["size"] = size;
    obj["bold"] = bold;
    obj["italic"] = italic;
    obj["underline"] = underline;
    return obj;
}

FontConfig FontConfig::fromJson(const QJsonObject& json) {
    FontConfig cfg;
    if (json.contains("family")) cfg.family = json["family"].toString();
    if (json.contains("size")) cfg.size = json["size"].toInt(10);
    if (json.contains("bold")) cfg.bold = json["bold"].toBool(false);
    if (json.contains("italic")) cfg.italic = json["italic"].toBool(false);
    if (json.contains("underline")) cfg.underline = json["underline"].toBool(false);
    return cfg;
}

QString FontManager::config_file_path() {
    return QDir(obter_caminho_persistente()).filePath("font_config.json");
}

FontConfig FontManager::default_config() {
    return FontConfig();
}

void FontManager::ensure_config_dir() {
    obter_caminho_persistente();
}

FontConfig FontManager::get_config() {
    try {
        ensure_config_dir();
        QString path = config_file_path();
        if (QFile::exists(path)) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                if (doc.isObject()) {
                    return FontConfig::fromJson(doc.object());
                }
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao ler configuração de fontes: %1").arg(e.what()));
    }
    return default_config();
}

bool FontManager::save_config(const FontConfig& config) {
    try {
        ensure_config_dir();
        QString path = config_file_path();
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QJsonDocument doc(config.toJson());
            file.write(doc.toJson(QJsonDocument::Indented));
            LogManager::info(QString("Configuração de fontes salva: family=%1, size=%2").arg(config.family).arg(config.size));
            return true;
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao salvar configuração de fontes: %1").arg(e.what()));
    }
    return false;
}

QFont FontManager::get_font() {
    try {
        FontConfig config = get_config();
        QFont font(config.family, config.size);
        font.setBold(config.bold);
        font.setItalic(config.italic);
        font.setUnderline(config.underline);
        return font;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar QFont: %1").arg(e.what()));
        return QFont("Courier New", 10);
    }
}

QString FontManager::get_html_style() {
    try {
        FontConfig config = get_config();
        QString font_family = config.family;
        int font_size = config.size;
        QString font_weight = config.bold ? "bold" : "normal";
        QString font_style = config.italic ? "italic" : "normal";
        QString text_decoration = config.underline ? "underline" : "none";

        QString css = QString(
            "<style>\n"
            "    body {\n"
            "        font-family: '%1', monospace;\n"
            "        font-size: %2pt;\n"
            "        font-weight: %3;\n"
            "        font-style: %4;\n"
            "        text-decoration: %5;\n"
            "        white-space: pre-wrap;\n"
            "        word-wrap: break-word;\n"
            "        margin: 0;\n"
            "        padding: 4px;\n"
            "    }\n"
            "    pre {\n"
            "        font-family: '%1', monospace;\n"
            "        font-size: %2pt;\n"
            "        font-weight: %3;\n"
            "        font-style: %4;\n"
            "    }\n"
            "</style>\n"
        ).arg(font_family).arg(font_size).arg(font_weight, font_style, text_decoration);
        return css;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar CSS de fontes: %1").arg(e.what()));
        return "<style></style>";
    }
}

bool FontManager::reset_to_default() {
    return save_config(default_config());
}
