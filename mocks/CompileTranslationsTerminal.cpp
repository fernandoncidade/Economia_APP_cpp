#include "CompileTranslationsTerminal.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>

#if defined(Q_OS_WIN)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {

QString normalize_target_id(QString value)
{
    value = value.trimmed().toLower();
    value.remove(QLatin1Char('_'));
    value.remove(QLatin1Char('-'));
    value.remove(QLatin1Char(' '));

    if (value.contains(QStringLiteral("economia")) || value == QStringLiteral("host") || value == QStringLiteral("app")) {
        return QStringLiteral("economia");
    }
    if (value == QStringLiteral("todos") || value == QStringLiteral("all")) {
        return QStringLiteral("all");
    }

    return value;
}

QString find_project_root_from(const QString &start_path)
{
    QFileInfo info(start_path);
    QDir dir(info.isDir() ? info.absoluteFilePath() : info.absolutePath());

    while (true) {
        const bool has_project_files =
            dir.exists(QStringLiteral("CMakeLists.txt")) && dir.exists(QStringLiteral("source"));
        const bool has_translations = dir.exists(QStringLiteral("source/language/translations"));

        if (has_project_files && has_translations) {
            return dir.absolutePath();
        }

        if (!dir.cdUp()) {
            break;
        }
    }

    return {};
}

QString mock_windows_icon_name()
{
#if defined(MOCKS_WINDOWS_ICON_NAME)
    return QStringLiteral(MOCKS_WINDOWS_ICON_NAME);
#else
    return QStringLiteral("economia.ico");
#endif
}

QString find_mock_taskbar_icon_path()
{
#if defined(Q_OS_WIN)
    const QString primary_icon_name = mock_windows_icon_name();
    const QStringList candidate_names = {
        primary_icon_name,
        QStringLiteral("economia.ico"),
        QStringLiteral("economia.png"),
        QStringLiteral("economia_150-150.png"),
        QStringLiteral("economia_300-300.png")
    };
    const QDir app_dir(QCoreApplication::applicationDirPath());
    const QDir current_dir(QDir::currentPath());
    const QDir source_dir(QFileInfo(QString::fromLocal8Bit(__FILE__)).absolutePath());
    const QString project_root = find_project_root_from(app_dir.absolutePath());

    for (const QString &name : candidate_names) {
        if (name.isEmpty()) {
            continue;
        }
        QStringList candidates = {
            app_dir.filePath(QStringLiteral("icones/%1").arg(name)),
            current_dir.filePath(QStringLiteral("icones/%1").arg(name)),
            current_dir.filePath(QStringLiteral("mocks/icones/%1").arg(name)),
            source_dir.filePath(QStringLiteral("icones/%1").arg(name)),
            source_dir.filePath(QStringLiteral("../source/assets/icones/%1").arg(name))
        };

        if (!project_root.isEmpty()) {
            candidates.prepend(QDir(project_root).filePath(QStringLiteral("source/assets/icones/%1").arg(name)));
            candidates.prepend(QDir(project_root).filePath(QStringLiteral("mocks/icones/%1").arg(name)));
            candidates.prepend(QDir(project_root).filePath(QStringLiteral("icones/%1").arg(name)));
        }

        for (const QString &candidate : candidates) {
            if (QFileInfo::exists(candidate)) {
                return QFileInfo(candidate).absoluteFilePath();
            }
        }
    }
#endif
    return {};
}

void apply_console_taskbar_icon()
{
#if defined(Q_OS_WIN)
    const HWND console_window = GetConsoleWindow();
    if (console_window == nullptr) {
        return;
    }

    const QString icon_path = find_mock_taskbar_icon_path();
    if (icon_path.isEmpty() || !icon_path.endsWith(QStringLiteral(".ico"), Qt::CaseInsensitive)) {
        return;
    }

    const std::wstring native_icon_path = QDir::toNativeSeparators(icon_path).toStdWString();
    const HICON icon_big = static_cast<HICON>(
        LoadImageW(nullptr, native_icon_path.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_SHARED));
    const HICON icon_small = static_cast<HICON>(
        LoadImageW(nullptr, native_icon_path.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE | LR_SHARED));

    if (icon_big != nullptr) {
        SendMessageW(console_window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon_big));
    }
    if (icon_small != nullptr) {
        SendMessageW(console_window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon_small));
    }
