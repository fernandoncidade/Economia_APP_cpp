#include "ui_23_history_container.hpp"
#include "../utils/FontManager.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"
#include "../utils/MathRenderer.hpp"

#include <QScrollBar>
#include <QTimer>
#include <QCoreApplication>
#include <QSizePolicy>

HistoryContainer::HistoryContainer(QWidget* parent)
    : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_innerWidget = new QWidget(m_scrollArea);
    m_innerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_innerLayout = new QVBoxLayout(m_innerWidget);
    m_innerLayout->setContentsMargins(2, 2, 2, 2);
    m_innerLayout->setSpacing(8);

    m_innerWidget->setLayout(m_innerLayout);
    m_scrollArea->setWidget(m_innerWidget);
    mainLayout->addWidget(m_scrollArea);
    setLayout(mainLayout);
}

QString HistoryContainer::convert_to_html(const QString& text) const {
    QString font_css = FontManager::get_html_style();
    QString rich_body = TextFormat::to_rich_html(text);

    // If rich_body contains HTML tables (e.g. mathematical radicals), break out of <pre>
    if (rich_body.contains("<table", Qt::CaseInsensitive)) {
        rich_body.replace("<table", "</pre><table", Qt::CaseInsensitive);
        rich_body.replace("</table>", "</table><pre class=\"calc\">", Qt::CaseInsensitive);
    }

    return QString(
        "<html>\n"
        "<head>\n"
        "    <meta charset=\"utf-8\">\n"
        "    %1\n"
        "    <style>\n"
        "        body { margin: 0; padding: 4px; }\n"
        "        pre.calc { margin: 0; padding: 0; white-space: pre-wrap; word-break: break-word; }\n"
        "        sub { font-size: 75%; vertical-align: sub; }\n"
        "        sup { font-size: 75%; vertical-align: super; }\n"
        "    </style>\n"
        "</head>\n"
        "<body>\n"
        "    <pre class=\"calc\">%2</pre>\n"
        "</body>\n"
        "</html>\n"
    ).arg(font_css, rich_body);
}

void HistoryContainer::scroll_to_bottom() {
    QTimer::singleShot(50, this, [this]() {
        if (m_scrollArea && m_scrollArea->verticalScrollBar()) {
            m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
        }
    });
}

void HistoryContainer::append(const QString& text, QWidget* extra_widget, std::function<QString()> retranslate_fn) {
    try {
        QString formatted_text = TextFormat::format_sub_superscripts(text);
        if (m_editingIndex.has_value()) {
            int idx = m_editingIndex.value();
            if (idx >= 0 && idx < m_entries.size()) {
                auto& e = m_entries[idx];
                e.textEdit->setPlainText(formatted_text);
                e.textEdit->setProperty("raw_text", formatted_text);
                e.raw_text = formatted_text;
                e.user_edited = true;
                e.textEdit->setReadOnly(false);
                e.textEdit->setFocus();
                return;
            }
        }

        auto* entry_w = new QWidget(m_innerWidget);
        entry_w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto* entry_layout = new QHBoxLayout(entry_w);
        entry_layout->setContentsMargins(2, 2, 2, 2);
        entry_layout->setSpacing(6);

        auto* chk = new QCheckBox(entry_w);
        chk->setToolTip(QCoreApplication::translate("App", "Marque para editar/excluir esta entrada"));
        chk->setFixedSize(20, 20);
        chk->setStyleSheet(
            "QCheckBox { spacing: 0px; padding: 0px; margin: 0px; }"
            "QCheckBox::indicator { width: 18px; height: 18px; }"
        );
        entry_layout->addWidget(chk, 0, Qt::AlignTop);

        auto* right_container = new QWidget(entry_w);
        right_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        auto* right_vlayout = new QVBoxLayout(right_container);
        right_vlayout->setContentsMargins(0, 0, 0, 0);
        right_vlayout->setSpacing(6);

        auto* te = new QTextEdit(right_container);
        te->setReadOnly(true);
        te->setHtml(convert_to_html(formatted_text));
        MathRenderer::apply_radicals(te->document());
        te->setProperty("raw_text", formatted_text);
        te->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        te->setMinimumHeight(120);
        te->setAcceptRichText(true);
        right_vlayout->addWidget(te, 1);

        if (extra_widget) {
            extra_widget->setParent(right_container);
            extra_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            extra_widget->setMinimumHeight(120);
            right_vlayout->addWidget(extra_widget, 1);
        }

        entry_layout->addWidget(right_container, 1);
        entry_w->setLayout(entry_layout);

        // Add entry_w with stretch factor 1 to proportionately share layout height
        m_innerLayout->addWidget(entry_w, 1);

        HistoryEntry entry;
        entry.container = entry_w;
        entry.checkbox = chk;
        entry.textEdit = te;
        entry.extraWidget = extra_widget;
        entry.retranslate_fn = std::move(retranslate_fn);
        entry.raw_text = formatted_text;
        entry.current_lang = GerenciadorTraducao::idioma_atual_global();
        entry.user_edited = false;
        m_entries.append(entry);

        scroll_to_bottom();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao adicionar entrada no HistoryContainer: %1").arg(e.what()));
    }
}

void HistoryContainer::refresh_all_fonts() {
    try {
        for (const auto& e : m_entries) {
            QString raw = e.textEdit->property("raw_text").toString();
            if (raw.isEmpty()) {
                raw = e.textEdit->toPlainText();
            }
            if (!raw.isEmpty()) {
                e.textEdit->setHtml(convert_to_html(raw));
                MathRenderer::apply_radicals(e.textEdit->document());
            }
        }
        LogManager::info("Fontes de todas as entradas atualizadas");
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao atualizar fontes: %1").arg(e.what()));
    }
}

