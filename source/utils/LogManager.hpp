#ifndef LOG_MANAGER_HPP
#define LOG_MANAGER_HPP

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class Logger {
public:
    void debug(const QString& message) const;
    void info(const QString& message) const;
    void warning(const QString& message) const;
    void error(const QString& message, bool exc_info = false) const;
    void critical(const QString& message, bool exc_info = true) const;
};

class LogManager {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERR,
        CRITICAL
    };

    static LogManager& instance();
    static Logger get_logger();
    static QString get_log_file();

    void log(const QString& message, LogLevel level = LogLevel::INFO);
    static void log(const QString& level, const QString& message);

    static void debug(const QString& message);
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message, bool exc_info = false);
    static void critical(const QString& message, bool exc_info = true);

private:
    LogManager();
    ~LogManager();

    void configure_logging();

    QString m_log_file;
    QFile m_file;
    QTextStream m_stream;
    QMutex m_mutex;
    bool m_configured = false;
};

#endif // LOG_MANAGER_HPP
