#include "TrialManager.hpp"
#include "CaminhoPersistenteUtils.hpp"
#include "IconUtils.hpp"
#include "LogManager.hpp"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QJsonDocument>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

const char* const TrialManager::DEFAULT_TRIAL = "days";
const char* const TrialManager::PAID_VERSION_URL = "https://apps.microsoft.com/detail/9PLR0KD6KSJJ";

static const wchar_t* REG_PATH = L"Software\\Economia_APP";
static const wchar_t* REG_KEY = L"InstallTime";

QString TrialManager::get_config_path() {
    try {
        QString dir = obter_caminho_persistente();
        return QDir(dir).filePath("trial_config.json");
    } catch (...) {
        return QString();
    }
}

std::optional<int64_t> TrialManager::get_first_run_timestamp() {
#ifdef Q_OS_WIN
    try {
        REGSAM views[] = { KEY_READ | KEY_WOW64_64KEY, KEY_READ | KEY_WOW64_32KEY, KEY_READ };
        for (REGSAM access : views) {
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, access, &hKey) == ERROR_SUCCESS) {
                DWORD type = 0;
                DWORD size = 0;
                if (RegQueryValueExW(hKey, REG_KEY, nullptr, &type, nullptr, &size) == ERROR_SUCCESS) {
                    if (type == REG_DWORD && size == sizeof(DWORD)) {
                        DWORD val = 0;
                        if (RegQueryValueExW(hKey, REG_KEY, nullptr, &type, reinterpret_cast<LPBYTE>(&val), &size) == ERROR_SUCCESS) {
                            RegCloseKey(hKey);
                            return static_cast<int64_t>(val);
                        }
                    } else if (type == REG_QWORD && size == sizeof(uint64_t)) {
                        uint64_t val = 0;
                        if (RegQueryValueExW(hKey, REG_KEY, nullptr, &type, reinterpret_cast<LPBYTE>(&val), &size) == ERROR_SUCCESS) {
                            RegCloseKey(hKey);
                            return static_cast<int64_t>(val);
                        }
                    } else if (type == REG_SZ) {
                        std::wstring str(size / sizeof(wchar_t), L'\0');
                        if (RegQueryValueExW(hKey, REG_KEY, nullptr, &type, reinterpret_cast<LPBYTE>(&str[0]), &size) == ERROR_SUCCESS) {
                            RegCloseKey(hKey);
                            try {
                                return std::stoll(str);
                            } catch (...) {}
                        }
                    }
                }
                RegCloseKey(hKey);
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao obter timestamp do registro: %1").arg(e.what()));
    }
#endif
    return std::nullopt;
}

void TrialManager::set_first_run_timestamp(int64_t timestamp) {
#ifdef Q_OS_WIN
    try {
        REGSAM views[] = { KEY_WRITE | KEY_WOW64_64KEY, KEY_WRITE | KEY_WOW64_32KEY, KEY_WRITE };
        for (REGSAM access : views) {
            HKEY hKey;
            if (RegCreateKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, nullptr, 0, access, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
                uint64_t val = static_cast<uint64_t>(timestamp);
                RegSetValueExW(hKey, REG_KEY, 0, REG_QWORD, reinterpret_cast<const BYTE*>(&val), sizeof(val));
                RegCloseKey(hKey);
                return;
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao definir timestamp no registro: %1").arg(e.what()));
    }
#else
    Q_UNUSED(timestamp);
#endif
}

void TrialManager::delete_first_run_timestamp() {
#ifdef Q_OS_WIN
    try {
        REGSAM views[] = { KEY_ALL_ACCESS | KEY_WOW64_64KEY, KEY_ALL_ACCESS | KEY_WOW64_32KEY, KEY_ALL_ACCESS };
        for (REGSAM access : views) {
            HKEY hKey;
            if (RegOpenKeyExW(HKEY_CURRENT_USER, REG_PATH, 0, access, &hKey) == ERROR_SUCCESS) {
                RegDeleteValueW(hKey, REG_KEY);
                RegCloseKey(hKey);
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao remover timestamp do registro: %1").arg(e.what()));
    }
#endif
    try {
        QString path = get_config_path();
        if (!path.isEmpty() && QFile::exists(path)) {
            QFile::remove(path);
        }
    } catch (...) {}
}

QJsonObject TrialManager::get_trial_info() {
    try {
        QString path = get_config_path();
        auto first_run = get_first_run_timestamp();

        if (!path.isEmpty() && QFile::exists(path)) {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                if (doc.isObject()) {
                    QJsonObject info = doc.object();
                    if (first_run.has_value()) {
                        info["first_run"] = static_cast<qint64>(first_run.value());
                    } else {
                        qint64 ts = info.value("first_run").toInteger(QDateTime::currentSecsSinceEpoch());
                        set_first_run_timestamp(ts);
                    }
                    return info;
                }
            }
        }

        int64_t timestamp = first_run.value_or(QDateTime::currentSecsSinceEpoch());
        QJsonObject info;
        info["first_run"] = static_cast<qint64>(timestamp);

        if (!path.isEmpty()) {
            QFile file(path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QJsonDocument doc(info);
                file.write(doc.toJson());
            }
        }

        if (!first_run.has_value()) {
            set_first_run_timestamp(timestamp);
        }

        return info;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro geral em get_trial_info: %1").arg(e.what()));
        QJsonObject info;
        info["first_run"] = static_cast<qint64>(QDateTime::currentSecsSinceEpoch());
        return info;
    }
}

bool TrialManager::is_trial_expired() {
    try {
        QJsonObject info = get_trial_info();
        int64_t first_run = info.value("first_run").toInteger(QDateTime::currentSecsSinceEpoch());
        int64_t now = QDateTime::currentSecsSinceEpoch();
        int64_t trial_seconds = 0;

        QString mode = DEFAULT_TRIAL;
        if (mode == "minutes") {
            trial_seconds = DEFAULT_TRIAL_VALUE * 60;
        } else if (mode == "days") {
            trial_seconds = static_cast<int64_t>(DEFAULT_TRIAL_VALUE) * 24 * 3600;
        }

        return (now - first_run) > trial_seconds;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao verificar expiração do trial: %1").arg(e.what()));
        return false;
    }
}

void TrialManager::enforce_trial(QWidget* parent) {
    try {
        if (LIBERAR_USO_DEFINITIVO) {
            return;
        }

        if (is_trial_expired()) {
            QMessageBox msg(parent);
            QString icon_file = get_icon_path("economia.ico");
            if (!icon_file.isEmpty()) {
                msg.setWindowIcon(QIcon(icon_file));
            }
            msg.setIcon(QMessageBox::Critical);
            msg.setWindowTitle(QCoreApplication::translate("App", "trial_expired_title"));
            msg.setTextFormat(Qt::RichText);
            msg.setText(
                QString("%1<br>%2<br><br>%3<br><br><a href=\"%4\">%5</a>")
                .arg(QCoreApplication::translate("App", "trial_expired_message"),
                     QCoreApplication::translate("App", "trial_buy_message"),
                     QCoreApplication::translate("App", "trial_uninstall_message"),
                     PAID_VERSION_URL,
                     QCoreApplication::translate("App", "trial_paid_link"))
            );
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
            std::exit(0);
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao aplicar restrição do trial: %1").arg(e.what()));
    }
}
