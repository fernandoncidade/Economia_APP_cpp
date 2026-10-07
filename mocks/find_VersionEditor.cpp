#include "find_VersionEditor.hpp"

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QPair>
#include <QPushButton>
#include <QRegularExpression>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtGlobal>

#include <optional>

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace mocks::version_editor {
namespace {

const QStringList kPtMonths = {
    QStringLiteral("Janeiro"),
    QStringLiteral("Fevereiro"),
    QStringLiteral("Março"),
    QStringLiteral("Abril"),
    QStringLiteral("Maio"),
    QStringLiteral("Junho"),
    QStringLiteral("Julho"),
    QStringLiteral("Agosto"),
    QStringLiteral("Setembro"),
    QStringLiteral("Outubro"),
    QStringLiteral("Novembro"),
    QStringLiteral("Dezembro"),
};

const QStringList kPtMonthsLower = {
    QStringLiteral("janeiro"),
    QStringLiteral("fevereiro"),
    QStringLiteral("março"),
    QStringLiteral("abril"),
    QStringLiteral("maio"),
    QStringLiteral("junho"),
    QStringLiteral("julho"),
    QStringLiteral("agosto"),
    QStringLiteral("setembro"),
    QStringLiteral("outubro"),
    QStringLiteral("novembro"),
    QStringLiteral("dezembro"),
};

const QStringList kEnMonths = {
    QStringLiteral("January"),
    QStringLiteral("February"),
    QStringLiteral("March"),
    QStringLiteral("April"),
    QStringLiteral("May"),
    QStringLiteral("June"),
    QStringLiteral("July"),
    QStringLiteral("August"),
    QStringLiteral("September"),
    QStringLiteral("October"),
    QStringLiteral("November"),
    QStringLiteral("December"),
};

const QString &group_about()
{
    static const QString value = QStringLiteral("Sobre");
    return value;
}

const QString &group_clc()
{
    static const QString value = QStringLiteral("CLC");
    return value;
}

const QString &group_copyright()
{
    static const QString value = QStringLiteral("Direitos Autorais");
    return value;
}

const QString &group_eula()
{
    static const QString value = QStringLiteral("EULA");
    return value;
}

const QString &group_notices()
{
    static const QString value = QStringLiteral("Notices");
    return value;
}

const QString &group_privacy()
{
    static const QString value = QStringLiteral("Política de Privacidade");
    return value;
}

const QString &group_misc()
{
    static const QString value = QStringLiteral("Miscelânia");
    return value;
}

const QStringList &group_order()
{
    static const QStringList order = {
        group_about(),
        group_clc(),
        group_copyright(),
        group_eula(),
        group_notices(),
        group_privacy(),
        group_misc(),
    };
    return order;
}

QString build_version_from_date(const QDate &date = QDate::currentDate())
{
    const QDate safe_date = date.isValid() ? date : QDate::currentDate();
    if (!safe_date.isValid()) {
        return QStringLiteral("2026.9.29.0");
    }

    return QStringLiteral("%1.%2.%3.0")
        .arg(safe_date.year())
        .arg(safe_date.month())
        .arg(safe_date.day());
}

QString readme_version_token(const QString &version)
{
    const QString trimmed = version.trimmed();
    if (trimmed.isEmpty()) {
        return QStringLiteral("v0.0.0.0");
    }

    return trimmed.startsWith(QLatin1Char('v'), Qt::CaseInsensitive) ? trimmed : QStringLiteral("v%1").arg(trimmed);
}

QString replace_readme_version_token(const QString &line, const QString &version)
{
    const QRegularExpression rx(QStringLiteral(R"(^(.*\*\*)v[\d\.]+(\*\*.*)$)"));
    const QRegularExpressionMatch match = rx.match(line);
    if (!match.hasMatch()) {
        return line;
    }

    return QStringLiteral("%1%2%3").arg(match.captured(1), version, match.captured(2));
}

QString spec_content_key(const FileSpec &spec)
{
    return spec.content_key.isEmpty() ? spec.key : spec.content_key;
}

const QMap<QString, QStringList> &patterns()
{
    static const QMap<QString, QStringList> data = {
        {QStringLiteral("about_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*.*)$)")}},
        {QStringLiteral("about_en"), {QStringLiteral(R"(^(Version:\s*.*)$)")}},
        {QStringLiteral("history_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*.*)$)")}},
        {QStringLiteral("history_en"), {QStringLiteral(R"(^(Version:\s*.*)$)")}},
        {QStringLiteral("clc_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*.*)$)"), QStringLiteral(R"(^(Data:\s*.*)$)")}},
        {QStringLiteral("clc_en"), {QStringLiteral(R"(^(Version:\s*.*)$)"), QStringLiteral(R"(^(Date:\s*.*)$)")}},
        {QStringLiteral("copyright_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o.*)$)"), QStringLiteral(R"(^(?:Data:\s*.*|Versão\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("copyright_en"), {QStringLiteral(R"(^(Version.*)$)"), QStringLiteral(R"(^(?:Date:\s*.*|Version\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("eula_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o.*)$)"), QStringLiteral(R"(^(?:Data:\s*.*|Versão\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("eula_en"), {QStringLiteral(R"(^(Version.*)$)"), QStringLiteral(R"(^(?:Date:\s*.*|Version\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("notice_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o.*)$)"), QStringLiteral(R"(^(?:Data:\s*.*|Versão\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("notice_en"), {QStringLiteral(R"(^(Version.*)$)"), QStringLiteral(R"(^(?:Date:\s*.*|Version\s+[\d\.]+,?\s+.*)$)")}},
        {QStringLiteral("privacy_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o:\s*.*)$)"), QStringLiteral(R"(^(Última atualização:\s*.*)$)")}},
        {QStringLiteral("privacy_en"), {QStringLiteral(R"(^(Version:\s*.*)$)"), QStringLiteral(R"(^(Last updated:\s*.*)$)")}},
        {QStringLiteral("about_dialog"), {QStringLiteral(R"(^(\s*"<p><b>%1:</b>\s*[\d\.]+</p>".*)$)")}},
        {QStringLiteral("readme_pt"),
         {QStringLiteral(R"(^(> \*\*Observa(?:ção|cao):\*\*.*(?:Economia_APP|economia_app).*)$)"),
          QStringLiteral(R"(^(Vers(?:ã|a)o:\s*v[\d\.]+<br>)\s*$)"),
          QStringLiteral(R"(^(Data t(?:é|e)cnica desta revis(?:ã|a)o:\s*.*<br>)\s*$)")}},
        {QStringLiteral("readme_en"),
         {QStringLiteral(R"(^(> \*\*Note:\*\*.*(?:Economia_APP|economia_app).*)$)"),
          QStringLiteral(R"(^(Version:\s*v[\d\.]+<br>)\s*$)"),
          QStringLiteral(R"(^(Technical revision date:\s*.*<br>)\s*$)")}},
        {QStringLiteral("manual_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o documentada:\s*`.*`)\s*$)"), QStringLiteral(R"(^(Data:\s*`.*`)\s*$)")}},
        {QStringLiteral("observacao_pt"), {QStringLiteral(R"(^(Vers(?:ã|a)o documentada:\s*`.*`)\s*$)"), QStringLiteral(R"(^(Data do snapshot t(?:é|e)cnico:\s*`.*`)\s*$)")}},
        {QStringLiteral("installer_unified"), {QStringLiteral(R"(^(#define\s+MyAppVersion\s+"[\d\.]+";?)\r?$)")}},
        {QStringLiteral("installer_mingw"),
         {QStringLiteral(R"(^(#define\s+MyAppVersion\s+"[\d\.]+";?)\r?$)"),
          QStringLiteral(R"(^(OutputBaseFilename=Economia_APP_mingw_v[\d\.]+)\r?$)")}},
        {QStringLiteral("installer_msvc"),
         {QStringLiteral(R"(^(#define\s+MyAppVersion\s+"[\d\.]+";?)\r?$)"),
          QStringLiteral(R"(^(\s*OutputBaseFilename=Economia_APP_msvc_v[\d\.]+)\r?$)")}},
    };

    return data;
}

QString resolve_file_path(const QString &base_dir, const QString &relative_path)
{
    const QDir dir(base_dir);
    const QString primary = QFileInfo(dir.filePath(relative_path)).absoluteFilePath();
    if (QFileInfo::exists(primary)) {
        return primary;
    }

    const QStringList rel_parts = QDir::fromNativeSeparators(relative_path).split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if (!rel_parts.isEmpty() && rel_parts.first() == QStringLiteral("source")) {
        QString stripped;
        for (int i = 1; i < rel_parts.size(); ++i) {
            if (!stripped.isEmpty()) {
                stripped += QLatin1Char('/');
            }
            stripped += rel_parts.at(i);
        }

        const QString alternate = QFileInfo(dir.filePath(stripped)).absoluteFilePath();
        if (QFileInfo::exists(alternate)) {
            return alternate;
        }
    }

    return primary;
}

std::optional<QString> read_file_text(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }

    QByteArray data = file.readAll();
    if (data.startsWith("\xEF\xBB\xBF")) {
        data = data.mid(3);
    }

    QString utf8 = QString::fromUtf8(data);
    if (!utf8.contains(QChar::ReplacementCharacter)) {
        return utf8;
    }

    QString latin1 = QString::fromLatin1(data);
    return latin1;
}

bool write_file_text(const QString &path, const QString &text, QString *error_message)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error_message != nullptr) {
            *error_message = file.errorString();
        }
        return false;
    }

    const qint64 written = file.write(text.toUtf8());
    if (written < 0) {
        if (error_message != nullptr) {
            *error_message = file.errorString();
        }
        return false;
    }

    return true;
}

QString detect_line_ending(const QString &text)
{
    return text.contains(QStringLiteral("\r\n")) ? QStringLiteral("\r\n") : QStringLiteral("\n");
}

QStringList split_lines_preserving_empty(const QString &text)
{
    return text.split(QRegularExpression(QStringLiteral(R"(\r\n|\n|\r)")), Qt::KeepEmptyParts);
}

QString join_lines(const QStringList &lines, const QString &separator)
{
    return lines.join(separator);
}

QStringList first_lines(const QString &text, int limit)
{
    QStringList lines = split_lines_preserving_empty(text);
    if (lines.size() > limit) {
        lines = lines.mid(0, limit);
    }
    return lines;
}

QString mock_windows_icon_name()
{
#if defined(MOCKS_WINDOWS_ICON_NAME)
    return QStringLiteral(MOCKS_WINDOWS_ICON_NAME);
#else
    return QStringLiteral("economia.ico");
#endif
}

QString find_icon_path(const QString &project_root, const QString &preferred_name = QString())
{
    const QString primary_icon_name = preferred_name.trimmed().isEmpty() ? mock_windows_icon_name() : preferred_name.trimmed();
    const QStringList candidate_names = {
        primary_icon_name,
        QStringLiteral("economia.ico"),
        QStringLiteral("economia.png"),
        QStringLiteral("economia_150-150.png"),
        QStringLiteral("economia_300-300.png")
    };

    const QDir root(project_root);
    const QDir app_dir(QCoreApplication::applicationDirPath());
    const QDir current_dir(QDir::currentPath());
    const QDir source_dir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath());

    for (const QString &icon_name : candidate_names) {
        if (icon_name.isEmpty()) {
            continue;
        }
        QStringList candidates = {
            app_dir.filePath(QStringLiteral("icones/%1").arg(icon_name)),
            app_dir.filePath(QStringLiteral("assets/icones/%1").arg(icon_name)),
            current_dir.filePath(QStringLiteral("icones/%1").arg(icon_name)),
            current_dir.filePath(QStringLiteral("assets/icones/%1").arg(icon_name)),
            current_dir.filePath(QStringLiteral("mocks/icones/%1").arg(icon_name)),
            source_dir.filePath(QStringLiteral("icones/%1").arg(icon_name)),
            source_dir.filePath(QStringLiteral("assets/icones/%1").arg(icon_name)),
            source_dir.filePath(QStringLiteral("../assets/icones/%1").arg(icon_name)),
            source_dir.filePath(QStringLiteral("../source/assets/icones/%1").arg(icon_name)),
        };

        if (!project_root.isEmpty()) {
            candidates.prepend(root.filePath(QStringLiteral("assets/icones/%1").arg(icon_name)));
            candidates.prepend(root.filePath(QStringLiteral("source/assets/icones/%1").arg(icon_name)));
            candidates.prepend(root.filePath(QStringLiteral("mocks/icones/%1").arg(icon_name)));
            candidates.prepend(root.filePath(QStringLiteral("icones/%1").arg(icon_name)));
        }

        for (const QString &candidate : candidates) {
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
        }
    }

    return QString();
}

}  // namespace

