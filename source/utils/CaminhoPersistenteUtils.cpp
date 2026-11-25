#include "CaminhoPersistenteUtils.hpp"
#include "LogManager.hpp"
#include <QDir>
#include <QStandardPaths>

QString obter_caminho_persistente() {
    QString localAppData = qEnvironmentVariable("LOCALAPPDATA");
    if (localAppData.isEmpty()) {
        localAppData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    
    QDir dir(QDir(localAppData).filePath("Economia_APP"));
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            LogManager::error("Erro ao criar diretório de configuração: " + dir.absolutePath());
        }
    }
    return dir.absolutePath();
}
