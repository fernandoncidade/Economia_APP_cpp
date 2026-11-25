#include "IconUtils.hpp"
#include "ApplicationPathUtils.hpp"
#include "LogManager.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

QString get_icon_path(const QString& icon_name) {
    try {
        QString base_path = get_app_base_path();
        QStringList possible_paths;

        auto add_paths = [&](const QString& root) {
            possible_paths << QDir(root).filePath("assets/icones/" + icon_name);
            possible_paths << QDir(root).filePath("source/assets/icones/" + icon_name);
            possible_paths << QDir(root).filePath("icones/" + icon_name);
            possible_paths << ":/assets/icones/" + icon_name;
        };

        add_paths(base_path);
        add_paths(QDir(base_path).filePath(".."));
        add_paths(QDir(base_path).filePath("../.."));
        add_paths(QDir(base_path).filePath("../../.."));
        add_paths("C:/Users/ferna/APLICATIVOS/CPP/Economia_APP_cpp");

        for (const QString& path : possible_paths) {
            if (path.startsWith(":/") && QFile::exists(path)) {
                return path;
            }
            QFileInfo fi(path);
            if (fi.exists() && fi.isFile()) {
                return fi.canonicalFilePath();
            }
        }

        if (!possible_paths.isEmpty()) {
            return QFileInfo(possible_paths.first()).absoluteFilePath();
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao obter caminho do ícone '%1': %2").arg(icon_name, e.what()));
    }
    return QString();
}
