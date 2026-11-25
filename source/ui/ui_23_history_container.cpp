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
#include <QAbstractTextDocumentLayout>
#include <QtMath>
#include <QTableWidget>
#include <QHeaderView>

HistoryTextEdit::HistoryTextEdit(QWidget* parent)
    : QTextEdit(parent) {
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAcceptRichText(true);
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Plain);

    if (document() && document()->documentLayout()) {
        connect(document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged,
                this, [this]() {
                    adjust_content_height();
                });
    }
}

void HistoryTextEdit::adjust_content_height() {
    if (m_adjusting) return;
    m_adjusting = true;

    int avail_w = viewport() ? viewport()->width() : width();
    if (avail_w <= 20) {
        if (parentWidget() && parentWidget()->width() > 40) {
            avail_w = parentWidget()->width() - frameWidth() * 2 - contentsMargins().left() - contentsMargins().right() - 8;
        }
    }
    if (avail_w <= 20) {
        avail_w = 400;
    }

    if (document()) {
        if (qAbs(document()->textWidth() - avail_w) > 1.0) {
            document()->setTextWidth(avail_w);
        }

        int doc_h = qCeil(document()->size().height());
        int margins_h = frameWidth() * 2 + contentsMargins().top() + contentsMargins().bottom();
        int target_h = doc_h + margins_h + 10;
        if (target_h < 40) target_h = 40;

        if (height() != target_h || minimumHeight() != target_h || maximumHeight() != target_h) {
            setFixedHeight(target_h);
            updateGeometry();
        }
    }

    m_adjusting = false;
}

void HistoryTextEdit::resizeEvent(QResizeEvent* event) {
    QTextEdit::resizeEvent(event);
    if (event->oldSize().width() != event->size().width()) {
        adjust_content_height();
    }
}

void HistoryTextEdit::showEvent(QShowEvent* event) {
    QTextEdit::showEvent(event);
    adjust_content_height();
}

void HistoryTextEdit::wheelEvent(QWheelEvent* event) {
    event->ignore();
}

HistoryContainer::HistoryContainer(QWidget* parent)
    : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_scrollArea->verticalScrollBar()) {
        m_scrollArea->verticalScrollBar()->setSingleStep(25);
    }

    m_innerWidget = new QWidget(m_scrollArea);
    m_innerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_innerLayout = new QVBoxLayout(m_innerWidget);
    m_innerLayout->setContentsMargins(4, 4, 4, 4);
    m_innerLayout->setSpacing(12);

    m_bottomSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_innerLayout->addItem(m_bottomSpacer);

    m_innerWidget->setLayout(m_innerLayout);
    m_scrollArea->setWidget(m_innerWidget);
    mainLayout->addWidget(m_scrollArea);
    setLayout(mainLayout);
}

