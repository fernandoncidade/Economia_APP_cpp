#include "LogManager.hpp"
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <iostream>

void Logger::debug(const QString& message) const { LogManager::debug(message); }
void Logger::info(const QString& message) const { LogManager::info(message); }
void Logger::warning(const QString& message) const { LogManager::warning(message); }
void Logger::error(const QString& message, bool exc_info) const { LogManager::error(message, exc_info); }
void Logger::critical(const QString& message, bool exc_info) const { LogManager::critical(message, exc_info); }

LogManager& LogManager::instance() {
    static LogManager instance;
    return instance;
}

Logger LogManager::get_logger() {
    instance();
    return Logger{};
}

LogManager::LogManager() {
    configure_logging();
}

LogManager::~LogManager() {
    QMutexLocker locker(&m_mutex);
    if (m_file.isOpen()) {
        m_stream.flush();
        m_file.close();
    }
}

void LogManager::configure_logging() {
    if (m_configured) return;
    m_configured = true;

    QString localAppData = qEnvironmentVariable("LOCALAPPDATA");
    if (localAppData.isEmpty()) {
        localAppData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }

    QDir logDir(QDir(localAppData).filePath("Economia_APP/logs"));
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    m_log_file = logDir.filePath(QString("file_economia_%1.log").arg(timestamp));

    m_file.setFileName(m_log_file);
    if (m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_stream.setDevice(&m_file);
    }
}

QString LogManager::get_log_file() {
    LogManager& mgr = instance();
    QMutexLocker locker(&mgr.m_mutex);
    if (!mgr.m_configured) {
        mgr.configure_logging();
    }
    return mgr.m_log_file;
}

void LogManager::log(const QString& message, LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: debug(message); break;
        case LogLevel::INFO: info(message); break;
        case LogLevel::WARNING: warning(message); break;
        case LogLevel::ERR: error(message); break;
        case LogLevel::CRITICAL: critical(message); break;
    }
}

void LogManager::log(const QString& level, const QString& message) {
    LogManager& mgr = instance();
    QMutexLocker locker(&mgr.m_mutex);
    if (!mgr.m_configured) {
        mgr.configure_logging();
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString logLine = QString("%1 [%2] FileEconomia: %3").arg(timeStr, level, message);

    if (mgr.m_file.isOpen()) {
        mgr.m_stream << logLine << "\n";
        mgr.m_stream.flush();
    }

    std::cout << logLine.toStdString() << std::endl;
}

void LogManager::debug(const QString& message) {
    log("DEBUG", message);
}

void LogManager::info(const QString& message) {
    log("INFO", message);
}

void LogManager::warning(const QString& message) {
    log("WARNING", message);
}

void LogManager::error(const QString& message, bool /*exc_info*/) {
    log("ERROR", message);
}

void LogManager::critical(const QString& message, bool /*exc_info*/) {
    log("CRITICAL", message);
}
