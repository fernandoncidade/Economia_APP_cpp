#include "ui_27_SobreDialog.hpp"
#include "ui_29_opcoes_sobre.hpp"
#include "../utils/IconUtils.hpp"
#include "../utils/LogManager.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QTextBrowser>
#include <QIcon>
#include <QSizePolicy>
#include <QEvent>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QPalette>
#include <QColor>
#include <QScrollBar>

SobreDialog::SobreDialog(QWidget* parent, const SobreDialogParams& params)
    : QDialog(parent), m_app(parent) {
    try {
        setWindowTitle(params.titulo);
        setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        setAttribute(Qt::WA_DeleteOnClose, true);
        setWindowModality(Qt::NonModal);
        setModal(false);
        setMinimumSize(640, 480);
        resize(920, 600);

        QString icon_path = get_icon_path("economia.ico");
        if (!icon_path.isEmpty()) {
            setWindowIcon(QIcon(icon_path));
        }

        auto* layout = new QVBoxLayout(this);

        m_fixedLabel = new QLabel(this);
        m_fixedLabel->setOpenExternalLinks(true);
        m_fixedLabel->setTextFormat(Qt::RichText);
        m_fixedLabel->setWordWrap(true);
        if (!params.texto_fixo.isEmpty()) {
            m_fixedLabel->setText(params.texto_fixo);
        }
        layout->addWidget(m_fixedLabel);

        // Search Bar Controls
        auto* search_layout = new QGridLayout();
        m_searchLabel = new QLabel(this);
        m_searchLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        m_searchEdit = new QLineEdit(this);
        m_searchEdit->setClearButtonEnabled(true);

        m_searchCountLabel = new QLabel(this);
        m_searchCountLabel->setAlignment(Qt::AlignCenter);
        m_searchCountLabel->setStyleSheet(QStringLiteral("color: #64748b; font-weight: 500; padding: 0 4px;"));

        m_searchPrevBtn = new QToolButton(this);
        m_searchNextBtn = new QToolButton(this);
        m_searchPrevBtn->setArrowType(Qt::LeftArrow);
        m_searchNextBtn->setArrowType(Qt::RightArrow);
        m_searchPrevBtn->setAutoRaise(true);
        m_searchNextBtn->setAutoRaise(true);
        m_searchPrevBtn->setEnabled(false);
        m_searchNextBtn->setEnabled(false);

        auto* search_controls = new QHBoxLayout();
        search_controls->setContentsMargins(0, 0, 0, 0);
        search_controls->setSpacing(4);
        search_controls->addWidget(m_searchEdit, 1);
        search_controls->addWidget(m_searchCountLabel);
        search_controls->addWidget(m_searchPrevBtn);
        search_controls->addWidget(m_searchNextBtn);

        search_layout->setColumnStretch(0, 0);
        search_layout->setColumnStretch(1, 1);
        search_layout->addWidget(m_searchLabel, 0, 0);
        search_layout->addLayout(search_controls, 0, 1);
        layout->addLayout(search_layout);

        m_tabs = new QTabWidget(this);

        auto create_text_browser = [this](const QString& text) -> QTextBrowser* {
            auto* tb = new QTextBrowser(this);
            tb->setReadOnly(true);
            tb->setOpenExternalLinks(true);
            tb->setPlainText(text);
            return tb;
        };

        // 1. History
        auto* tb_history = create_text_browser(params.texto_history.isEmpty() ? params.info_not_available_text : params.texto_history);
        m_textBrowsers.append(tb_history);
        m_tabs->addTab(tb_history, params.show_history_text);

        // 2. Details
        auto* tb_details = create_text_browser(params.detalhes.isEmpty() ? params.info_not_available_text : params.detalhes);
        m_textBrowsers.append(tb_details);
        m_tabs->addTab(tb_details, params.show_details_text);

        // 3. Licenses
        QString full_licenses = params.licencas;
        if (!params.sites_licencas.isEmpty()) {
            full_licenses += "\n\n" + params.site_oficial_text + ":\n" + params.sites_licencas;
        }
        auto* tb_licenses = create_text_browser(full_licenses.isEmpty() ? params.info_not_available_text : full_licenses);
        m_textBrowsers.append(tb_licenses);
        m_tabs->addTab(tb_licenses, params.show_licenses_text);

        // 4. Notices
        auto* tb_notices = create_text_browser(params.avisos.isEmpty() ? params.info_not_available_text : params.avisos);
        m_textBrowsers.append(tb_notices);
        m_tabs->addTab(tb_notices, params.show_notices_text);

        // 5. Privacy Policy
        auto* tb_privacy = create_text_browser(params.privacy_policy.isEmpty() ? params.info_not_available_text : params.privacy_policy);
        m_textBrowsers.append(tb_privacy);
        m_tabs->addTab(tb_privacy, params.show_privacy_policy_text);

        // 6. Release Notes
        auto* tb_release = create_text_browser(params.release_notes.isEmpty() ? params.info_not_available_text : params.release_notes);
        m_textBrowsers.append(tb_release);
        m_tabs->addTab(tb_release, params.show_release_notes_text);

        m_tabShowLabels = {
            params.show_history_text,
            params.show_details_text,
            params.show_licenses_text,
            params.show_notices_text,
            params.show_privacy_policy_text,
            params.show_release_notes_text
        };

        m_tabHideLabels = {
            params.hide_history_text.isEmpty() ? params.show_history_text : params.hide_history_text,
            params.hide_details_text.isEmpty() ? params.show_details_text : params.hide_details_text,
            params.hide_licenses_text.isEmpty() ? params.show_licenses_text : params.hide_licenses_text,
            params.hide_notices_text.isEmpty() ? params.show_notices_text : params.hide_notices_text,
            params.hide_privacy_policy_text.isEmpty() ? params.show_privacy_policy_text : params.hide_privacy_policy_text,
            params.hide_release_notes_text.isEmpty() ? params.show_release_notes_text : params.hide_release_notes_text
        };

        layout->addWidget(m_tabs, 1);

        auto* btnLayout = new QHBoxLayout();
        btnLayout->addStretch(1);
        m_okButton = new QPushButton(params.ok_text, this);
        m_okButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
        btnLayout->addWidget(m_okButton);
        layout->addLayout(btnLayout);

        connect(m_tabs, &QTabWidget::currentChanged, this, &SobreDialog::on_tab_changed);
        connect(m_searchEdit, &QLineEdit::textChanged, this, &SobreDialog::on_search_text_changed);
        connect(m_searchEdit, &QLineEdit::returnPressed, this, &SobreDialog::on_search_return_pressed);
        connect(m_searchPrevBtn, &QToolButton::clicked, this, &SobreDialog::on_search_prev_clicked);
        connect(m_searchNextBtn, &QToolButton::clicked, this, &SobreDialog::on_search_next_clicked);

        // Key filter for search input to support Shift+Enter
        m_searchEdit->installEventFilter(this);

        if (m_app != nullptr) {
            auto* gm = m_app->findChild<GerenciadorTraducao*>();
            if (gm != nullptr) {
                connect(gm, &GerenciadorTraducao::idioma_alterado, this, &SobreDialog::retranslate_ui);
            }
        }

        retranslate_ui();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar dialog sobre: %1").arg(e.what()));
    }
}

