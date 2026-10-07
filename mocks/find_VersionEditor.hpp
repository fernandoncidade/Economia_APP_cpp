#pragma once

#include <QList>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QWidget>

class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTabWidget;

namespace mocks::version_editor {

struct FileSpec {
    QString key;
    QString relative_path;
    QString content_key;
    QString group;
};

struct FileStatus {
    QString key;
    QString group;
    QString relative_path;
    QString path;
    bool exists = false;
    QStringList found;
    QStringList missing;
};

struct UpdateResult {
    QString key;
    QString group;
    QString relative_path;
    bool ok = false;
    QString message;
};

struct UpdateContext {
    QString pt_version;
    QString pt_date;
    QString en_version;
    QString en_date;
    QString pt_readme_version;
    QString en_readme_version;
};

/// Retorna a lista de especificações de todos os arquivos monitorados e atualizados pelo
/// VersionEditor, incluindo os documentos contratuais, de privacidade, avisos de copyright,
/// telas Sobre (ui_27_SobreDialog.cpp e ui_28_exibir_sobre.cpp), documentações do aplicativo
/// Economia_APP e as versões do README.md (notas de versão de release excluídas).
const QList<FileSpec> &file_specs();
QList<FileStatus> check_expected_lines(const QString &base_dir);
QList<UpdateResult> apply_updates(const UpdateContext &context, const QString &base_dir);
bool apply_line_updates(const QString &key, QStringList *lines, const UpdateContext &context);
QString detect_project_root();
QString normalize_dir(const QString &path);

class VersionEditorWidget : public QWidget {
public:
    explicit VersionEditorWidget(QWidget *parent = nullptr);

private:
    void selectBaseDirectory();
    void checkFiles();
    void saveChanges();
    void setStatusLines(const QStringList &lines);
    void setStatusGroups(const QList<QPair<QString, QStringList>> &groups);

    QString selected_base_dir_;

    QLineEdit *folder_path_ = nullptr;

    QLineEdit *pt_version_ = nullptr;
    QSpinBox *pt_day_ = nullptr;
    QComboBox *pt_month_ = nullptr;
    QSpinBox *pt_year_ = nullptr;

    QLineEdit *en_version_ = nullptr;
    QSpinBox *en_day_ = nullptr;
    QComboBox *en_month_ = nullptr;
    QSpinBox *en_year_ = nullptr;

    QPushButton *select_folder_btn_ = nullptr;
    QPushButton *check_btn_ = nullptr;
    QPushButton *save_btn_ = nullptr;
    QPushButton *refresh_btn_ = nullptr;

    QTabWidget *status_tabs_ = nullptr;
};

}  // namespace mocks::version_editor
