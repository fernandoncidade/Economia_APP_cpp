#include "CompileTranslationsGUI.hpp"

#include <QApplication>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QScopeGuard>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QStringList>
#include <QTextCursor>
#include <QVBoxLayout>

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

QString argument_target_id(const QStringList &arguments)
{
    for (int i = 1; i < arguments.size(); ++i) {
        const QString argument = arguments.at(i);
        if (argument == QStringLiteral("--all") || argument == QStringLiteral("-a")) {
            return QStringLiteral("all");
        }
        if ((argument == QStringLiteral("--module") || argument == QStringLiteral("-m") ||
             argument == QStringLiteral("--target")) &&
            i + 1 < arguments.size()) {
            return normalize_target_id(arguments.at(i + 1));
        }
        if (argument.startsWith(QStringLiteral("--module="))) {
            return normalize_target_id(argument.mid(QStringLiteral("--module=").size()));
        }
        if (argument.startsWith(QStringLiteral("--target="))) {
            return normalize_target_id(argument.mid(QStringLiteral("--target=").size()));
        }
    }

    return {};
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

QString trim_process_output(const QString &text)
{
    const QString simplified = text.trimmed();
    return simplified.isEmpty() ? QStringLiteral("(sem saida)") : simplified;
}

QString clean_path_from_ui(const QString &path)
{
    const QString normalized = QDir::fromNativeSeparators(path.trimmed());
    return normalized.isEmpty() ? QString() : QDir::cleanPath(normalized);
}

QString display_path_or_status(const QString &path)
{
    return path.trimmed().isEmpty() ? QStringLiteral("Nao localizado") : QDir::toNativeSeparators(path);
}

QString mock_windows_icon_name()
{
#if defined(MOCKS_WINDOWS_ICON_NAME)
    return QStringLiteral(MOCKS_WINDOWS_ICON_NAME);
#else
    return QStringLiteral("economia.ico");
#endif
}

QString find_mock_icon_path()
{
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
    const QDir source_dir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath());
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

    return {};
}

}  // namespace