QString normalize_dir(const QString &path)
{
    if (path.trimmed().isEmpty()) {
        return QString();
    }

    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

QString detect_project_root()
{
    const QString app_dir = QCoreApplication::applicationDirPath();
    const QString current_dir = QDir::currentPath();
    const QString source_file_dir = QFileInfo(QString::fromUtf8(__FILE__)).absolutePath();

    QStringList seeds = {app_dir, current_dir, source_file_dir};
    QStringList checked;

    auto is_project_root = [](const QString &candidate) {
        const QDir dir(candidate);
        return dir.exists(QStringLiteral("mocks")) && dir.exists(QStringLiteral("source"))
               && QFileInfo(dir.filePath(QStringLiteral("CMakeLists.txt"))).exists();
    };

    for (const QString &seed : seeds) {
        QDir dir(seed);
        for (int depth = 0; depth < 8; ++depth) {
            const QString candidate = normalize_dir(dir.absolutePath());
            if (!checked.contains(candidate)) {
                checked.append(candidate);
                if (is_project_root(candidate)) {
                    return candidate;
                }
            }

            if (!dir.cdUp()) {
                break;
            }
        }
    }

    return normalize_dir(QDir(source_file_dir).filePath(QStringLiteral("..")));
}

const QList<FileSpec> &file_specs()
{
    static const QList<FileSpec> specs = [] {
        QList<FileSpec> data;

        auto append = [&](const QString &group,
                          const QString &relative_path,
                          const QString &content_key) {
            QString key = relative_path;
            key.replace(QLatin1Char('/'), QLatin1Char('_'));
            key.replace(QLatin1Char(' '), QLatin1Char('_'));
            key.replace(QLatin1Char('-'), QLatin1Char('_'));
            key.replace(QLatin1Char('.'), QLatin1Char('_'));
            if (!content_key.isEmpty() && (relative_path == QStringLiteral("README.md") || relative_path.endsWith(QStringLiteral(".iss")))) {
                key += QLatin1Char('_') + content_key;
            }
            data.append({key, relative_path, content_key, group});
        };

        // Sobre
        append(group_about(), QStringLiteral("source/assets/ABOUT/ABOUT_pt_BR.txt"), QStringLiteral("about_pt"));
        append(group_about(), QStringLiteral("source/assets/ABOUT/ABOUT_en_US.txt"), QStringLiteral("about_en"));
        append(group_about(), QStringLiteral("source/ui/ui_28_exibir_sobre.cpp"), QStringLiteral("about_dialog"));

        // CLC
        append(group_clc(), QStringLiteral("source/assets/CLC/CLC_pt_BR - Economia.txt"), QStringLiteral("clc_pt"));
        append(group_clc(), QStringLiteral("source/assets/CLC/CLC_en_US - Economia.txt"), QStringLiteral("clc_en"));

        // Direitos Autorais
        append(group_copyright(), QStringLiteral("source/assets/COPYRIGHT/AVISO DE COPYRIGHT E MARCA REGISTRA_pt_BR.txt"), QStringLiteral("copyright_pt"));
        append(group_copyright(), QStringLiteral("source/assets/COPYRIGHT/COPYRIGHT AND TRADEMARK NOTICE_en_US.txt"), QStringLiteral("copyright_en"));

        // EULA
        append(group_eula(), QStringLiteral("source/assets/EULA/EULA_pt_BR - Economia.txt"), QStringLiteral("eula_pt"));
        append(group_eula(), QStringLiteral("source/assets/EULA/EULA_en_US - Economia.txt"), QStringLiteral("eula_en"));

        // Notices
        append(group_notices(), QStringLiteral("source/assets/NOTICES/NOTICE_pt_BR.txt"), QStringLiteral("notice_pt"));
        append(group_notices(), QStringLiteral("source/assets/NOTICES/NOTICE_en_US.txt"), QStringLiteral("notice_en"));

        // Política de Privacidade
        append(group_privacy(), QStringLiteral("source/assets/PRIVACY_POLICY/Privacy_Policy_pt_BR.txt"), QStringLiteral("privacy_pt"));
        append(group_privacy(), QStringLiteral("source/assets/PRIVACY_POLICY/Privacy_Policy_en_US.txt"), QStringLiteral("privacy_en"));

        // Miscelânia
        append(group_misc(), QStringLiteral("README.md"), QStringLiteral("readme_pt"));
        append(group_misc(), QStringLiteral("README.md"), QStringLiteral("readme_en"));
        append(group_misc(), QStringLiteral("MANUAL.md"), QStringLiteral("manual_pt"));
        append(group_misc(), QStringLiteral("OBSERVACAO.md"), QStringLiteral("observacao_pt"));
        append(group_misc(), QStringLiteral("repo/Economia_APP.iss"), QStringLiteral("installer_unified"));
        append(group_misc(), QStringLiteral("repo/Economia_APP_MinGW.iss"), QStringLiteral("installer_mingw"));
        append(group_misc(), QStringLiteral("repo/Economia_APP_MSVC.iss"), QStringLiteral("installer_msvc"));

        return data;
    }();

    return specs;
}

QList<FileStatus> check_expected_lines(const QString &base_dir)
{
    QList<FileStatus> statuses;

    for (const FileSpec &spec : file_specs()) {
        FileStatus status;
        status.key = spec.key;
        status.group = spec.group.isEmpty() ? group_misc() : spec.group;
        status.relative_path = spec.relative_path;
        status.path = resolve_file_path(base_dir, spec.relative_path);

        const std::optional<QString> text = read_file_text(status.path);
        if (!text.has_value()) {
            status.exists = false;
            statuses.append(status);
            continue;
        }

        status.exists = true;
        const QStringList pattern_list = patterns().value(spec_content_key(spec));
        if (pattern_list.isEmpty()) {
            status.found = first_lines(text.value(), 3);
            statuses.append(status);
            continue;
        }

        for (const QString &pattern : pattern_list) {
            const QRegularExpression rx(pattern, QRegularExpression::MultilineOption);
            const QRegularExpressionMatch match = rx.match(text.value());
            if (match.hasMatch()) {
                status.found.append(match.captured(1).trimmed());
            } else {
                status.missing.append(pattern);
            }
        }

        statuses.append(status);
    }

    return statuses;
}

bool apply_line_updates(const QString &key, QStringList *lines, const UpdateContext &context)
{
    if (lines == nullptr) {
        return false;
    }

    bool changed = false;
    for (QString &line : *lines) {
        const QString original = line;

        if (key == QStringLiteral("about_pt") || key == QStringLiteral("history_pt")) {
            if (line.startsWith(QStringLiteral("Versão:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Versao:"))) {
                line = QStringLiteral("Versao: %1").arg(context.pt_version);
            }
        } else if (key == QStringLiteral("about_en") || key == QStringLiteral("history_en")) {
            if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            }
        } else if (key == QStringLiteral("about_dialog")) {
            const QRegularExpression rx(QStringLiteral(R"((<p><b>%1:</b>\s*)[\d\.]+(</p>))"));
            const QRegularExpressionMatch match = rx.match(line);
            if (match.hasMatch()) {
                line = line.left(match.capturedStart(0)) + match.captured(1) + context.pt_version
                       + match.captured(2) + line.mid(match.capturedEnd(0));
            }
        } else if (key == QStringLiteral("clc_pt")) {
            if (line.startsWith(QStringLiteral("Versão:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: %1").arg(context.pt_date);
            }
        } else if (key == QStringLiteral("clc_en")) {
            if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Date:"))) {
                line = QStringLiteral("Date: %1").arg(context.en_date);
            }
        } else if (key == QStringLiteral("privacy_pt")) {
            if (line.startsWith(QStringLiteral("Versão:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Última atualização:"))) {
                line = QStringLiteral("Última atualização: %1").arg(context.pt_date);
            }
        } else if (key == QStringLiteral("privacy_en")) {
            if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Last updated:"))) {
                line = QStringLiteral("Last updated: %1").arg(context.en_date);
            }
        } else if (key == QStringLiteral("notice_pt") || key == QStringLiteral("eula_pt")
                   || key == QStringLiteral("copyright_pt")) {
            if (line.startsWith(QStringLiteral("Versão:"))) {
                line = QStringLiteral("Versão: %1").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: %1").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Versão "))) {
                line = QStringLiteral("Versão %1, %2").arg(context.pt_version, context.pt_date);
            }
        } else if (key == QStringLiteral("notice_en") || key == QStringLiteral("eula_en")
                   || key == QStringLiteral("copyright_en")) {
            if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1").arg(context.en_version);
            } else if (line.startsWith(QStringLiteral("Date:"))) {
                line = QStringLiteral("Date: %1").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("Version "))) {
                line = QStringLiteral("Version %1, %2").arg(context.en_version, context.en_date);
            }
        } else if (key == QStringLiteral("manual_pt")) {
            if (line.startsWith(QStringLiteral("Versão documentada:"))) {
                line = QStringLiteral("Versão documentada: `%1`").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data:"))) {
                line = QStringLiteral("Data: `%1`").arg(context.pt_date);
            }
        } else if (key == QStringLiteral("observacao_pt")) {
            if (line.startsWith(QStringLiteral("Versão documentada:"))) {
                line = QStringLiteral("Versão documentada: `%1`").arg(context.pt_version);
            } else if (line.startsWith(QStringLiteral("Data do snapshot técnico:"))) {
                line = QStringLiteral("Data do snapshot técnico: `%1`").arg(context.pt_date);
            }
        } else if (key == QStringLiteral("readme_pt")) {
            if (line.startsWith(QStringLiteral("> **Observação:**")) ||
                line.startsWith(QStringLiteral("> **Observacao:**"))) {
                line = replace_readme_version_token(line, context.pt_readme_version);
            } else if (line.startsWith(QStringLiteral("Versão:"))) {
                line = QStringLiteral("Versão: %1<br>").arg(context.pt_readme_version);
            } else if (line.startsWith(QStringLiteral("Versao:"))) {
                line = QStringLiteral("Versao: %1<br>").arg(context.pt_readme_version);
            } else if (line.startsWith(QStringLiteral("Data técnica desta revisão:"))) {
                line = QStringLiteral("Data técnica desta revisão: %1<br>").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Data tecnica desta revisao:"))) {
                line = QStringLiteral("Data tecnica desta revisao: %1<br>").arg(context.pt_date);
            } else if (line.startsWith(QStringLiteral("Esta versão oficial"))) {
                const QRegularExpression rx(QStringLiteral(R"(\*\*(?:v?[\d\.]+)\*\*)"));
                line.replace(rx, QStringLiteral("**%1**").arg(context.pt_version));
            }
        } else if (key == QStringLiteral("readme_en")) {
            if (line.startsWith(QStringLiteral("> **Note:**"))) {
                line = replace_readme_version_token(line, context.en_readme_version);
            } else if (line.startsWith(QStringLiteral("Version:"))) {
                line = QStringLiteral("Version: %1<br>").arg(context.en_readme_version);
            } else if (line.startsWith(QStringLiteral("Technical revision date:"))) {
                line = QStringLiteral("Technical revision date: %1<br>").arg(context.en_date);
            } else if (line.startsWith(QStringLiteral("This official release"))) {
                const QRegularExpression rx(QStringLiteral(R"(\*\*(?:v?[\d\.]+)\*\*)"));
                line.replace(rx, QStringLiteral("**%1**").arg(context.en_version));
            }
        } else if (key == QStringLiteral("installer_unified") || key == QStringLiteral("installer_mingw")
                   || key == QStringLiteral("installer_msvc")) {
            if (line.startsWith(QStringLiteral("#define MyAppVersion"))) {
                const QRegularExpression rx(QStringLiteral(R"((#define\s+MyAppVersion\s+")[\d\.]+(";?))"));
                const QRegularExpressionMatch match = rx.match(line);
                if (match.hasMatch()) {
                    line = match.captured(1) + context.pt_version + match.captured(2);
                }
            } else if (key == QStringLiteral("installer_mingw")
                       && line.startsWith(QStringLiteral("OutputBaseFilename=Economia_APP_mingw_v"))) {
                line = QStringLiteral("OutputBaseFilename=Economia_APP_mingw_v%1").arg(context.pt_version);
            } else if (key == QStringLiteral("installer_msvc")
                       && line.trimmed().startsWith(QStringLiteral("OutputBaseFilename=Economia_APP_msvc_v"))) {
                const QString indentation = line.left(line.size() - line.trimmed().size());
                line = QStringLiteral("%1OutputBaseFilename=Economia_APP_msvc_v%2")
                           .arg(indentation, context.pt_version);
            }
        }

        if (line != original) {
            changed = true;
        }
    }

    return changed;
}