void HistoryContainer::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (event->oldSize().width() != event->size().width()) {
        for (const auto& e : m_entries) {
            if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
                hte->adjust_content_height();
            }
        }
    }
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
    QTimer::singleShot(80, this, [this]() {
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
                if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
                    hte->adjust_content_height();
                }
                return;
            }
        }

        auto* entry_w = new QWidget(m_innerWidget);
        entry_w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        auto* entry_layout = new QHBoxLayout(entry_w);
        entry_layout->setContentsMargins(4, 4, 4, 4);
        entry_layout->setSpacing(8);

        auto* chk = new QCheckBox(entry_w);
        chk->setToolTip(QCoreApplication::translate("App", "Marque para editar/excluir esta entrada"));
        chk->setFixedSize(20, 20);
        chk->setStyleSheet(
            "QCheckBox { spacing: 0px; padding: 0px; margin: 0px; }"
            "QCheckBox::indicator { width: 18px; height: 18px; }"
        );
        entry_layout->addWidget(chk, 0, Qt::AlignTop);

        auto* right_container = new QWidget(entry_w);
        right_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        auto* right_vlayout = new QVBoxLayout(right_container);
        right_vlayout->setContentsMargins(0, 0, 0, 0);
        right_vlayout->setSpacing(6);

        auto* te = new HistoryTextEdit(right_container);
        te->setReadOnly(true);
        te->setFont(font());
        te->setHtml(convert_to_html(formatted_text));
        MathRenderer::apply_radicals(te->document());
        te->setProperty("raw_text", formatted_text);
        te->adjust_content_height();
        right_vlayout->addWidget(te, 0);

        if (extra_widget) {
            extra_widget->setParent(right_container);
            extra_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            if (extra_widget->minimumHeight() < 120) {
                extra_widget->setMinimumHeight(120);
            }
            right_vlayout->addWidget(extra_widget, 0);
        }

        entry_layout->addWidget(right_container, 1);
        entry_w->setLayout(entry_layout);

        int insert_idx = m_innerLayout->count();
        if (m_bottomSpacer) {
            int spacer_idx = m_innerLayout->indexOf(m_bottomSpacer);
            if (spacer_idx >= 0) {
                insert_idx = spacer_idx;
            }
        }
        m_innerLayout->insertWidget(insert_idx, entry_w, 0);

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

        QTimer::singleShot(40, te, [te]() {
            te->adjust_content_height();
        });

        scroll_to_bottom();
        emit historyChanged();
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
                if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
                    hte->adjust_content_height();
                }
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
            if (e.container) {
                m_innerLayout->removeWidget(e.container);
                delete e.container;
            }
        }
        m_entries.clear();
        m_editingIndex.reset();
        if (m_scrollArea && m_scrollArea->verticalScrollBar()) {
            m_scrollArea->verticalScrollBar()->setValue(0);
        }
        emit historyChanged();
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
        if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
            hte->adjust_content_height();
        }
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
            if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
                hte->adjust_content_height();
            }
        }
        m_editingIndex.reset();
        emit historyChanged();
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
        bool deleted = false;
        for (int i = m_entries.size() - 1; i >= 0; --i) {
            if (m_entries[i].checkbox->isChecked()) {
                QWidget* w = m_entries[i].container;
                m_innerLayout->removeWidget(w);
                delete w;
                m_entries.removeAt(i);
                deleted = true;
                if (m_editingIndex.has_value()) {
                    if (i < m_editingIndex.value()) {
                        m_editingIndex = m_editingIndex.value() - 1;
                    } else if (i == m_editingIndex.value()) {
                        m_editingIndex.reset();
                    }
                }
            }
        }
        if (deleted) {
            emit historyChanged();
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
        if (e.textEdit) {
            e.textEdit->setFont(font);
            if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
                hte->adjust_content_height();
            }
        }
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
                if (auto* hte = qobject_cast<HistoryTextEdit*>(e.textEdit)) {
                    hte->adjust_content_height();
                }
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

bool HistoryContainer::isEmpty() const {
    return m_entries.isEmpty();
}