namespace mocks::translations {

TranslationCompileDialog::TranslationCompileDialog(QWidget *parent, const QString &initial_target_id)
    : QDialog(parent),
      targets_(available_targets()),
      project_root_(locate_project_root()),
      lrelease_executable_(locate_lrelease_executable())
{
    setWindowTitle(QStringLiteral("Compilador de Traducoes Qt6 - Economia_APP"));
    setMinimumSize(800, 600);
    resize(860, 680);

    const QString icon_path = find_mock_icon_path();
    if (!icon_path.isEmpty()) {
        const QIcon icon(icon_path);
        setWindowIcon(icon);
        QApplication::setWindowIcon(icon);
    }

    setup_ui();
    populate_targets(initial_target_id);
    refresh_context_labels();
}

void TranslationCompileDialog::setup_ui()
{
    auto *main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(10);

    auto *header_layout = new QVBoxLayout();
    auto *title_label = new QLabel(QStringLiteral("<h2>Compilar Traducoes (.ts -> .qm) - Economia_APP</h2>"), this);
    title_label->setTextFormat(Qt::RichText);
    header_layout->addWidget(title_label);

    translations_dir_label_ = new QLabel(this);
    translations_dir_label_->setWordWrap(true);
    header_layout->addWidget(translations_dir_label_);

    lrelease_label_ = new QLabel(this);
    lrelease_label_->setWordWrap(true);
    header_layout->addWidget(lrelease_label_);

    status_label_ = new QLabel(this);
    status_label_->setWordWrap(true);
    status_label_->setStyleSheet(QStringLiteral("font-weight: bold; color: #1565C0;"));
    header_layout->addWidget(status_label_);

    main_layout->addLayout(header_layout);

    auto *form_layout = new QVBoxLayout();
    form_layout->setSpacing(6);

    auto *target_layout = new QHBoxLayout();
    auto *target_title = new QLabel(QStringLiteral("Catalogo alvo:"), this);
    target_title->setMinimumWidth(110);
    target_combo_ = new QComboBox(this);
    target_layout->addWidget(target_title);
    target_layout->addWidget(target_combo_, 1);
    form_layout->addLayout(target_layout);

    auto *src_layout = new QHBoxLayout();
    auto *src_title = new QLabel(QStringLiteral("Pasta fonte (.ts):"), this);
    src_title->setMinimumWidth(110);
    source_dir_edit_ = new QLineEdit(this);
    auto *src_browse_button = new QPushButton(QStringLiteral("Selecionar..."), this);
    auto *src_reset_button = new QPushButton(QStringLiteral("Padrao"), this);
    src_layout->addWidget(src_title);
    src_layout->addWidget(source_dir_edit_, 1);
    src_layout->addWidget(src_browse_button);
    src_layout->addWidget(src_reset_button);
    form_layout->addLayout(src_layout);

    auto *out_layout = new QHBoxLayout();
    auto *out_title = new QLabel(QStringLiteral("Pasta destino (.qm):"), this);
    out_title->setMinimumWidth(110);
    output_dir_edit_ = new QLineEdit(this);
    auto *out_browse_button = new QPushButton(QStringLiteral("Selecionar..."), this);
    auto *out_reset_button = new QPushButton(QStringLiteral("Padrao"), this);
    out_layout->addWidget(out_title);
    out_layout->addWidget(output_dir_edit_, 1);
    out_layout->addWidget(out_browse_button);
    out_layout->addWidget(out_reset_button);
    form_layout->addLayout(out_layout);

    main_layout->addLayout(form_layout);

    auto *log_label = new QLabel(QStringLiteral("Log de Execucao:"), this);
    log_output_ = new QPlainTextEdit(this);
    log_output_->setReadOnly(true);
    log_output_->setStyleSheet(QStringLiteral("font-family: Consolas, 'Courier New', monospace; font-size: 10pt;"));
    main_layout->addWidget(log_label);
    main_layout->addWidget(log_output_, 1);

    auto *buttons_layout = new QHBoxLayout();
    compile_button_ = new QPushButton(QStringLiteral("Compilar Traducoes"), this);
    compile_button_->setDefault(true);
    compile_button_->setStyleSheet(QStringLiteral("padding: 6px 14px; font-weight: bold;"));

    auto *close_button = new QPushButton(QStringLiteral("Fechar"), this);
    buttons_layout->addStretch();
    buttons_layout->addWidget(compile_button_);
    buttons_layout->addWidget(close_button);
    main_layout->addLayout(buttons_layout);

    connect(target_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        source_dir_custom_ = false;
        output_dir_custom_ = false;
        sync_path_fields_from_target();
        refresh_context_labels();
    });

    connect(source_dir_edit_, &QLineEdit::textEdited, this, [this](const QString &text) {
        source_dir_custom_ = !text.trimmed().isEmpty();
        refresh_context_labels();
    });

    connect(output_dir_edit_, &QLineEdit::textEdited, this, [this](const QString &text) {
        output_dir_custom_ = !text.trimmed().isEmpty();
        refresh_context_labels();
    });

    connect(src_browse_button, &QPushButton::clicked, this, &TranslationCompileDialog::browse_source_directory);
    connect(out_browse_button, &QPushButton::clicked, this, &TranslationCompileDialog::browse_output_directory);
    connect(src_reset_button, &QPushButton::clicked, this, &TranslationCompileDialog::reset_source_directory);
    connect(out_reset_button, &QPushButton::clicked, this, &TranslationCompileDialog::reset_output_directory);

    connect(compile_button_, &QPushButton::clicked, this, &TranslationCompileDialog::run_compilation);
    connect(close_button, &QPushButton::clicked, this, &QDialog::accept);
}

void TranslationCompileDialog::populate_targets(const QString &initial_target_id)
{
    const QSignalBlocker blocker(target_combo_);
    target_combo_->clear();

    const QString normalized_initial = normalize_target_id(initial_target_id);
    int selected_index = 0;

    for (int i = 0; i < targets_.size(); ++i) {
        const auto &target = targets_.at(i);
        target_combo_->addItem(target.display_name, target.id);
        if (!normalized_initial.isEmpty() && target.id == normalized_initial) {
            selected_index = i;
        }
    }

    target_combo_->setCurrentIndex(selected_index);
    sync_path_fields_from_target();
}

TranslationTarget TranslationCompileDialog::current_target() const
{
    const int index = target_combo_ != nullptr ? target_combo_->currentIndex() : -1;
    if (index >= 0 && index < targets_.size()) {
        return targets_.at(index);
    }
    return targets_.isEmpty() ? TranslationTarget{} : targets_.first();
}

