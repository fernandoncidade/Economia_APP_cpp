#ifndef UI_27_SOBRE_DIALOG_HPP
#define UI_27_SOBRE_DIALOG_HPP

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QTextBrowser>
#include <QList>
#include <QTextCursor>

struct SobreDialogParams {
    QString titulo = "Sobre";
    QString texto_fixo;
    QString texto_history;
    QString detalhes;
    QString licencas;
    QString sites_licencas;
    QString show_history_text = "History";
    QString hide_history_text = "Hide history";
    QString show_details_text = "Details";
    QString hide_details_text = "Hide details";
    QString show_licenses_text = "Licenses";
    QString hide_licenses_text = "Hide licenses";
    QString ok_text = "OK";
    QString site_oficial_text = "Official site";
    QString avisos;
    QString show_notices_text = "Notices";
    QString hide_notices_text = "Hide notices";
    QString privacy_policy;
    QString show_privacy_policy_text = "Privacy Policy";
    QString hide_privacy_policy_text = "Hide privacy policy";
    QString info_not_available_text = "Information not available";
    QString release_notes;
    QString show_release_notes_text = "Release Notes";
    QString hide_release_notes_text = "Hide Release Notes";
};

struct AboutSearchState {
    QString query;
    QList<QTextCursor> matches;
    int current_index = -1;
};

class SobreDialog : public QDialog {
    Q_OBJECT

public:
    explicit SobreDialog(QWidget* parent, const SobreDialogParams& params);
    ~SobreDialog() override = default;

public slots:
    void retranslate_ui(const QString& codigo_idioma = QString());

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void update_tab_labels(int current_index);
    void on_search_text_changed(const QString& text);
    void on_search_return_pressed();
    void on_search_prev_clicked();
    void on_search_next_clicked();
    void on_tab_changed(int current_index);

private:
    void rebuild_search_current_tab();
    void navigate_search(int step);
    void update_search_highlights(QTextBrowser* viewer);
    void update_search_ui();
    bool has_any_matches() const;

    QWidget* m_app = nullptr;
    QLabel* m_fixedLabel = nullptr;
    QLabel* m_searchLabel = nullptr;
    QLineEdit* m_searchEdit = nullptr;
    QLabel* m_searchCountLabel = nullptr;
    QToolButton* m_searchPrevBtn = nullptr;
    QToolButton* m_searchNextBtn = nullptr;
    QTabWidget* m_tabs = nullptr;
    QList<QTextBrowser*> m_textBrowsers;
    QPushButton* m_okButton = nullptr;
    QStringList m_tabShowLabels;
    QStringList m_tabHideLabels;
    AboutSearchState m_searchState;
};

#endif // UI_27_SOBRE_DIALOG_HPP
