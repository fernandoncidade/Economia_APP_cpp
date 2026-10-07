#include "ui_24_font_config_dialog.hpp"
#include "../utils/FontManager.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/DialogHelper.hpp"
#include "ui_23_history_container.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QCoreApplication>
#include <QMessageBox>

FontConfigDialog::FontConfigDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(QCoreApplication::translate("App", "Configuração de Fontes dos Resultados"));
    resize(500, 420);

    auto* mainLayout = new QVBoxLayout(this);

    auto* formGroupBox = new QGroupBox(QCoreApplication::translate("App", "Opções de Fonte"), this);
    auto* formLayout = new QFormLayout(formGroupBox);

    m_fontFamily = new QFontComboBox(this);
    m_fontFamily->setFontFilters(QFontComboBox::MonospacedFonts);
    formLayout->addRow(QCoreApplication::translate("App", "Família da Fonte:"), m_fontFamily);

    m_fontSize = new QSpinBox(this);
    m_fontSize->setRange(6, 32);
    m_fontSize->setValue(10);
    formLayout->addRow(QCoreApplication::translate("App", "Tamanho:"), m_fontSize);

    auto* styleWidget = new QWidget(this);
    auto* styleLayout = new QHBoxLayout(styleWidget);
    styleLayout->setContentsMargins(0, 0, 0, 0);

    m_bold = new QCheckBox(QCoreApplication::translate("App", "Negrito"), this);
    m_italic = new QCheckBox(QCoreApplication::translate("App", "Itálico"), this);
    m_underline = new QCheckBox(QCoreApplication::translate("App", "Sublinhado"), this);

    styleLayout->addWidget(m_bold);
    styleLayout->addWidget(m_italic);
    styleLayout->addWidget(m_underline);
    formLayout->addRow(QCoreApplication::translate("App", "Estilo:"), styleWidget);

    mainLayout->addWidget(formGroupBox);

    auto* previewGroupBox = new QGroupBox(QCoreApplication::translate("App", "Pré-visualização"), this);
    auto* previewLayout = new QVBoxLayout(previewGroupBox);

    m_preview = new QTextEdit(this);
    m_preview->setReadOnly(true);
    previewLayout->addWidget(m_preview);
    mainLayout->addWidget(previewGroupBox);

    auto* btnLayout = new QHBoxLayout();
    m_resetButton = new QPushButton(QCoreApplication::translate("App", "Restaurar Padrões"), this);
    btnLayout->addWidget(m_resetButton);
    btnLayout->addStretch(1);

    m_saveButton = new QPushButton(QCoreApplication::translate("App", "Salvar"), this);
    m_cancelButton = new QPushButton(QCoreApplication::translate("App", "Cancelar"), this);
    btnLayout->addWidget(m_saveButton);
    btnLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(btnLayout);

    connect(m_fontFamily, &QFontComboBox::currentFontChanged, this, &FontConfigDialog::update_preview);
    connect(m_fontSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &FontConfigDialog::update_preview);
    connect(m_bold, &QCheckBox::toggled, this, &FontConfigDialog::update_preview);
    connect(m_italic, &QCheckBox::toggled, this, &FontConfigDialog::update_preview);
    connect(m_underline, &QCheckBox::toggled, this, &FontConfigDialog::update_preview);

    connect(m_saveButton, &QPushButton::clicked, this, &FontConfigDialog::on_save);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_resetButton, &QPushButton::clicked, this, &FontConfigDialog::on_reset);

    load_current_config();
}

void FontConfigDialog::load_current_config() {
    FontConfig cfg = FontManager::get_config();
    m_fontFamily->setCurrentFont(QFont(cfg.family));
    m_fontSize->setValue(cfg.size);
    m_bold->setChecked(cfg.bold);
    m_italic->setChecked(cfg.italic);
    m_underline->setChecked(cfg.underline);
    update_preview();
}

void FontConfigDialog::update_preview() {
    QString family = m_fontFamily->currentFont().family();
    int size = m_fontSize->value();
    QString weight = m_bold->isChecked() ? "bold" : "normal";
    QString style = m_italic->isChecked() ? "italic" : "normal";
    QString decor = m_underline->isChecked() ? "underline" : "none";

    QString html = QString(
        "<html><head><style>"
        "body { font-family: '%1', monospace; font-size: %2pt; font-weight: %3; font-style: %4; text-decoration: %5; white-space: pre-wrap; margin: 4px; }"
        "</style></head><body>"
        "<pre>J = P × i × n\n"
        "J = 10.000,00 × 0,05 × 12\n"
        "J = R$ 6.000,00\n\n"
        "M = P + J = R$ 16.000,00</pre>"
        "</body></html>"
    ).arg(family).arg(size).arg(weight, style, decor);

    m_preview->setHtml(html);
}

void FontConfigDialog::on_save() {
    FontConfig cfg;
    cfg.family = m_fontFamily->currentFont().family();
    cfg.size = m_fontSize->value();
    cfg.bold = m_bold->isChecked();
    cfg.italic = m_italic->isChecked();
    cfg.underline = m_underline->isChecked();

    if (FontManager::save_config(cfg)) {
        if (parentWidget()) {
            QList<HistoryContainer*> containers = parentWidget()->findChildren<HistoryContainer*>();
            for (auto* hc : containers) {
                hc->refresh_all_fonts();
            }
        }
        accept();
    } else {
        DialogHelper::warning(this, QCoreApplication::translate("App", "Erro"),
                             QCoreApplication::translate("App", "Erro ao salvar configuração de fontes."));
    }
}

void FontConfigDialog::on_reset() {
    FontConfig def = FontManager::default_config();
    m_fontFamily->setCurrentFont(QFont(def.family));
    m_fontSize->setValue(def.size);
    m_bold->setChecked(def.bold);
    m_italic->setChecked(def.italic);
    m_underline->setChecked(def.underline);
    update_preview();
}