void SobreDialog::changeEvent(QEvent* event) {
    if (event && event->type() == QEvent::LanguageChange) {
        retranslate_ui();
    }
    QDialog::changeEvent(event);
}

void SobreDialog::retranslate_ui(const QString& codigo_idioma) {
    try {
        QString idioma = codigo_idioma;
        if (idioma.isEmpty()) {
            if (m_app != nullptr) {
                auto* gm = m_app->findChild<GerenciadorTraducao*>();
                if (gm != nullptr) {
                    idioma = gm->obter_idioma_atual();
                } else {
                    idioma = GerenciadorTraducao::idioma_atual_global();
                }
            } else {
                idioma = GerenciadorTraducao::idioma_atual_global();
            }
        }
        if (idioma.isEmpty()) {
            idioma = QStringLiteral("pt_BR");
        }

        const bool is_en = (idioma == QStringLiteral("en_US"));

        auto tr_str = [is_en](const char* key, const QString& ptVal, const QString& enVal) -> QString {
            QString translated = QCoreApplication::translate("App", key);
            if (translated != key && !translated.isEmpty()) {
                return translated;
            }
            return is_en ? enVal : ptVal;
        };

        setWindowTitle(is_en ? QStringLiteral("About - Engineering Economy Calculator")
                             : QStringLiteral("Sobre - Calculadora para Economia"));

        QString cabecalho_fixo = QString(
            "<h3>ECONOMIA APP</h3>"
            "<p><b>%1:</b> 2026.10.7.0</p>"
            "<p><b>%2:</b> Fernando Nillsson Cidade</p>"
            "<p><b>%3:</b> %4</p>"
        ).arg(tr_str("version", QStringLiteral("Versão"), QStringLiteral("Version")),
              tr_str("authors", QStringLiteral("Autores"), QStringLiteral("Authors")),
              tr_str("description", QStringLiteral("Descrição"), QStringLiteral("Description")),
              tr_str("description_text", QStringLiteral("Calculadora de Economia de Engenharia"), QStringLiteral("Engineering Economy Calculator")));
        m_fixedLabel->setText(cabecalho_fixo);

        m_searchLabel->setText(is_en ? QStringLiteral("Search:") : QStringLiteral("Buscar:"));
        m_searchEdit->setPlaceholderText(is_en ? QStringLiteral("Search text in about dialog...") : QStringLiteral("Buscar texto na janela Sobre..."));
        m_searchPrevBtn->setToolTip(is_en ? QStringLiteral("Previous match") : QStringLiteral("Resultado anterior"));
        m_searchNextBtn->setToolTip(is_en ? QStringLiteral("Next match") : QStringLiteral("Próximo resultado"));
        m_searchPrevBtn->setAccessibleName(m_searchPrevBtn->toolTip());
        m_searchNextBtn->setAccessibleName(m_searchNextBtn->toolTip());
        m_okButton->setText(is_en ? QStringLiteral("OK") : QStringLiteral("OK"));

        m_tabShowLabels = {
            tr_str("show_history", QStringLiteral("Histórico"), QStringLiteral("History")),
            tr_str("show_details", QStringLiteral("Detalhes"), QStringLiteral("Details")),
            tr_str("show_licenses", QStringLiteral("Licenças"), QStringLiteral("Licenses")),
            tr_str("show_notices", QStringLiteral("Avisos"), QStringLiteral("Notices")),
            tr_str("show_privacy_policy", QStringLiteral("Política de Privacidade"), QStringLiteral("Privacy Policy")),
            tr_str("show_release_notes", QStringLiteral("Notas de Versão"), QStringLiteral("Release Notes"))
        };

        m_tabHideLabels = {
            tr_str("hide_history", QStringLiteral("Ocultar histórico"), QStringLiteral("Hide history")),
            tr_str("hide_details", QStringLiteral("Ocultar detalhes"), QStringLiteral("Hide details")),
            tr_str("hide_licenses", QStringLiteral("Ocultar licenças"), QStringLiteral("Hide licenses")),
            tr_str("hide_notices", QStringLiteral("Ocultar avisos"), QStringLiteral("Hide notices")),
            tr_str("hide_privacy_policy", QStringLiteral("Ocultar política de privacidade"), QStringLiteral("Hide privacy policy")),
            tr_str("hide_release_notes", QStringLiteral("Ocultar notas de versão"), QStringLiteral("Hide release notes"))
        };

        // Text content updates
        const QString info_na = is_en ? QStringLiteral("Information not available") : QStringLiteral("Informação não disponível");
        if (m_textBrowsers.size() >= 6) {
            QString hist = is_en ? OpcoesSobre::get_history_app_en_us() : OpcoesSobre::get_history_app_pt_br();
            m_textBrowsers[0]->setPlainText(hist.isEmpty() ? info_na : hist);

            QString det = is_en ? OpcoesSobre::get_about_text_en_us() : OpcoesSobre::get_about_text_pt_br();
            m_textBrowsers[1]->setPlainText(det.isEmpty() ? info_na : det);

            QString lic = is_en ? OpcoesSobre::get_license_text_en_us() : OpcoesSobre::get_license_text_pt_br();
            if (!OpcoesSobre::SITE_LICENSES.isEmpty()) {
                lic += QStringLiteral("\n\n") + (is_en ? QStringLiteral("Official site:") : QStringLiteral("Site oficial:")) + QStringLiteral("\n") + OpcoesSobre::SITE_LICENSES;
            }
            m_textBrowsers[2]->setPlainText(lic.isEmpty() ? info_na : lic);

            QString noti = is_en ? OpcoesSobre::get_notice_text_en_us() : OpcoesSobre::get_notice_text_pt_br();
            m_textBrowsers[3]->setPlainText(noti.isEmpty() ? info_na : noti);

            QString priv = is_en ? OpcoesSobre::get_privacy_policy_en_us() : OpcoesSobre::get_privacy_policy_pt_br();
            m_textBrowsers[4]->setPlainText(priv.isEmpty() ? info_na : priv);

            QString rel = is_en ? OpcoesSobre::get_release_notes_en_us() : OpcoesSobre::get_release_notes_pt_br();
            m_textBrowsers[5]->setPlainText(rel.isEmpty() ? info_na : rel);
        }

        update_tab_labels(m_tabs->currentIndex());
        rebuild_search_current_tab();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao retraduzir dialog Sobre: %1").arg(e.what()));
    }
}