QList<UpdateResult> apply_updates(const UpdateContext &context, const QString &base_dir)
{
    QList<UpdateResult> results;

    for (const FileSpec &spec : file_specs()) {
        UpdateResult result;
        result.key = spec.key;
        result.group = spec.group.isEmpty() ? group_misc() : spec.group;
        result.relative_path = spec.relative_path;

        const QString path = resolve_file_path(base_dir, spec.relative_path);
        const std::optional<QString> text = read_file_text(path);
        if (!text.has_value()) {
            result.ok = false;
            result.message = QStringLiteral("arquivo inexistente");
            results.append(result);
            continue;
        }

        const QString line_ending = detect_line_ending(text.value());
        QStringList lines = split_lines_preserving_empty(text.value());
        const bool changed = apply_line_updates(spec_content_key(spec), &lines, context);

        if (!changed) {
            result.ok = false;
            result.message = QStringLiteral("padrões não encontrados / sem alteração");
            results.append(result);
            continue;
        }

        QString error_message;
        if (write_file_text(path, join_lines(lines, line_ending), &error_message)) {
            result.ok = true;
            result.message = QStringLiteral("atualizado");
        } else {
            result.ok = false;
            result.message = QStringLiteral("erro escrita: %1").arg(error_message);
        }

        results.append(result);
    }

    return results;
}