QString TranslationCompileDialog::current_translations_directory() const
{
    if (project_root_.isEmpty()) {
        return {};
    }

    const QString rel = current_target().translations_relative_path;
    return QDir::cleanPath(QDir(project_root_).filePath(rel));
}

QString TranslationCompileDialog::effective_source_directory() const
{
    if (source_dir_custom_ && source_dir_edit_ != nullptr) {
        const QString custom = clean_path_from_ui(source_dir_edit_->text());
        if (!custom.isEmpty()) {
            return custom;
        }
    }
    return current_translations_directory();
}

QString TranslationCompileDialog::effective_output_directory() const
{
    if (output_dir_custom_ && output_dir_edit_ != nullptr) {
        const QString custom = clean_path_from_ui(output_dir_edit_->text());
        if (!custom.isEmpty()) {
            return custom;
        }
    }
    return effective_source_directory();
}

void TranslationCompileDialog::sync_path_fields_from_target()
{
    const QString default_dir = current_translations_directory();
    if (source_dir_edit_ != nullptr && !source_dir_custom_) {
        source_dir_edit_->setText(QDir::toNativeSeparators(default_dir));
    }
    if (output_dir_edit_ != nullptr && !output_dir_custom_) {
        output_dir_edit_->setText(QDir::toNativeSeparators(default_dir));
    }
}

void TranslationCompileDialog::refresh_context_labels()
{
    const QString translations_dir = effective_source_directory();
    translations_dir_label_->setText(
        QStringLiteral("Diretorio ativo de traducoes: %1").arg(display_path_or_status(translations_dir)));

    lrelease_label_->setText(
        QStringLiteral("Utilitario lrelease localizado: %1").arg(display_path_or_status(lrelease_executable_)));

    if (lrelease_executable_.isEmpty()) {
        status_label_->setText(QStringLiteral("Status: lrelease.exe nao localizado no Qt/PATH."));
        status_label_->setStyleSheet(QStringLiteral("font-weight: bold; color: #D32F2F;"));
        compile_button_->setEnabled(false);
    } else if (translations_dir.isEmpty() || !QDir(translations_dir).exists()) {
        status_label_->setText(QStringLiteral("Status: Diretorio de traducoes nao encontrado."));
        status_label_->setStyleSheet(QStringLiteral("font-weight: bold; color: #E65100;"));
        compile_button_->setEnabled(false);
    } else {
        status_label_->setText(QStringLiteral("Status: Pronto para compilar."));
        status_label_->setStyleSheet(QStringLiteral("font-weight: bold; color: #2E7D32;"));
        compile_button_->setEnabled(true);
    }
}

void TranslationCompileDialog::browse_source_directory()
{
    const QString initial = effective_source_directory();
    const QString chosen = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Selecionar pasta fonte com arquivos .ts"), initial);
    if (!chosen.isEmpty()) {
        source_dir_custom_ = true;
        source_dir_edit_->setText(QDir::toNativeSeparators(chosen));
        if (!output_dir_custom_) {
            output_dir_edit_->setText(QDir::toNativeSeparators(chosen));
        }
        refresh_context_labels();
    }
}

void TranslationCompileDialog::browse_output_directory()
{
    const QString initial = effective_output_directory();
    const QString chosen = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Selecionar pasta destino para arquivos .qm"), initial);
    if (!chosen.isEmpty()) {
        output_dir_custom_ = true;
        output_dir_edit_->setText(QDir::toNativeSeparators(chosen));
        refresh_context_labels();
    }
}

void TranslationCompileDialog::reset_source_directory()
{
    source_dir_custom_ = false;
    sync_path_fields_from_target();
    refresh_context_labels();
}

void TranslationCompileDialog::reset_output_directory()
{
    output_dir_custom_ = false;
    sync_path_fields_from_target();
    refresh_context_labels();
}

void TranslationCompileDialog::append_log(const QString &message)
{
    if (log_output_ == nullptr) {
        return;
    }
    log_output_->appendPlainText(message);
    log_output_->moveCursor(QTextCursor::End);
}

QString TranslationCompileDialog::locate_project_root() const
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

QString TranslationCompileDialog::locate_lrelease_executable() const
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