void SobreDialog::update_tab_labels(int current_index) {
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (i < m_tabShowLabels.size() && i < m_tabHideLabels.size()) {
            QString label = (i == current_index && !m_tabHideLabels[i].isEmpty()) ? m_tabHideLabels[i] : m_tabShowLabels[i];
            m_tabs->setTabText(i, label);
        }
    }
}

void SobreDialog::on_tab_changed(int current_index) {
    update_tab_labels(current_index);
    rebuild_search_current_tab();
}

void SobreDialog::on_search_text_changed(const QString& text) {
    Q_UNUSED(text);
    rebuild_search_current_tab();
}

void SobreDialog::on_search_return_pressed() {
    navigate_search(1);
}

void SobreDialog::on_search_prev_clicked() {
    navigate_search(-1);
}

void SobreDialog::on_search_next_clicked() {
    navigate_search(1);
}

bool SobreDialog::has_any_matches() const {
    if (m_searchEdit == nullptr) return false;
    const QString query = m_searchEdit->text().trimmed();
    if (query.isEmpty()) return false;
    for (auto* browser : m_textBrowsers) {
        if (browser && browser->document()) {
            if (!browser->document()->find(query).isNull()) {
                return true;
            }
        }
    }
    return false;
}

void SobreDialog::update_search_highlights(QTextBrowser* viewer) {
    if (viewer == nullptr || viewer->document() == nullptr) {
        return;
    }

    QList<QTextEdit::ExtraSelection> selections;
    if (m_searchState.query.isEmpty()) {
        viewer->setExtraSelections(selections);
        return;
    }

    QColor highlight_color = viewer->palette().color(QPalette::Highlight);
    highlight_color.setAlpha(95);
    QColor current_color = viewer->palette().color(QPalette::Highlight);
    current_color.setAlpha(190);
    const QColor current_text_color = viewer->palette().color(QPalette::HighlightedText);

    for (int index = 0; index < m_searchState.matches.size(); ++index) {
        QTextEdit::ExtraSelection selection;
        selection.cursor = m_searchState.matches.at(index);
        selection.format.setBackground(index == m_searchState.current_index ? current_color : highlight_color);
        if (index == m_searchState.current_index) {
            selection.format.setForeground(current_text_color);
        }
        selections.append(selection);
    }

    viewer->setExtraSelections(selections);
}

