#include "TestSupport.hpp"
#include "language/tr_01_gerenciadorTraducao.hpp"
#include "ui/ui_23_history_container.hpp"
#include "ui/ui_27_SobreDialog.hpp"
#include "ui/ui_29_opcoes_sobre.hpp"
#include "ui/ui_30_Manual.hpp"

#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolButton>

class TestUiDialogsAboutManualI18n : public QObject {
    Q_OBJECT

private slots:
    void test_main_window_12_tabs_menubar_and_clear_all() {
        auto app = TestSupport::createIsolatedApp();
        QCOMPARE(app->tabs->count(), 12);
        QVERIFY(app->menuBar() != nullptr);
        QVERIFY(!app->menuBar()->actions().isEmpty());
        QVERIFY(!app->windowTitle().isEmpty());

        app->interest_p->setText("1000");
        app->interest_i->setText("5");
        app->interest_n->setText("2");
        app->calculate_interest();
        QCOMPARE(app->interest_result->count(), 1);

        app->clear_all_history();
        QCOMPARE(app->interest_result->count(), 0);
    }

    void test_opcoes_sobre_and_sobre_dialog_search_and_retranslation() {
        QVERIFY(OpcoesSobre::SITE_LICENSES.contains("agpl-3.0"));
        QVERIFY(!OpcoesSobre::SITE_LICENSES.contains("python-pillow"));

        QVERIFY(!OpcoesSobre::get_about_text_pt_br().isEmpty());
        QVERIFY(!OpcoesSobre::get_about_text_en_us().isEmpty());
        QVERIFY(!OpcoesSobre::get_license_text_pt_br().isEmpty());
        QVERIFY(!OpcoesSobre::get_license_text_en_us().isEmpty());
        QVERIFY(!OpcoesSobre::get_notice_text_pt_br().isEmpty());
        QVERIFY(!OpcoesSobre::get_notice_text_en_us().isEmpty());
        QVERIFY(!OpcoesSobre::get_privacy_policy_pt_br().isEmpty());
        QVERIFY(!OpcoesSobre::get_privacy_policy_en_us().isEmpty());
        QVERIFY(!OpcoesSobre::get_history_app_pt_br().isEmpty());
        QVERIFY(!OpcoesSobre::get_history_app_en_us().isEmpty());
        QVERIFY(!OpcoesSobre::get_release_notes_pt_br().isEmpty());
        QVERIFY(!OpcoesSobre::get_release_notes_en_us().isEmpty());

        SobreDialogParams params;
        params.titulo = "Sobre Economia_APP";
        params.texto_fixo = "<p><b>Versão:</b> 2026.10.7.0</p>";
        params.texto_history = OpcoesSobre::get_history_app_pt_br();
        params.detalhes = OpcoesSobre::get_about_text_pt_br();
        params.licencas = OpcoesSobre::get_license_text_pt_br();
        params.avisos = OpcoesSobre::get_notice_text_pt_br();
        params.privacy_policy = OpcoesSobre::get_privacy_policy_pt_br();
        params.release_notes = OpcoesSobre::get_release_notes_pt_br();
        params.sites_licencas = OpcoesSobre::SITE_LICENSES;

        SobreDialog dlg(nullptr, params);
        auto* tabs = dlg.findChild<QTabWidget*>();
        auto* searchEdit = dlg.findChild<QLineEdit*>();
        QVERIFY(tabs != nullptr);
        QVERIFY(searchEdit != nullptr);
        QVERIFY(tabs->count() >= 5);

        tabs->setCurrentIndex(1);
        searchEdit->setText("Economia");
        QCoreApplication::processEvents();
        const auto toolBtns = dlg.findChildren<QToolButton*>();
        QVERIFY(toolBtns.size() >= 2);
        toolBtns[1]->click(); // next match

        dlg.retranslate_ui("en_US");
        dlg.retranslate_ui("pt_BR");
    }

    void test_manual_sections_blocks_and_positions_bilingual() {
        for (const QString& lang : {QStringLiteral("pt_BR"), QStringLiteral("en_US")}) {
            QVERIFY(!source::ui::Manual::get_manual_title(lang).isEmpty());
            const auto sections = source::ui::Manual::get_manual_document(lang);
            QVERIFY(sections.size() >= 6);
            const auto blocksPair = source::ui::Manual::get_manual_blocks(lang);
            QVERIFY(!blocksPair.first.isEmpty());
            const auto withPos = source::ui::Manual::get_manual_text_with_positions(lang);
            QVERIFY(!withPos.text.isEmpty());
            QVERIFY(!withPos.positions.isEmpty());
            QVERIFY(!withPos.order.isEmpty());
        }
    }

    void test_runtime_language_switch_preserves_subscripts() {
        auto app = TestSupport::createIsolatedApp();
        QVERIFY(app->gerenciador != nullptr);

        app->retranslate_ui("en_US");
        QCoreApplication::processEvents();
        QVERIFY2(app->label_tma_real->text().contains("<sub>r</sub>") && !app->label_tma_real->text().contains("i_r"),
                 qPrintable(app->label_tma_real->text()));
        QVERIFY2(app->caue_input_table->horizontalHeaderItem(1)->text().contains(QChar(0x2099)),
                 qPrintable(app->caue_input_table->horizontalHeaderItem(1)->text()));

        app->retranslate_ui("pt_BR");
        QCoreApplication::processEvents();
        QVERIFY(app->label_tma_real->text().contains("<sub>r</sub>"));
    }
};

ECONOMIA_TEST_MAIN(TestUiDialogsAboutManualI18n)
#include "test_07_ui_dialogs_about_manual_and_i18n.moc"