#endif
}

QVector<mocks::translations::TranslationTarget> available_targets()
{
    return {
        {QStringLiteral("all"),
         QStringLiteral("Todos os Arquivos de Tradução (source/language/translations)"),
         {QStringLiteral("*.ts")},
         QStringLiteral("source/language/translations")},
        {QStringLiteral("economia"),
         QStringLiteral("Economia_APP (Principal)"),
         {QStringLiteral("economia_*.ts")},
         QStringLiteral("source/language/translations")},
    };
}

QString trim_process_output(const QString &text)
{
    const QString simplified = text.trimmed();
    return simplified.isEmpty() ? QStringLiteral("(sem saida)") : simplified;
}

}  // namespace

namespace mocks::translations {

TranslationCompilerTerminal::TranslationCompilerTerminal()
    : targets_(available_targets()),
      project_root_(locate_project_root()),
      lrelease_executable_(locate_lrelease_executable())
{
}

QString TranslationCompilerTerminal::locate_project_root() const
{
    const QString app_path = QCoreApplication::applicationDirPath();
    QString found = find_project_root_from(app_path);
    if (!found.isEmpty()) {
        return found;
    }

    found = find_project_root_from(QDir::currentPath());
    if (!found.isEmpty()) {
        return found;
    }

    const QDir source_dir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath());
    return find_project_root_from(source_dir.absolutePath());
}

