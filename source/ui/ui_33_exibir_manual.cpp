#include "ui_33_exibir_manual.hpp"
#include "ui_30_Manual.hpp"
#include "../utils/IconUtils.hpp"
#include "../utils/LogManager.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QListWidget>
#include <QTextBrowser>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPointer>
#include <QCoreApplication>
#include <QColor>
#include <QFont>
#include <QUrl>
#include <memory>

namespace {

static QPointer<QDialog> s_manual_dialog = nullptr;

QString manual_root_anchor()
{
    return QStringLiteral("manual-root");
}

QString manual_section_anchor(const QString &section_id)
{
    return QStringLiteral("manual-section-%1").arg(section_id);
}

void append_manual_line(QTextCursor &cursor, const QString &text, const QTextCharFormat &format = QTextCharFormat())
{
    if (format.isValid()) {
        cursor.insertText(text, format);
    } else {
        cursor.insertText(text);
    }
    cursor.insertBlock();
}

void render_manual_document(QTextBrowser *viewer, const QString &lang)
{
    if (viewer == nullptr || viewer->document() == nullptr) {
        return;
    }

    const QString normalized = source::ui::Manual::normalize_language(lang);
    const auto blocks_and_order = source::ui::Manual::get_manual_blocks(normalized);
    const QList<source::ui::Manual::ManualBlock> &blocks = blocks_and_order.first;

    QTextDocument *document = viewer->document();
    document->clear();

    QTextCursor cursor(document);
    cursor.beginEditBlock();

    const QColor accent_blue(QStringLiteral("#1d4ed8"));

    QTextCharFormat title_format;
    QFont title_font = viewer->font();
    title_font.setBold(true);
    title_font.setPointSize(title_font.pointSize() + 6);
    title_format.setFont(title_font);
    title_format.setAnchor(true);
    title_format.setAnchorNames({manual_root_anchor()});
    title_format.setForeground(accent_blue);

    QTextCharFormat toc_link_format;
    toc_link_format.setAnchor(true);
    toc_link_format.setForeground(accent_blue);
    toc_link_format.setFontUnderline(true);

    QTextCharFormat section_title_format;
    QFont section_title_font = viewer->font();
    section_title_font.setBold(true);
    section_title_font.setPointSize(section_title_font.pointSize() + 3);
    section_title_format.setFont(section_title_font);
    section_title_format.setAnchor(true);
    section_title_format.setForeground(accent_blue);
    section_title_format.setFontUnderline(true);

    QTextCharFormat detail_title_format;
    QFont detail_title_font = viewer->font();
    detail_title_font.setBold(true);
    detail_title_format.setFont(detail_title_font);

    QTextCharFormat body_format;
    body_format.setFont(viewer->font());

    bool title_rendered = false;
    for (const auto &block : blocks) {
        if (block.kind == QStringLiteral("blank")) {
            cursor.insertBlock();
            continue;
        }

        if (block.kind == QStringLiteral("line")) {
            if (!title_rendered) {
                append_manual_line(cursor, block.text, title_format);
                title_rendered = true;
            } else {
                append_manual_line(cursor, block.text, body_format);
            }
            continue;
        }

        if (block.kind == QStringLiteral("toc_title")) {
            append_manual_line(cursor, block.text, detail_title_format);
            continue;
        }

        if (block.kind == QStringLiteral("toc_item")) {
            QTextCharFormat link_format = toc_link_format;
            link_format.setAnchorHref(QStringLiteral("#%1").arg(manual_section_anchor(block.section_id)));
            append_manual_line(cursor, block.text, link_format);
            continue;
        }

        if (block.kind == QStringLiteral("section_title")) {
            QTextCharFormat link_format = section_title_format;
            link_format.setAnchorHref(QStringLiteral("#%1").arg(manual_root_anchor()));
            link_format.setAnchorNames({manual_section_anchor(block.section_id)});
            append_manual_line(cursor, block.text, link_format);
            continue;
        }

        if (block.kind == QStringLiteral("detail_title")) {
            append_manual_line(cursor, block.text, detail_title_format);
            continue;
        }

        if (block.kind == QStringLiteral("bullet")) {
            append_manual_line(cursor, QStringLiteral("• %1").arg(block.text), body_format);
            continue;
        }

        append_manual_line(cursor, block.text, body_format);
    }

    cursor.endEditBlock();

    QTextCursor reset_cursor(document);
    reset_cursor.movePosition(QTextCursor::Start);
    viewer->setTextCursor(reset_cursor);
    if (viewer->verticalScrollBar() != nullptr) {
        viewer->verticalScrollBar()->setValue(0);
    }
}

int find_manual_anchor_position(const QTextDocument *document, const QString &anchor_name)
{
    if (document == nullptr || anchor_name.isEmpty()) {
        return -1;
    }

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid()) {
                continue;
            }