TranslationCompileDialog::CompileSummary TranslationCompileDialog::compile_current_target(
    const QString &source_directory, const QString &output_directory)
{
    CompileSummary summary;
    const TranslationTarget target = current_target();
    const QDir source_dir(source_directory);
    const QDir output_dir(output_directory);

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

    summary.discovered_files = ts_files.size();
    if (ts_files.isEmpty()) {
        append_log(QStringLiteral("[AVISO] Nenhum arquivo .ts encontrado em %1 para o padrao selecionado.")
                       .arg(QDir::toNativeSeparators(source_directory)));
        return summary;
    }

    for (const QString &ts_file_name : ts_files) {
        const QString source_ts_path = source_dir.filePath(ts_file_name);
        const QString base_name = QFileInfo(ts_file_name).completeBaseName();
        const QString output_qm_path = output_dir.filePath(base_name + QStringLiteral(".qm"));

        append_log(QStringLiteral("-> Compilando: %1 -> %2")
                       .arg(QDir::toNativeSeparators(source_ts_path), QDir::toNativeSeparators(output_qm_path)));

        QProcess process;
        const QStringList arguments = {source_ts_path, QStringLiteral("-qm"), output_qm_path};
        process.start(lrelease_executable_, arguments);

        if (!process.waitForStarted(5000)) {
            summary.failed_files++;
            append_log(QStringLiteral("   [ERRO] Falha ao iniciar processo lrelease: %1").arg(process.errorString()));
            continue;
        }

        process.waitForFinished(-1);
        const int exit_code = process.exitCode();
        const QString stdout_text = QString::fromUtf8(process.readAllStandardOutput());
        const QString stderr_text = QString::fromUtf8(process.readAllStandardError());

        if (exit_code == 0) {
            summary.succeeded_files++;
            append_log(QStringLiteral("   [SUCESSO] QM gerado: %1").arg(trim_process_output(stdout_text)));
        } else {
            summary.failed_files++;
            append_log(QStringLiteral("   [FALHA] Codigo %1. Saida: %2 | Erro: %3")
                           .arg(QString::number(exit_code), trim_process_output(stdout_text),
                                trim_process_output(stderr_text)));
        }
    }

    return summary;
}

void TranslationCompileDialog::run_compilation()
{
    if (lrelease_executable_.isEmpty()) {
        QMessageBox::critical(this, QStringLiteral("Erro"), QStringLiteral("lrelease nao foi localizado no ambiente."));
        return;
    }

    const QString source_directory = effective_source_directory();
    const QString output_directory = effective_output_directory();

    if (source_directory.isEmpty() || !QDir(source_directory).exists()) {
        QMessageBox::warning(this, QStringLiteral("Aviso"), QStringLiteral("Pasta fonte de traducoes invalida ou ausente."));
        return;
    }

    compile_button_->setEnabled(false);
    const auto restore_button = qScopeGuard([this]() { compile_button_->setEnabled(true); });

    append_log(QStringLiteral("================================================================================"));
    append_log(QStringLiteral("[%1] Iniciando compilacao de traducoes")
                   .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))));
    append_log(QStringLiteral("Alvo: %1 (%2)").arg(current_target().display_name, current_target().id));
    append_log(QStringLiteral("Fonte (.ts): %1").arg(QDir::toNativeSeparators(source_directory)));
    append_log(QStringLiteral("Destino (.qm): %1").arg(QDir::toNativeSeparators(output_directory)));

    const CompileSummary summary = compile_current_target(source_directory, output_directory);

    append_log(QStringLiteral("Resumo: descobertos=%1, sucesso=%2, falhas=%3")
                   .arg(summary.discovered_files)
                   .arg(summary.succeeded_files)
                   .arg(summary.failed_files));
    append_log(QStringLiteral("================================================================================"));

    if (summary.has_failures()) {
        status_label_->setText(QStringLiteral("Status: Compilacao concluida com falhas."));
        status_label_->setStyleSheet(QStringLiteral("font-weight: bold; color: #D32F2F;"));
    } else {
        status_label_->setText(QStringLiteral("Status: Compilacao concluida com sucesso."));
        status_label_->setStyleSheet(QStringLiteral("font-weight: bold; color: #2E7D32;"));
    }
}

}  // namespace mocks::translations

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    const QString target_id = argument_target_id(app.arguments());
    mocks::translations::TranslationCompileDialog dialog(nullptr, target_id);
    dialog.show();
    return app.exec();
}
