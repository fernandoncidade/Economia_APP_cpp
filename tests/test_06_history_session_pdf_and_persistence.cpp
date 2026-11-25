#include "TestSupport.hpp"
#include "ui/ui_23_history_container.hpp"
#include "ui/ui_25_export_pdf.hpp"
#include "utils/DialogHelper.hpp"
#include "utils/LogManager.hpp"
#include "utils/MathRenderer.hpp"
#include "utils/SessionManager.hpp"
#include "utils/TextFormat.hpp"
#include "utils/TrialManager.hpp"

#include <QCheckBox>
#include <QFile>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QPrinter>
#include <QPushButton>
#include <QTemporaryDir>
#include <QTextDocument>

class TestHistorySessionPdfPersistence : public QObject {
    Q_OBJECT

private slots:
    void test_history_container_edit_delete_json_and_export_html() {
        HistoryContainer hc;
        hc.append("Calculo 1\n  F = 1210,00");
        hc.append("Calculo 2\n  P = 1000,00");
        QCOMPARE(hc.count(), 2);
        QVERIFY(!hc.isEmpty());

        const QJsonArray saved = hc.toJsonArray();
        QCOMPARE(saved.size(), 2);

        // Select first checkbox and test edit / cancel / edit / commit
        const auto checkboxes = hc.findChildren<QCheckBox*>();
        QVERIFY(checkboxes.size() >= 2);
        checkboxes[0]->setChecked(true);
        QCOMPARE(hc.get_selected_indices(), QList<int>{0});

        QVERIFY(hc.edit_selected());
        QVERIFY(hc.is_editing());
        QVERIFY(hc.cancel_edit());
        QVERIFY(!hc.is_editing());

        checkboxes[0]->setChecked(true);
        QVERIFY(hc.edit_selected());
        const auto textEdits = hc.findChildren<QTextEdit*>();
        QVERIFY(!textEdits.isEmpty());
        textEdits[0]->setPlainText("Calculo 1 Editado");
        QVERIFY(hc.commit_edit());
        QVERIFY(hc.get_entry_text(0).contains("Editado"));

        // Delete selected item
        checkboxes[0]->setChecked(true);
        hc.delete_selected();
        QCOMPARE(hc.count(), 1);

        // Restore JSON array
        hc.loadFromJsonArray(saved);
        QCOMPARE(hc.count(), 2);
        const QString exportHtml = hc.toExportHtml();
        QVERIFY(exportHtml.contains("Calculo 1") && exportHtml.contains("Calculo 2"));
    }

    void test_session_manager_save_and_restore_full_app() {
        SessionManager::clear_session();
        QVERIFY(!SessionManager::has_saved_session());

        {
            auto app1 = TestSupport::createIsolatedApp();
            app1->interest_p->setText("54321");
            app1->interest_i->setText("8.5");
            app1->interest_n->setText("10");
            app1->calculate_interest();
            QVERIFY(app1->interest_result->count() == 1);
            QVERIFY(SessionManager::save_session(app1.get()));
        }

        QVERIFY(SessionManager::has_saved_session());
        QVERIFY(QFileInfo::exists(SessionManager::get_session_file_path()));

        {
            TestSupport::scheduleAutoDismissModal(QMessageBox::Yes, 3000);
            auto app2 = std::make_unique<FinancialCalculatorApp>();
            QTest::qWait(250);
            QCoreApplication::processEvents();
            QCOMPARE(app2->interest_p->text(), QStringLiteral("54321"));
            QCOMPARE(app2->interest_i->text(), QStringLiteral("8.5"));
            QCOMPARE(app2->interest_n->text(), QStringLiteral("10"));
            QVERIFY(app2->interest_result->count() >= 1);
        }

        SessionManager::clear_session();
        QVERIFY(!SessionManager::has_saved_session());
    }

    void test_pdf_export_helpers_and_real_qprinter_pdf_generation() {
        const QString ptName = PdfExport::localize_filename("juros.pdf");
        QVERIFY(ptName.endsWith(".pdf"));

        auto app = TestSupport::createIsolatedApp();
        app->interest_calc_type->setCurrentIndex(0);
        app->interest_regime->setCurrentIndex(0);
        app->interest_p->setText("1000");
        app->interest_i->setText("10");
        app->interest_n->setText("2");
        app->calculate_interest();

        QTemporaryDir outDir;
        QVERIFY(outDir.isValid());
        const QString pdfPath = outDir.filePath("suite_export_test.pdf");

        QTextDocument doc;
        MathRenderer::register_handler(&doc);
        doc.setHtml(app->interest_result->toExportHtml());
        MathRenderer::apply_radicals(&doc);

        QPrinter printer(QPrinter::PrinterResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(pdfPath);
        doc.print(&printer);

        QFile pdfFile(pdfPath);
        QVERIFY(pdfFile.exists());
        QVERIFY(pdfFile.size() > 1024);
        QVERIFY(pdfFile.open(QIODevice::ReadOnly));
        QVERIFY(pdfFile.read(5).startsWith("%PDF-"));
    }

    void test_trial_log_and_dialog_helper() {
        QVERIFY(TrialManager::LIBERAR_USO_DEFINITIVO);
        QVERIFY(!TrialManager::is_trial_expired());
        const QJsonObject trialInfo = TrialManager::get_trial_info();
        QVERIFY(!trialInfo.isEmpty());

        LogManager::info("Suite test log verification entry");
        const QString logFile = LogManager::get_log_file();
        QVERIFY(!logFile.isEmpty());
        QVERIFY(QFileInfo::exists(logFile));

        QMessageBox msg;
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        DialogHelper::traduzir_botoes(&msg);
        QVERIFY(!msg.button(QMessageBox::Yes)->text().isEmpty());
        QVERIFY(!msg.button(QMessageBox::No)->text().isEmpty());
    }
};

ECONOMIA_TEST_MAIN(TestHistorySessionPdfPersistence)
#include "test_06_history_session_pdf_and_persistence.moc"
