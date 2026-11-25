#include "tr_02_compileTranslations.hpp"
#include "../utils/ApplicationPathUtils.hpp"
#include "../utils/LogManager.hpp"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <iostream>

namespace TranslationsCompiler {

bool compilar_traducoes() {
    QString base_path = get_app_base_path();
    QStringList candidateDirs = {
        QDir(base_path).filePath("source/language/translations"),
        QDir(base_path).filePath("language/translations"),
        "C:/Users/ferna/APLICATIVOS/CPP/Economia_APP_cpp/source/language/translations"
    };

    QString translationsDir;
    for (const QString& d : candidateDirs) {
        if (QDir(d).exists()) {
            translationsDir = d;
            break;
        }
    }

    if (translationsDir.isEmpty()) {
        LogManager::error("Diretório de traduções não encontrado para compilação.");
        return false;
    }

    QDir dir(translationsDir);
    QStringList tsFiles = dir.entryList(QStringList() << "*.ts", QDir::Files);
    bool allSuccess = true;

    for (const QString& tsFileName : tsFiles) {
        QString tsPath = dir.filePath(tsFileName);
        QString qmPath = dir.filePath(QFileInfo(tsFileName).completeBaseName() + ".qm");

        std::cout << "Compilando: " << tsFileName.toStdString() << std::endl;
        int exitCode = QProcess::execute("lrelease", QStringList() << tsPath << "-qm" << qmPath);
        if (exitCode != 0) {
            // Try with C:/Qt/6.11.1/mingw_64/bin/lrelease.exe or pyside6-lrelease
            exitCode = QProcess::execute("C:/Qt/6.11.1/mingw_64/bin/lrelease.exe", QStringList() << tsPath << "-qm" << qmPath);
        }

        if (exitCode == 0) {
            std::cout << "Sucesso: " << qmPath.toStdString() << std::endl;
        } else {
            std::cout << "Erro ao compilar " << tsFileName.toStdString() << " (código: " << exitCode << ")" << std::endl;
            allSuccess = false;
        }
    }

    return allSuccess;
}

} // namespace TranslationsCompiler