void SobreDialog::rebuild_search_current_tab() {
    int idx = m_tabs ? m_tabs->currentIndex() : -1;
    if (idx < 0 || idx >= m_textBrowsers.size()) {
        m_searchState.query.clear();
        m_searchState.matches.clear();
        m_searchState.current_index = -1;
        update_search_ui();
        return;
    }

    QTextBrowser* viewer = m_textBrowsers[idx];
    const QString query = m_searchEdit ? m_searchEdit->text().trimmed() : QString();

    m_searchState.query = query;
    m_searchState.matches.clear();
    m_searchState.current_index = -1;

    if (viewer == nullptr || viewer->document() == nullptr || query.isEmpty()) {
        update_search_highlights(viewer);
        update_search_ui();
        return;
    }

    QTextCursor cursor(viewer->document());
    while (true) {
        cursor = viewer->document()->find(query, cursor);
        if (cursor.isNull()) {
            break;
        }
        m_searchState.matches.append(cursor);
    }

    if (!m_searchState.matches.isEmpty()) {
        m_searchState.current_index = 0;
        const QTextCursor first_cursor = m_searchState.matches.at(0);
        viewer->setTextCursor(first_cursor);
        viewer->ensureCursorVisible();
    }

    update_search_highlights(viewer);
    update_search_ui();
}