            const QTextCharFormat format = fragment.charFormat();
            if (format.isAnchor() && format.anchorNames().contains(anchor_name)) {
                return fragment.position();
            }
        }
    }

    return -1;
}

void scroll_manual_viewer_to_position(QTextBrowser *viewer, int pos)
{
    if (viewer == nullptr || pos < 0 || viewer->document() == nullptr || viewer->verticalScrollBar() == nullptr) {
        return;
    }

    QTextCursor cursor(viewer->document());
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::StartOfBlock);
    viewer->setTextCursor(cursor);

    const QTextBlock block = cursor.block();
    if (!block.isValid()) {
        viewer->ensureCursorVisible();
        return;
    }

    const qreal block_top = viewer->document()->documentLayout()->blockBoundingRect(block).top();
    viewer->verticalScrollBar()->setValue(block_top <= 0.0 ? 0 : static_cast<int>(block_top));
}

void scroll_manual_viewer_to_anchor(QTextBrowser *viewer, const QString &anchor_name)
{
    if (viewer == nullptr || anchor_name.isEmpty()) {
        return;
    }

    const int pos = find_manual_anchor_position(viewer->document(), anchor_name);
    if (pos >= 0) {
        scroll_manual_viewer_to_position(viewer, pos);
        return;
    }

    viewer->scrollToAnchor(anchor_name);
}

int find_manual_row_for_anchor(QListWidget *list, const QString &anchor_name)
{
    if (list == nullptr || anchor_name.isEmpty()) {
        return -1;
    }

    for (int row = 0; row < list->count(); ++row) {
        QListWidgetItem *item = list->item(row);
        if (item != nullptr && item->data(Qt::UserRole).toString() == anchor_name) {
            return row;
        }
    }

    return -1;
}

struct ManualSearchState {
    QString query;
    QList<QTextCursor> matches;
    int current_index = -1;
};

void update_manual_search_highlights(QTextBrowser *viewer, const ManualSearchState &state)
{
    if (viewer == nullptr || viewer->document() == nullptr) {
        return;
    }

    QList<QTextEdit::ExtraSelection> selections;
    if (state.query.isEmpty()) {
        viewer->setExtraSelections(selections);
        return;
    }

    QColor highlight_color = viewer->palette().color(QPalette::Highlight);
    highlight_color.setAlpha(95);
    QColor current_color = viewer->palette().color(QPalette::Highlight);
    current_color.setAlpha(190);
    const QColor current_text_color = viewer->palette().color(QPalette::HighlightedText);

    for (int index = 0; index < state.matches.size(); ++index) {
        QTextEdit::ExtraSelection selection;
        selection.cursor = state.matches.at(index);
        selection.format.setBackground(index == state.current_index ? current_color : highlight_color);
        if (index == state.current_index) {
            selection.format.setForeground(current_text_color);
        }
        selections.append(selection);
    }

    viewer->setExtraSelections(selections);
}

void rebuild_manual_search(QTextBrowser *viewer, ManualSearchState &state, const QString &search_text)
{
    state.query = search_text.trimmed();
    state.matches.clear();
    state.current_index = -1;

    if (viewer == nullptr || viewer->document() == nullptr || state.query.isEmpty()) {
        update_manual_search_highlights(viewer, state);
        return;
    }

    QTextCursor cursor(viewer->document());
    while (true) {
        cursor = viewer->document()->find(state.query, cursor);
        if (cursor.isNull()) {
            break;
        }
        state.matches.append(cursor);
    }

    if (!state.matches.isEmpty()) {
        state.current_index = 0;
        scroll_manual_viewer_to_position(viewer, state.matches.first().selectionStart());
    }

    update_manual_search_highlights(viewer, state);
}

void navigate_manual_search(QTextBrowser *viewer, ManualSearchState &state, int step)
{
    if (viewer == nullptr || state.matches.isEmpty() || step == 0) {
        return;
    }

    if (state.current_index < 0) {
        state.current_index = step < 0 ? state.matches.size() - 1 : 0;
    } else {
        state.current_index = (state.current_index + step + state.matches.size()) % state.matches.size();
    }

    const QTextCursor cursor = state.matches.at(state.current_index);
    update_manual_search_highlights(viewer, state);
    viewer->setTextCursor(cursor);
    scroll_manual_viewer_to_position(viewer, cursor.selectionStart());
}

