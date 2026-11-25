#include "ApplicationPathUtils.hpp"
#include "LogManager.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QStringConverter>

QString get_app_base_path() {
    QString appDir = QCoreApplication::applicationDirPath();
    return appDir;
}

QString get_project_root() {
    QString base = get_app_base_path();
    QDir d(base);
    if (d.exists("assets") || d.exists("source/assets") || d.exists("CMakeLists.txt")) {
        return d.absolutePath();
    }
    if (d.cdUp() && (d.exists("assets") || d.exists("source/assets") || d.exists("CMakeLists.txt"))) {
        return d.absolutePath();
    }
    if (d.cdUp() && (d.exists("assets") || d.exists("source/assets") || d.exists("CMakeLists.txt"))) {
        return d.absolutePath();
    }
    return base;
}

QString get_text_file_path(const QString& filename, const QString& folder) {
    try {
        QString base_path = get_app_base_path();
        QStringList possible_paths;

        auto add_paths = [&](const QString& root) {
            if (!folder.isEmpty()) {
                possible_paths << QDir(root).filePath("assets/" + folder + "/" + filename);
                possible_paths << QDir(root).filePath("source/assets/" + folder + "/" + filename);
                possible_paths << QDir(root).filePath(folder + "/" + filename);
                possible_paths << ":/assets/" + folder + "/" + filename;
            } else {
                possible_paths << QDir(root).filePath("assets/" + filename);
                possible_paths << QDir(root).filePath("source/assets/" + filename);
                possible_paths << QDir(root).filePath(filename);
                possible_paths << ":/assets/" + filename;
            }
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
        LogManager::error(QString("Erro ao obter caminho do arquivo de texto '%1': %2").arg(filename, e.what()));
    }
    return QString();
}

QString load_text_file(const QString& filename, const QString& folder, const QString& encoding) {
    Q_UNUSED(encoding);
    try {
        QString file_path = get_text_file_path(filename, folder);
        if (!file_path.isEmpty() && QFile::exists(file_path)) {
            QFile file(file_path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                return QString::fromUtf8(file.readAll());
            }
        }
        LogManager::error(QString("Arquivo de texto '%1' não encontrado.").arg(filename));
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao carregar arquivo de texto '%1': %2").arg(filename, e.what()));
    }
    return QString();
}