void SobreDialog::navigate_search(int step) {
    if (m_searchEdit == nullptr || step == 0) return;
    const QString query = m_searchEdit->text().trimmed();
    if (query.isEmpty()) return;

    int current_tab = m_tabs ? m_tabs->currentIndex() : -1;
    if (current_tab < 0 || current_tab >= m_textBrowsers.size()) return;

    // 1. Move within current tab if possible
    if (!m_searchState.matches.isEmpty()) {
        int next_idx = m_searchState.current_index + step;
        if (next_idx >= 0 && next_idx < m_searchState.matches.size()) {
            m_searchState.current_index = next_idx;
            QTextBrowser* viewer = m_textBrowsers[current_tab];
            const QTextCursor cursor = m_searchState.matches.at(m_searchState.current_index);
            update_search_highlights(viewer);
            viewer->setTextCursor(cursor);
            viewer->ensureCursorVisible();
            update_search_ui();
            return;
        }
    }

    // 2. Scan other tabs in circular direction
    int count = m_tabs->count();
    for (int i = 1; i < count; ++i) {
        int next_tab = (current_tab + (step > 0 ? i : -i) + count * 10) % count;
        QTextBrowser* candidate_viewer = m_textBrowsers[next_tab];
        if (candidate_viewer && candidate_viewer->document()) {
            QTextCursor test_cursor = candidate_viewer->document()->find(query);
            if (!test_cursor.isNull()) {
                m_tabs->setCurrentIndex(next_tab);
                if (step < 0 && !m_searchState.matches.isEmpty()) {
                    m_searchState.current_index = m_searchState.matches.size() - 1;
                    QTextBrowser* viewer = m_textBrowsers[next_tab];
                    const QTextCursor cursor = m_searchState.matches.at(m_searchState.current_index);
                    update_search_highlights(viewer);
                    viewer->setTextCursor(cursor);
                    viewer->ensureCursorVisible();
                    update_search_ui();
                }
                return;
            }
        }
    }

    // 3. Wrap within current tab if only current tab has matches
    if (!m_searchState.matches.isEmpty()) {
        m_searchState.current_index = step < 0 ? m_searchState.matches.size() - 1 : 0;
        QTextBrowser* viewer = m_textBrowsers[current_tab];
        const QTextCursor cursor = m_searchState.matches.at(m_searchState.current_index);
        update_search_highlights(viewer);
        viewer->setTextCursor(cursor);
        viewer->ensureCursorVisible();
        update_search_ui();
    }
}

void SobreDialog::update_search_ui() {
    bool has_matches = has_any_matches();
    if (m_searchPrevBtn) m_searchPrevBtn->setEnabled(has_matches);
    if (m_searchNextBtn) m_searchNextBtn->setEnabled(has_matches);

    if (m_searchCountLabel == nullptr) return;

    const QString query = m_searchEdit ? m_searchEdit->text().trimmed() : QString();
    if (query.isEmpty()) {
        m_searchCountLabel->clear();
        return;
    }

    bool is_en = (m_searchLabel && m_searchLabel->text() == QStringLiteral("Search:"));
    if (m_searchState.matches.isEmpty()) {
        m_searchCountLabel->setText(is_en ? QStringLiteral("0 of 0") : QStringLiteral("0 de 0"));
    } else {
        m_searchCountLabel->setText(is_en ? QStringLiteral("%1 of %2").arg(m_searchState.current_index + 1).arg(m_searchState.matches.size())
                                          : QStringLiteral("%1 de %2").arg(m_searchState.current_index + 1).arg(m_searchState.matches.size()));
    }
}