void update_manual_search_buttons(QLabel *count_label,
                                  QToolButton *previous_button,
                                  QToolButton *next_button,
                                  const ManualSearchState &state,
                                  bool is_en)
{
    const bool enabled = !state.matches.isEmpty();
    if (previous_button != nullptr) {
        previous_button->setEnabled(enabled);
    }
    if (next_button != nullptr) {
        next_button->setEnabled(enabled);
    }
    if (count_label != nullptr) {
        if (state.query.isEmpty()) {
            count_label->clear();
        } else if (state.matches.isEmpty()) {
            count_label->setText(is_en ? QStringLiteral("0 of 0") : QStringLiteral("0 de 0"));
        } else {
            count_label->setText(is_en ? QStringLiteral("%1 of %2").arg(state.current_index + 1).arg(state.matches.size())
                                       : QStringLiteral("%1 de %2").arg(state.current_index + 1).arg(state.matches.size()));
        }
    }
}

} // namespace

void exibir_manual(QWidget* app)
{
    if (s_manual_dialog != nullptr && s_manual_dialog->isVisible()) {
        if (s_manual_dialog->isMinimized()) {
            s_manual_dialog->showNormal();
        }
        s_manual_dialog->raise();
        s_manual_dialog->activateWindow();
        return;
    }

    auto *dialog = new QDialog(app);
    dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setWindowModality(Qt::NonModal);
    dialog->setModal(false);
    dialog->setMinimumSize(960, 650);
    dialog->resize(1050, 720);

    QString icon_path = get_icon_path("economia.ico");
    if (!icon_path.isEmpty()) {
        dialog->setWindowIcon(QIcon(icon_path));
    }

    auto *root = new QVBoxLayout(dialog);
    auto *search_layout = new QGridLayout();
    auto *search_label = new QLabel(dialog);
    auto *search_edit = new QLineEdit(dialog);
    auto *search_count_label = new QLabel(dialog);
    auto *search_previous_button = new QToolButton(dialog);
    auto *search_next_button = new QToolButton(dialog);
    auto *search_controls = new QHBoxLayout();

    search_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    search_edit->setClearButtonEnabled(true);
    search_count_label->setAlignment(Qt::AlignCenter);
    search_count_label->setStyleSheet(QStringLiteral("color: #64748b; font-weight: 500; padding: 0 4px;"));
    search_previous_button->setArrowType(Qt::LeftArrow);
    search_next_button->setArrowType(Qt::RightArrow);
    search_previous_button->setAutoRaise(true);
    search_next_button->setAutoRaise(true);
    search_previous_button->setEnabled(false);
    search_next_button->setEnabled(false);

    search_controls->setContentsMargins(0, 0, 0, 0);
    search_controls->setSpacing(4);
    search_controls->addWidget(search_edit, 1);
    search_controls->addWidget(search_count_label);
    search_controls->addWidget(search_previous_button);
    search_controls->addWidget(search_next_button);

    search_layout->setColumnStretch(0, 0);
    search_layout->setColumnStretch(1, 1);
    search_layout->addWidget(search_label, 0, 0);
    search_layout->addLayout(search_controls, 0, 1);
    root->addLayout(search_layout);

    auto *body = new QHBoxLayout();
    auto *list = new QListWidget(dialog);
    auto *viewer = new QTextBrowser(dialog);
    auto *close_button = new QPushButton(dialog);

    viewer->setReadOnly(true);
    viewer->setOpenLinks(false);
    viewer->setOpenExternalLinks(false);
    list->setMinimumWidth(310);

    body->addWidget(list, 0);
    body->addWidget(viewer, 1);
    root->addLayout(body, 1);

    auto *footer = new QHBoxLayout();
    footer->addStretch(1);
    close_button->setAutoDefault(false);
    close_button->setDefault(false);
    footer->addWidget(close_button);
    root->addLayout(footer);

    auto search_state = std::make_shared<ManualSearchState>();

    auto update_search = [search_label, search_count_label, search_previous_button, search_next_button, search_state]() {
        bool is_en = (search_label->text() == QStringLiteral("Search:"));
        update_manual_search_buttons(search_count_label, search_previous_button, search_next_button, *search_state, is_en);
    };

    QObject::connect(search_edit, &QLineEdit::textChanged, dialog, [viewer, search_state, update_search](const QString &text) {
        rebuild_manual_search(viewer, *search_state, text);
        update_search();
    });

    QObject::connect(search_edit, &QLineEdit::returnPressed, dialog, [viewer, search_state, update_search]() {
        navigate_manual_search(viewer, *search_state, 1);
        update_search();
    });

    QObject::connect(search_previous_button, &QToolButton::clicked, dialog, [viewer, search_state, update_search]() {
        navigate_manual_search(viewer, *search_state, -1);
        update_search();
    });

    QObject::connect(search_next_button, &QToolButton::clicked, dialog, [viewer, search_state, update_search]() {
        navigate_manual_search(viewer, *search_state, 1);
        update_search();
    });

    const auto render_manual = [app, dialog, list, viewer, close_button, search_label, search_edit, search_count_label, search_previous_button, search_next_button, search_state, update_search]() {
        int current_row = list->currentRow();
        if (current_row < 0) {
            current_row = 0;
        }

        QString idioma = QStringLiteral("pt_BR");
        if (app != nullptr) {
            auto *gm = app->findChild<GerenciadorTraducao*>();
            if (gm != nullptr) {
                idioma = gm->obter_idioma_atual();
            } else {
                idioma = GerenciadorTraducao::idioma_atual_global();
            }
        } else {
            idioma = GerenciadorTraducao::idioma_atual_global();
        }

        const QString normalized_lang = source::ui::Manual::normalize_language(idioma);
        const bool is_en = (normalized_lang == QStringLiteral("en_US"));

        const QList<source::ui::Manual::ManualSection> sections = source::ui::Manual::get_manual_document(normalized_lang);
        const QString manual_title = source::ui::Manual::get_manual_title(normalized_lang);

        dialog->setWindowTitle(manual_title);
        search_label->setText(is_en ? QStringLiteral("Search:") : QStringLiteral("Buscar:"));
        search_edit->setPlaceholderText(is_en ? QStringLiteral("Search text in manual...") : QStringLiteral("Buscar texto no manual..."));
        search_previous_button->setToolTip(is_en ? QStringLiteral("Previous match") : QStringLiteral("Resultado anterior"));
        search_next_button->setToolTip(is_en ? QStringLiteral("Next match") : QStringLiteral("Próximo resultado"));
        search_previous_button->setAccessibleName(search_previous_button->toolTip());
        search_next_button->setAccessibleName(search_next_button->toolTip());
        close_button->setText(is_en ? QStringLiteral("Close") : QStringLiteral("Fechar"));

        render_manual_document(viewer, normalized_lang);

        list->blockSignals(true);
        list->clear();
        auto *manual_item = new QListWidgetItem(manual_title, list);
        manual_item->setData(Qt::UserRole, manual_root_anchor());
        for (const auto &section : sections) {
            auto *section_item = new QListWidgetItem(section.title, list);
            section_item->setData(Qt::UserRole, manual_section_anchor(section.id));
        }
        list->blockSignals(false);

        QObject::disconnect(list, nullptr, dialog, nullptr);
        QObject::disconnect(viewer, nullptr, dialog, nullptr);

        const auto navigate_to_anchor = [list, viewer](const QString &anchor_name, bool sync_list_selection) {
            if (anchor_name.isEmpty()) {
                return;
            }

            scroll_manual_viewer_to_anchor(viewer, anchor_name);

            if (!sync_list_selection) {
                return;
            }

            const int row = find_manual_row_for_anchor(list, anchor_name);
            if (row < 0 || row == list->currentRow()) {
                return;
            }

            const QSignalBlocker blocker(list);
            list->setCurrentRow(row);
        };

        QObject::connect(list, &QListWidget::currentRowChanged, dialog, [list, navigate_to_anchor](int row) {
            QListWidgetItem *item = list->item(row);
            if (item == nullptr) {
                return;
            }

            navigate_to_anchor(item->data(Qt::UserRole).toString(), false);
        });

        QObject::connect(viewer, &QTextBrowser::anchorClicked, dialog, [navigate_to_anchor](const QUrl &url) {
            const QString anchor_name = url.fragment().isEmpty() ? url.toString(QUrl::FullyDecoded) : url.fragment();
            navigate_to_anchor(anchor_name, true);
        });

        if (list->count() > 0) {
            list->setCurrentRow(qBound(0, current_row, list->count() - 1));
        }
        rebuild_manual_search(viewer, *search_state, search_edit->text());
        update_search();
    };

    QObject::connect(close_button, &QPushButton::clicked, dialog, &QDialog::close);

    if (app != nullptr) {
        auto *gm = app->findChild<GerenciadorTraducao*>();
        if (gm != nullptr) {
            QObject::connect(gm, &GerenciadorTraducao::idioma_alterado, dialog, [render_manual](const QString &) {
                render_manual();
            });
        }
    }

    QObject::connect(dialog, &QObject::destroyed, [dialog]() {
        if (s_manual_dialog == dialog) {
            s_manual_dialog = nullptr;
        }
    });

    s_manual_dialog = dialog;
    render_manual();
    dialog->show();
}