void HistoryContainer::clear() {
    try {
        for (const auto& e : m_entries) {
            m_innerLayout->removeWidget(e.container);
            delete e.container;
        }
        m_entries.clear();
        m_editingIndex.reset();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao limpar HistoryContainer: %1").arg(e.what()));
    }
}

QString HistoryContainer::toPlainText() const {
    try {
        QStringList parts;
        for (const auto& e : m_entries) {
            QString txt = e.raw_text.trimmed();
            if (txt.isEmpty()) {
                txt = e.textEdit->property("raw_text").toString().trimmed();
            }
            if (txt.isEmpty()) {
                txt = e.textEdit->toPlainText().trimmed();
            }
            if (!txt.isEmpty()) {
                parts << MathRenderer::to_plain_text(txt);
            }
        }
        return parts.join("\n\n");
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao obter texto do HistoryContainer: %1").arg(e.what()));
        return QString();
    }
}

QString HistoryContainer::toRawText() const {
    try {
        QStringList parts;
        for (const auto& e : m_entries) {
            QString txt = e.raw_text.trimmed();
            if (txt.isEmpty()) {
                txt = e.textEdit->property("raw_text").toString().trimmed();
            }
            if (txt.isEmpty()) {
                txt = e.textEdit->toPlainText().trimmed();
            }
            if (!txt.isEmpty()) {
                parts << txt;
            }
        }
        return parts.join("\n\n");
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao obter raw text do HistoryContainer: %1").arg(e.what()));
        return QString();
    }
}

void HistoryContainer::setReadOnly(bool value) {
    for (const auto& e : m_entries) {
        e.textEdit->setReadOnly(value);
    }
}

QList<int> HistoryContainer::get_selected_indices() const {
    QList<int> indices;
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries[i].checkbox->isChecked()) {
            indices.append(i);
        }
    }
    return indices;
}

bool HistoryContainer::edit_selected() {
    try {
        auto selected = get_selected_indices();
        if (selected.size() != 1) {
            return false;
        }
        int idx = selected[0];
        auto& e = m_entries[idx];
        QString raw = e.textEdit->property("raw_text").toString();
        if (raw.isEmpty()) raw = e.textEdit->toPlainText();
        e.textEdit->setPlainText(raw);
        m_editingIndex = idx;
        e.textEdit->setReadOnly(false);
        e.textEdit->setFocus();
        return true;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao iniciar edição no HistoryContainer: %1").arg(e.what()));
        return false;
    }
}

bool HistoryContainer::commit_edit() {
    try {
        if (!m_editingIndex.has_value()) {
            return false;
        }
        int idx = m_editingIndex.value();
        if (idx >= 0 && idx < m_entries.size()) {
            auto& e = m_entries[idx];
            QString current = TextFormat::format_sub_superscripts(e.textEdit->toPlainText());
            e.textEdit->setProperty("raw_text", current);
            e.raw_text = current;
            e.user_edited = true;
            e.textEdit->setHtml(convert_to_html(current));
            MathRenderer::apply_radicals(e.textEdit->document());
            e.textEdit->setReadOnly(true);
            e.checkbox->setChecked(false);
        }
        m_editingIndex.reset();
        return true;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao confirmar edição no HistoryContainer: %1").arg(e.what()));
        return false;
    }
}

bool HistoryContainer::cancel_edit() {
    return commit_edit();
}

void HistoryContainer::delete_selected() {
    try {
        for (int i = m_entries.size() - 1; i >= 0; --i) {
            if (m_entries[i].checkbox->isChecked()) {
                QWidget* w = m_entries[i].container;
                m_innerLayout->removeWidget(w);
                delete w;
                m_entries.removeAt(i);
                if (m_editingIndex.has_value()) {
                    if (i < m_editingIndex.value()) {
                        m_editingIndex = m_editingIndex.value() - 1;
                    } else if (i == m_editingIndex.value()) {
                        m_editingIndex.reset();
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao deletar entradas no HistoryContainer: %1").arg(e.what()));
    }
}

bool HistoryContainer::is_editing() const {
    return m_editingIndex.has_value();
}

void HistoryContainer::setFont(const QFont& font) {
    QWidget::setFont(font);
    for (const auto& e : m_entries) {
        e.textEdit->setFont(font);
    }
}

void HistoryContainer::retranslate_entries(const QString& target_language) {
    try {
        for (auto& e : m_entries) {
            if (e.checkbox) {
                e.checkbox->setToolTip(QCoreApplication::translate("App", "Marque para editar/excluir esta entrada"));
            }

            if (e.current_lang == target_language && !e.user_edited) {
                continue;
            }

            QString new_text;
            if (e.retranslate_fn && !e.user_edited) {
                new_text = e.retranslate_fn();
            } else {
                QString current = e.textEdit->property("raw_text").toString();
                if (current.isEmpty()) {
                    current = e.raw_text.isEmpty() ? e.textEdit->toPlainText() : e.raw_text;
                }
                new_text = GerenciadorTraducao::traduzir_texto(current, target_language);
            }

            if (!new_text.isEmpty()) {
                new_text = TextFormat::format_sub_superscripts(new_text);
                e.raw_text = new_text;
                e.current_lang = target_language;
                e.textEdit->setProperty("raw_text", new_text);
                e.textEdit->setHtml(convert_to_html(new_text));
                MathRenderer::apply_radicals(e.textEdit->document());
            }
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao retraduzir entradas no HistoryContainer: %1").arg(e.what()));
    }
}

int HistoryContainer::count() const {
    return m_entries.size();
}

QString HistoryContainer::get_entry_text(int index) const {
    if (index >= 0 && index < m_entries.size() && m_entries[index].textEdit) {
        return m_entries[index].textEdit->toPlainText();
    }
    return QString();
}

