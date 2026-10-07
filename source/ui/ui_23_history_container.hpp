#ifndef UI_23_HISTORY_CONTAINER_HPP
#define UI_23_HISTORY_CONTAINER_HPP

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QFont>
#include <QList>
#include <optional>
#include <functional>

struct HistoryEntry {
    QWidget* container = nullptr;
    QCheckBox* checkbox = nullptr;
    QTextEdit* textEdit = nullptr;
    QWidget* extraWidget = nullptr;
    std::function<QString()> retranslate_fn = nullptr;
    QString raw_text;
    QString current_lang = "pt_BR";
    bool user_edited = false;
};

class HistoryContainer : public QWidget {
    Q_OBJECT

public:
    explicit HistoryContainer(QWidget* parent = nullptr);
    ~HistoryContainer() override = default;

    void append(const QString& text, QWidget* extra_widget = nullptr, std::function<QString()> retranslate_fn = nullptr);
    void refresh_all_fonts();
    void clear();
    QString toPlainText() const;
    QString toRawText() const;
    void setReadOnly(bool value);

    QList<int> get_selected_indices() const;
    bool edit_selected();
    bool commit_edit();
    bool cancel_edit();
    void delete_selected();
    bool is_editing() const;

    void setFont(const QFont& font);
    void retranslate_entries(const QString& target_language);

    int count() const;
    QString get_entry_text(int index) const;

private:
    QString convert_to_html(const QString& text) const;
    void scroll_to_bottom();

    QScrollArea* m_scrollArea = nullptr;
    QWidget* m_innerWidget = nullptr;
    QVBoxLayout* m_innerLayout = nullptr;
    QList<HistoryEntry> m_entries;
    std::optional<int> m_editingIndex;
};

#endif // UI_23_HISTORY_CONTAINER_HPP
