#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QTextStream>

namespace mocks::translations {

struct TranslationTarget {
    QString id;
    QString display_name;
    QStringList file_patterns;
    QString translations_relative_path;
};

class TranslationCompilerTerminal final {
public:
    TranslationCompilerTerminal();
    int run(const QStringList &arguments);

    struct CliOptions {
        QString target_id = QStringLiteral("all");
        QString custom_source_dir;
        QString custom_output_dir;
        bool show_help = false;
        bool list_targets = false;
    };

    void print_help(QTextStream &out) const;
    void print_targets(QTextStream &out) const;
    CliOptions parse_arguments(const QStringList &arguments, QTextStream &out, int &exit_code) const;
    TranslationTarget resolve_target(const QString &id) const;
    int compile_target(const TranslationTarget &target, const QString &source_dir_path,
                       const QString &output_dir_path, QTextStream &out) const;

    QString locate_project_root() const;
    QString locate_lrelease_executable() const;

private:
    QVector<TranslationTarget> targets_;
    QString project_root_;
    QString lrelease_executable_;
};

}  // namespace mocks::translations
