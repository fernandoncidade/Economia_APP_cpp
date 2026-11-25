#ifndef TRIAL_MANAGER_HPP
#define TRIAL_MANAGER_HPP

#include <QString>
#include <QJsonObject>
#include <QWidget>
#include <optional>
#include <cstdint>

class TrialManager {
public:
    static const bool LIBERAR_USO_DEFINITIVO = true;
    static const int DEFAULT_TRIAL_VALUE = 7;
    static const char* const DEFAULT_TRIAL;
    static const char* const PAID_VERSION_URL;

    static QString get_config_path();
    static std::optional<int64_t> get_first_run_timestamp();
    static void set_first_run_timestamp(int64_t timestamp);
    static void delete_first_run_timestamp();

    static QJsonObject get_trial_info();
    static bool is_trial_expired();
    static void enforce_trial(QWidget* parent = nullptr);
};

#endif // TRIAL_MANAGER_HPP