QJsonArray HistoryContainer::toJsonArray() const {
    QJsonArray arr;
    try {
        for (const auto& e : m_entries) {
            QJsonObject obj;
            QString text = e.raw_text;
            if (text.isEmpty() && e.textEdit) {
                text = e.textEdit->property("raw_text").toString();
                if (text.isEmpty()) {
                    text = e.textEdit->toPlainText();
                }
            }
            obj["raw_text"] = text;
            obj["current_lang"] = e.current_lang;
            obj["user_edited"] = e.user_edited;

            if (e.extraWidget) {
                if (auto* tw = qobject_cast<QTableWidget*>(e.extraWidget)) {
                    QJsonObject tbl;
                    int cols = tw->columnCount();
                    int rows = tw->rowCount();
                    tbl["cols"] = cols;
                    tbl["rows"] = rows;

                    QJsonArray headers;
                    QJsonArray userRoles;
                    for (int c = 0; c < cols; ++c) {
                        auto* hi = tw->horizontalHeaderItem(c);
                        headers.append(hi ? hi->text() : "");
                        userRoles.append(hi ? hi->data(Qt::UserRole).toString() : "");
                    }
                    tbl["headers"] = headers;
                    tbl["user_roles"] = userRoles;

                    QJsonArray rowsData;
                    for (int r = 0; r < rows; ++r) {
                        QJsonArray rowCells;
                        for (int c = 0; c < cols; ++c) {
                            auto* it = tw->item(r, c);
                            rowCells.append(it ? it->text() : "");
                        }
                        rowsData.append(rowCells);
                    }
                    tbl["data"] = rowsData;
                    obj["table"] = tbl;
                }
            }
            arr.append(obj);
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao serializar HistoryContainer para JSON: %1").arg(e.what()));
    }
    return arr;
}

void HistoryContainer::loadFromJsonArray(const QJsonArray& arr) {
    try {
        const bool wasBlocked = blockSignals(true);
        clear();

        for (const auto& val : arr) {
            if (!val.isObject()) continue;
            QJsonObject obj = val.toObject();
            QString text = obj["raw_text"].toString();
            QString lang = obj["current_lang"].toString();
            bool edited = obj["user_edited"].toBool();

            QWidget* extra_w = nullptr;
            if (obj.contains("table")) {
                QJsonObject tbl = obj["table"].toObject();
                int cols = tbl["cols"].toInt();
                int rows = tbl["rows"].toInt();
                QJsonArray headers = tbl["headers"].toArray();
                QJsonArray userRoles = tbl["user_roles"].toArray();
                QJsonArray rowsData = tbl["data"].toArray();

                auto* tw = new QTableWidget();
                tw->setColumnCount(cols);
                tw->setRowCount(rows);
                for (int c = 0; c < cols; ++c) {
                    QString hText = (c < headers.size()) ? headers.at(c).toString() : QString();
                    QString uRole = (c < userRoles.size()) ? userRoles.at(c).toString() : QString();
                    auto* hi = new QTableWidgetItem(hText);
                    if (!uRole.isEmpty()) {
                        hi->setData(Qt::UserRole, uRole);
                    }
                    tw->setHorizontalHeaderItem(c, hi);
                    if (tw->horizontalHeader()) {
                        tw->horizontalHeader()->setSectionResizeMode(c, QHeaderView::ResizeMode::Stretch);
                    }
                }
                for (int r = 0; r < rows; ++r) {
                    QJsonArray rowCells = (r < rowsData.size()) ? rowsData.at(r).toArray() : QJsonArray();
                    for (int c = 0; c < cols; ++c) {
                        QString cellText = (c < rowCells.size()) ? rowCells.at(c).toString() : QString();
                        auto* it = new QTableWidgetItem(cellText);
                        it->setFlags(it->flags() & ~Qt::ItemFlag::ItemIsEditable);
                        tw->setItem(r, c, it);
                    }
                }
                int table_h = std::min(350, std::max(120, (rows + 1) * 26 + 30));
                tw->setFixedHeight(table_h);
                extra_w = tw;
            }

            append(text, extra_w);
            if (!m_entries.isEmpty()) {
                m_entries.last().current_lang = lang;
                m_entries.last().user_edited = edited;
            }
        }
        blockSignals(wasBlocked);
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao carregar HistoryContainer de JSON: %1").arg(e.what()));
    }
}

QString HistoryContainer::toExportHtml() const {
    try {
        QStringList html_parts;
        for (const auto& e : m_entries) {
            QString raw = e.raw_text.trimmed();
            if (raw.isEmpty() && e.textEdit) {
                raw = e.textEdit->toPlainText().trimmed();
            }
            if (!raw.isEmpty()) {
                QString rich = TextFormat::to_rich_html(raw);
                if (rich.contains("<table", Qt::CaseInsensitive)) {
                    rich.replace("<table", "</pre><table", Qt::CaseInsensitive);
                    rich.replace("</table>", "</table><pre class=\"calc\">", Qt::CaseInsensitive);
                }
                html_parts << QString("<pre class=\"calc\">%1</pre>").arg(rich);
            }

            if (e.extraWidget) {
                if (auto* tw = qobject_cast<QTableWidget*>(e.extraWidget)) {
                    int cols = tw->columnCount();
                    int rows = tw->rowCount();
                    QStringList headers;
                    for (int c = 0; c < cols; ++c) {
                        auto* hi = tw->horizontalHeaderItem(c);
                        QString hText = hi ? hi->text() : QString("Col %1").arg(c + 1);
                        if (hi && !hi->data(Qt::UserRole).toString().isEmpty()) {
                            hText = QCoreApplication::translate("App", hi->data(Qt::UserRole).toString().toUtf8().constData());
                        }
                        headers << hText.toHtmlEscaped();
                    }
                    QStringList body_rows;
                    for (int r = 0; r < rows; ++r) {
                        QString row_str = "<tr>";
                        for (int c = 0; c < cols; ++c) {
                            auto* it = tw->item(r, c);
                            row_str += QString("<td style=\"border: 1px solid #444; padding: 4px 6px; text-align: left;\">%1</td>").arg(it ? it->text().toHtmlEscaped() : "");
                        }
                        row_str += "</tr>";
                        body_rows << row_str;
                    }
                    QString table_html = QString(
                        "<table style=\"border-collapse: collapse; width: 100%; font-size: 10pt; margin: 10px 0; table-layout: fixed;\">\n"
                        "<thead><tr style=\"background: #f0f0f0;\">\n"
                        "<th style=\"border: 1px solid #444; padding: 4px 6px; text-align: left;\">%1</th>\n"
                        "</tr></thead>\n"
                        "<tbody>\n"
                        "%2\n"
                        "</tbody></table>\n"
                    ).arg(headers.join("</th><th style=\"border: 1px solid #444; padding: 4px 6px; text-align: left;\">"), body_rows.join("\n"));
                    html_parts << table_html;
                }
            }
        }
        return html_parts.join("\n");
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao gerar HTML de exportação do HistoryContainer: %1").arg(e.what()));
        return QString();
    }
}