VersionEditorWidget::VersionEditorWidget(QWidget *parent) : QWidget(parent), selected_base_dir_(detect_project_root())
{
    setWindowTitle(QStringLiteral("Editor de Versões / Datas - Economia_APP (PT/EN)"));
    resize(980, 640);

    const QDate today = QDate::currentDate();
    const QString default_version = build_version_from_date(today);
    const int default_day = today.isValid() ? today.day() : 1;
    const int default_month_index = today.isValid() ? qMax(0, today.month() - 1) : 0;
    const int default_year = today.isValid() ? today.year() : 2026;

    const QString icon_path = find_icon_path(selected_base_dir_, QStringLiteral("economia.ico"));
    if (!icon_path.isEmpty()) {
        const QIcon icon(icon_path);
        QApplication::setWindowIcon(icon);
        setWindowIcon(icon);
    }

    auto *layout = new QVBoxLayout(this);

    auto *folder_layout = new QHBoxLayout();
    folder_layout->addWidget(new QLabel(QStringLiteral("Pasta dos arquivos:"), this));

    folder_path_ = new QLineEdit(selected_base_dir_, this);
    folder_path_->setReadOnly(true);
    folder_layout->addWidget(folder_path_);

    select_folder_btn_ = new QPushButton(QStringLiteral("Selecionar..."), this);
    folder_layout->addWidget(select_folder_btn_);
    layout->addLayout(folder_layout);

    auto *pt_group = new QGroupBox(QStringLiteral("Português (pt-BR)"), this);
    auto *pt_layout = new QHBoxLayout(pt_group);
    pt_layout->addWidget(new QLabel(QStringLiteral("Versão:"), pt_group));
    pt_version_ = new QLineEdit(default_version, pt_group);
    pt_layout->addWidget(pt_version_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Dia:"), pt_group));
    pt_day_ = new QSpinBox(pt_group);
    pt_day_->setRange(1, 31);
    pt_day_->setValue(default_day);
    pt_layout->addWidget(pt_day_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Mês:"), pt_group));
    pt_month_ = new QComboBox(pt_group);
    pt_month_->addItems(kPtMonths);
    pt_month_->setCurrentIndex(default_month_index);
    pt_layout->addWidget(pt_month_);

    pt_layout->addWidget(new QLabel(QStringLiteral("Ano:"), pt_group));
    pt_year_ = new QSpinBox(pt_group);
    pt_year_->setRange(1900, 3000);
    pt_year_->setValue(default_year);
    pt_layout->addWidget(pt_year_);
    layout->addWidget(pt_group);

    auto *en_group = new QGroupBox(QStringLiteral("English (en-US)"), this);
    auto *en_layout = new QHBoxLayout(en_group);
    en_layout->addWidget(new QLabel(QStringLiteral("Version:"), en_group));
    en_version_ = new QLineEdit(default_version, en_group);
    en_layout->addWidget(en_version_);

    en_layout->addWidget(new QLabel(QStringLiteral("Day:"), en_group));
    en_day_ = new QSpinBox(en_group);
    en_day_->setRange(1, 31);
    en_day_->setValue(default_day);
    en_layout->addWidget(en_day_);

    en_layout->addWidget(new QLabel(QStringLiteral("Month:"), en_group));
    en_month_ = new QComboBox(en_group);
    en_month_->addItems(kEnMonths);
    en_month_->setCurrentIndex(default_month_index);
    en_layout->addWidget(en_month_);

    en_layout->addWidget(new QLabel(QStringLiteral("Year:"), en_group));
    en_year_ = new QSpinBox(en_group);
    en_year_->setRange(1900, 3000);
    en_year_->setValue(default_year);
    en_layout->addWidget(en_year_);
    layout->addWidget(en_group);

    auto *btn_layout = new QHBoxLayout();
    check_btn_ = new QPushButton(QStringLiteral("Verificar arquivos"), this);
    save_btn_ = new QPushButton(QStringLiteral("Salvar alterações"), this);
    refresh_btn_ = new QPushButton(QStringLiteral("Recarregar verificação"), this);

    const QString icon_btn = find_icon_path(selected_base_dir_, QStringLiteral("economia.png"));
    if (!icon_btn.isEmpty()) {
        check_btn_->setIcon(QIcon(icon_btn));
        save_btn_->setIcon(QIcon(icon_btn));
        refresh_btn_->setIcon(QIcon(icon_btn));
        select_folder_btn_->setIcon(QIcon(icon_btn));
    }

    btn_layout->addWidget(check_btn_);
    btn_layout->addWidget(save_btn_);
    btn_layout->addWidget(refresh_btn_);
    layout->addLayout(btn_layout);

    status_tabs_ = new QTabWidget(this);
    status_tabs_->setDocumentMode(true);
    status_tabs_->setUsesScrollButtons(true);
    status_tabs_->setElideMode(Qt::ElideRight);
    layout->addWidget(status_tabs_, 1);

    connect(select_folder_btn_, &QPushButton::clicked, this, [this]() { selectBaseDirectory(); });
    connect(check_btn_, &QPushButton::clicked, this, [this]() { checkFiles(); });
    connect(save_btn_, &QPushButton::clicked, this, [this]() { saveChanges(); });
    connect(refresh_btn_, &QPushButton::clicked, this, [this]() { checkFiles(); });

    checkFiles();
}

void VersionEditorWidget::selectBaseDirectory()
{
    const QString selected = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Selecione a pasta onde os arquivos estão"), selected_base_dir_);
    if (selected.isEmpty()) {
        return;
    }

    selected_base_dir_ = normalize_dir(selected);
    folder_path_->setText(selected_base_dir_);
    checkFiles();
}

void VersionEditorWidget::checkFiles()
{
    const QList<FileStatus> statuses = check_expected_lines(selected_base_dir_);
    QMap<QString, QStringList> grouped_lines;
    for (const QString &group : group_order()) {
        grouped_lines[group].append(QStringLiteral("Base: %1").arg(selected_base_dir_));
        grouped_lines[group].append(QString());
    }

    for (const FileStatus &status : statuses) {
        QStringList &lines = grouped_lines[status.group];
        lines.append(QStringLiteral("%1").arg(status.relative_path));
        lines.append(QStringLiteral("  chave: %1").arg(status.key));
        lines.append(QStringLiteral("  arquivo: %1").arg(status.exists ? QStringLiteral("existe") : QStringLiteral("ausente")));

        if (status.exists) {
            if (!status.found.isEmpty()) {
                lines.append(QStringLiteral("  encontrado:"));
                for (const QString &entry : status.found) {
                    lines.append(QStringLiteral("    - %1").arg(entry));
                }
            }

            if (!status.missing.isEmpty()) {
                lines.append(QStringLiteral("  ausente no conteúdo:"));
                for (const QString &entry : status.missing) {
                    lines.append(QStringLiteral("    - %1").arg(entry));
                }
            }
        }

        lines.append(QString());
    }

    QList<QPair<QString, QStringList>> groups;
    for (const QString &group : group_order()) {
        groups.append({group, grouped_lines.value(group)});
    }
    setStatusGroups(groups);
}

void VersionEditorWidget::saveChanges()
{
    const QString pt_version = pt_version_->text().trimmed();
    const int pt_day = pt_day_->value();
    const int pt_month_index = pt_month_->currentIndex();
    const int pt_year = pt_year_->value();

    const QString en_version = en_version_->text().trimmed();
    const int en_day = en_day_->value();
    const int en_month_index = en_month_->currentIndex();
    const int en_year = en_year_->value();

    const UpdateContext context = {
        pt_version,
        QStringLiteral("%1 de %2 de %3")
            .arg(QString::number(pt_day), kPtMonthsLower.value(pt_month_index), QString::number(pt_year)),
        en_version,
        QStringLiteral("%1 %2, %3")
            .arg(kEnMonths.value(en_month_index), QString::number(en_day), QString::number(en_year)),
        readme_version_token(pt_version),
        readme_version_token(en_version),
    };

    const QList<UpdateResult> results = apply_updates(context, selected_base_dir_);

    QMap<QString, QStringList> grouped_lines;
    for (const QString &group : group_order()) {
        grouped_lines[group].append(QStringLiteral("Resultados:"));
        grouped_lines[group].append(QString());
    }

    for (const UpdateResult &result : results) {
        QStringList &lines = grouped_lines[result.group];
        lines.append(QStringLiteral("%1: %2 - %3")
                         .arg(result.relative_path,
                              result.ok ? QStringLiteral("OK") : QStringLiteral("FAIL"),
                              result.message));
    }

    QList<QPair<QString, QStringList>> groups;
    for (const QString &group : group_order()) {
        groups.append({group, grouped_lines.value(group)});
    }
    setStatusGroups(groups);
}

void VersionEditorWidget::setStatusLines(const QStringList &lines)
{
    setStatusGroups({{group_misc(), lines}});
}

void VersionEditorWidget::setStatusGroups(const QList<QPair<QString, QStringList>> &groups)
{
    if (status_tabs_ == nullptr) {
        return;
    }

    const QString current_title =
        status_tabs_->currentIndex() >= 0 ? status_tabs_->tabText(status_tabs_->currentIndex()) : QString();

    while (status_tabs_->count() > 0) {
        QWidget *page = status_tabs_->widget(0);
        status_tabs_->removeTab(0);
        if (page != nullptr) {
            page->deleteLater();
        }
    }

    int target_index = 0;
    for (const auto &group : groups) {
        auto *editor = new QTextEdit(status_tabs_);
        editor->setReadOnly(true);
        editor->setLineWrapMode(QTextEdit::NoWrap);
        QStringList lines = group.second;
        if (lines.isEmpty()) {
            lines.append(QStringLiteral("Nenhum arquivo neste grupo."));
        }
        editor->setPlainText(lines.join(QLatin1Char('\n')));

        const int index = status_tabs_->addTab(editor, group.first);
        if (!current_title.isEmpty() && group.first == current_title) {
            target_index = index;
        }
    }

    if (status_tabs_->count() > 0) {
        status_tabs_->setCurrentIndex(qBound(0, target_index, status_tabs_->count() - 1));
    }
}

}  // namespace mocks::version_editor

int main(int argc, char *argv[])
{
    if (argc > 1) {
        const QString arg = QString::fromLocal8Bit(argv[1]).trimmed().toLower();
        if (arg == QStringLiteral("--check") || arg == QStringLiteral("-c") || arg == QStringLiteral("--status")) {
#if defined(Q_OS_WIN)
            if (AttachConsole(ATTACH_PARENT_PROCESS)) {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == nullptr || hOut == INVALID_HANDLE_VALUE || GetFileType(hOut) == FILE_TYPE_UNKNOWN) {
                    FILE *fp = nullptr;
                    freopen_s(&fp, "CONOUT$", "w", stdout);
                    freopen_s(&fp, "CONOUT$", "w", stderr);
                }
            }
#endif
            QTextStream out(stdout);
            const QString base_dir = (argc > 2) ? mocks::version_editor::normalize_dir(QString::fromLocal8Bit(argv[2]))
                                                : mocks::version_editor::detect_project_root();
            const auto statuses = mocks::version_editor::check_expected_lines(base_dir);
            bool all_ok = true;
            for (const auto &status : statuses) {
                if (!status.exists) {
                    all_ok = false;
                    out << "[AUSENTE] " << status.relative_path << "\n";
                } else if (!status.missing.isEmpty()) {
                    all_ok = false;
                    out << "[PADRAO AUSENTE] " << status.relative_path << "\n";
                } else {
                    out << "[OK] " << status.relative_path << "\n";
                }
            }
            out << (all_ok ? "TODOS OS ARQUIVOS VERIFICADOS COM SUCESSO.\n"
                           : "FALHAS NA VERIFICACAO DE ARQUIVOS.\n");
            return all_ok ? 0 : 1;
        }
    }

    QApplication app(argc, argv);
    mocks::version_editor::VersionEditorWidget widget;
    widget.show();
    return app.exec();
}
