#ifndef UI_24_FONT_CONFIG_DIALOG_HPP
#define UI_24_FONT_CONFIG_DIALOG_HPP

#include <QDialog>
#include <QFontComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QPushButton>

class FontConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit FontConfigDialog(QWidget* parent = nullptr);
    ~FontConfigDialog() override = default;

private slots:
    void update_preview();
    void on_save();
    void on_reset();

private:
    void load_current_config();

    QFontComboBox* m_fontFamily = nullptr;
    QSpinBox* m_fontSize = nullptr;
    QCheckBox* m_bold = nullptr;
    QCheckBox* m_italic = nullptr;
    QCheckBox* m_underline = nullptr;
    QTextEdit* m_preview = nullptr;

    QPushButton* m_saveButton = nullptr;
    QPushButton* m_resetButton = nullptr;
    QPushButton* m_cancelButton = nullptr;
};

#endif // UI_24_FONT_CONFIG_DIALOG_HPP