QString TranslationCompilerTerminal::locate_lrelease_executable() const
{
    const QString binary_name =
#if defined(Q_OS_WIN)
        QStringLiteral("lrelease.exe");
#else
        QStringLiteral("lrelease");
#endif

    const QStringList direct_candidates = {
        QLibraryInfo::path(QLibraryInfo::BinariesPath) + QLatin1Char('/') + binary_name,
        QDir(QCoreApplication::applicationDirPath()).filePath(binary_name),
        QStringLiteral("C:/Qt/6.11.1/mingw_64/bin/%1").arg(binary_name),
        QStringLiteral("C:/Qt/6.11.1/msvc2022_64/bin/%1").arg(binary_name)
    };

    for (const QString &candidate : direct_candidates) {
        if (QFileInfo::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
    }

    const QString path_lookup = QStandardPaths::findExecutable(QStringLiteral("lrelease"));
    if (!path_lookup.isEmpty()) {
        return QDir::cleanPath(path_lookup);
    }

    return {};
}

void TranslationCompilerTerminal::print_help(QTextStream &out) const
{
    out << "Uso: CompileTranslationsTerminal [OPCOES]\n\n";
    out << "Opcoes:\n";
    out << "  --all, -a                 Compila todos os catalogos disponiveis\n";
    out << "  --module, -m <id>         Compila apenas o catalogo especificado\n";
    out << "  --target <id>             Alias para --module\n";
    out << "  --source-dir <caminho>    Substitui a pasta fonte de arquivos .ts\n";
    out << "  --output-dir <caminho>    Substitui a pasta de destino dos arquivos .qm\n";
    out << "  --list, -l                Lista os alvos disponiveis e sai\n";
    out << "  --help, -h                Exibe esta mensagem de ajuda\n\n";
    out << "Modulos disponiveis:\n";
    for (const auto &target : targets_) {
        out << "  - " << target.id << " : " << target.display_name << "\n";
    }
    out << "\n";
}

void TranslationCompilerTerminal::print_targets(QTextStream &out) const
{
    out << "Catalogos configurados:\n";
    for (const auto &target : targets_) {
        out << "  [" << target.id << "] " << target.display_name << "\n";
        out << "    rel: " << target.translations_relative_path << "\n";
        out << "    padroes: " << target.file_patterns.join(QStringLiteral(", ")) << "\n";
    }
}

TranslationCompilerTerminal::CliOptions TranslationCompilerTerminal::parse_arguments(
    const QStringList &arguments, QTextStream &out, int &exit_code) const
{
    CliOptions options;

    for (int i = 1; i < arguments.size(); ++i) {
        const QString arg = arguments.at(i);

        if (arg == QStringLiteral("--help") || arg == QStringLiteral("-h") || arg == QStringLiteral("/?")) {
            options.show_help = true;
            return options;
        }
        if (arg == QStringLiteral("--list") || arg == QStringLiteral("-l")) {
            options.list_targets = true;
            return options;
        }
        if (arg == QStringLiteral("--all") || arg == QStringLiteral("-a")) {
            options.target_id = QStringLiteral("all");
            continue;
        }
        if ((arg == QStringLiteral("--module") || arg == QStringLiteral("-m") || arg == QStringLiteral("--target")) &&
            i + 1 < arguments.size()) {
            options.target_id = normalize_target_id(arguments.at(++i));
            continue;
        }
        if (arg.startsWith(QStringLiteral("--module="))) {
            options.target_id = normalize_target_id(arg.mid(QStringLiteral("--module=").size()));
            continue;
        }
        if (arg.startsWith(QStringLiteral("--target="))) {
            options.target_id = normalize_target_id(arg.mid(QStringLiteral("--target=").size()));
            continue;
        }
        if (arg == QStringLiteral("--source-dir") && i + 1 < arguments.size()) {
            options.custom_source_dir = QDir::fromNativeSeparators(arguments.at(++i).trimmed());
            continue;
        }
        if (arg.startsWith(QStringLiteral("--source-dir="))) {
            options.custom_source_dir =
                QDir::fromNativeSeparators(arg.mid(QStringLiteral("--source-dir=").size()).trimmed());
            continue;
        }
        if (arg == QStringLiteral("--output-dir") && i + 1 < arguments.size()) {
            options.custom_output_dir = QDir::fromNativeSeparators(arguments.at(++i).trimmed());
            continue;
        }
        if (arg.startsWith(QStringLiteral("--output-dir="))) {
            options.custom_output_dir =
                QDir::fromNativeSeparators(arg.mid(QStringLiteral("--output-dir=").size()).trimmed());
            continue;
        }

        out << "[ERRO] Argumento desconhecido: " << arg << "\n\n";
        print_help(out);
        exit_code = 1;
        return options;
    }

    return options;
}

TranslationTarget TranslationCompilerTerminal::resolve_target(const QString &id) const
{
    const QString normalized = normalize_target_id(id);
    for (const auto &target : targets_) {
        if (target.id == normalized) {
            return target;
        }
    }
    return targets_.isEmpty() ? TranslationTarget{} : targets_.first();
}

int TranslationCompilerTerminal::compile_target(const TranslationTarget &target, const QString &source_dir_path,
                                                const QString &output_dir_path, QTextStream &out) const
{
    const QDir source_dir(source_dir_path);
    const QDir output_dir(output_dir_path);

    if (!source_dir.exists()) {
        out << "[ERRO] Pasta fonte nao existe: " << QDir::toNativeSeparators(source_dir_path) << "\n";
        return 1;
    }

    if (!output_dir.exists()) {
        output_dir.mkpath(QStringLiteral("."));
    }

    QStringList ts_files;
    for (const QString &pattern : target.file_patterns) {
        const QStringList entries = source_dir.entryList(QStringList{pattern}, QDir::Files, QDir::Name);
        for (const QString &entry : entries) {
            if (!ts_files.contains(entry)) {
                ts_files.append(entry);
            }
        }
    }

    if (ts_files.isEmpty()) {
        out << "[AVISO] Nenhum arquivo .ts localizado em " << QDir::toNativeSeparators(source_dir_path)
            << " para o catalogo " << target.id << "\n";
        return 0;
    }

    int failures = 0;
    for (const QString &ts_name : ts_files) {
        const QString source_ts = source_dir.filePath(ts_name);
        const QString base_name = QFileInfo(ts_name).completeBaseName();
        const QString output_qm = output_dir.filePath(base_name + QStringLiteral(".qm"));

        out << "-> " << QDir::toNativeSeparators(source_ts) << " -> " << QDir::toNativeSeparators(output_qm) << "\n";

        QProcess process;
        process.start(lrelease_executable_, {source_ts, QStringLiteral("-qm"), output_qm});

        if (!process.waitForStarted(5000)) {
            failures++;
            out << "   [FALHA] Nao foi possivel iniciar lrelease: " << process.errorString() << "\n";
            continue;
        }

        process.waitForFinished(-1);
        const int code = process.exitCode();
        const QString sout = QString::fromUtf8(process.readAllStandardOutput());
        const QString serr = QString::fromUtf8(process.readAllStandardError());

        if (code == 0) {
            out << "   [OK] " << trim_process_output(sout) << "\n";
        } else {
            failures++;
            out << "   [ERRO " << code << "] " << trim_process_output(sout) << " | " << trim_process_output(serr)
                << "\n";
        }
    }

    return failures == 0 ? 0 : 2;
}

int TranslationCompilerTerminal::run(const QStringList &arguments)
{
    apply_console_taskbar_icon();

    QTextStream out(stdout);
    int parse_error = 0;
    const CliOptions options = parse_arguments(arguments, out, parse_error);

    if (parse_error != 0) {
        return parse_error;
    }
    if (options.show_help) {
        print_help(out);
        return 0;
    }
    if (options.list_targets) {
        print_targets(out);
        return 0;
    }

    if (lrelease_executable_.isEmpty()) {
        out << "[ERRO CRITICO] Utilitario lrelease.exe nao localizado no sistema ou no Qt.\n";
        return 1;
    }

    if (project_root_.isEmpty()) {
        out << "[ERRO CRITICO] Raiz do projeto Economia_APP nao identificada.\n";
        return 1;
    }

    const TranslationTarget target = resolve_target(options.target_id);

    const QString default_dir = QDir::cleanPath(QDir(project_root_).filePath(target.translations_relative_path));
    const QString effective_source =
        options.custom_source_dir.isEmpty() ? default_dir : options.custom_source_dir;
    const QString effective_output =
        options.custom_output_dir.isEmpty() ? effective_source : options.custom_output_dir;

    out << "================================================================================\n";
    out << "Compilador de Traducoes Qt6 (Terminal) - Economia_APP\n";
    out << "Data/Hora: " << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")) << "\n";
    out << "Projeto: " << QDir::toNativeSeparators(project_root_) << "\n";
    out << "lrelease: " << QDir::toNativeSeparators(lrelease_executable_) << "\n";
    out << "Catalogo: " << target.display_name << " (" << target.id << ")\n";
    out << "Pasta Fonte: " << QDir::toNativeSeparators(effective_source) << "\n";
    out << "Pasta Destino: " << QDir::toNativeSeparators(effective_output) << "\n";
    out << "================================================================================\n";

    const int result = compile_target(target, effective_source, effective_output, out);

    out << "================================================================================\n";
    if (result == 0) {
        out << "Status final: SUCESSO. Arquivos .qm gerados com exito.\n";
    } else {
        out << "Status final: FALHA na compilacao de um ou mais arquivos.\n";
    }
    out << "================================================================================\n";

    return result;
}

}  // namespace mocks::translations

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    mocks::translations::TranslationCompilerTerminal compiler;
    return compiler.run(app.arguments());
}
