#include "TestSupport.hpp"

#include <QDir>
#include <QFileInfo>

class TestToolchainIsolationAndBinaries : public QObject {
    Q_OBJECT

private slots:
    void test_suite_executables_are_valid_pe64_with_correct_crt_isolation() {
        const QString buildDir = TestSupport::suiteBuildDir();
        const QString toolchain = TestSupport::activeToolchain();
        const QStringList binaries = {
            "test_01_core_interest_annuity_gradient.exe",
            "test_02_rates_fisher_minreturn_effrate.exe",
            "test_03_amortization_depreciation.exe",
            "test_04_investment_vpl_taxes_caue.exe",
            "test_05_textformat_mathrenderer_typography.exe",
            "test_06_history_session_pdf_and_persistence.exe",
            "test_07_ui_dialogs_about_manual_and_i18n.exe",
            "test_08_qt_plugins_modules_and_cpp23.exe",
            "test_09_toolchain_isolation_and_binaries.exe",
            "test_10_tools_audit_and_docs_integrity.exe",
            "find_VersionEditor.exe",
            "CompileTranslationsTerminal.exe"
        };

        for (const QString& bin : binaries) {
            const QString fullPath = QDir(buildDir).filePath(bin);
            const auto pe = TestSupport::inspectPe64(fullPath);
            QVERIFY2(pe.exists && pe.validDos && pe.validPe && pe.isAmd64 && pe.isPe32Plus,
                     qPrintable(QString("%1 failed PE64 inspection: %2").arg(bin, pe.error)));

            bool hasMinGw = false;
            bool hasMsvc = false;
            for (const QString& dll : pe.importedDlls) {
                const QString lower = dll.toLower();
                if (lower.contains("libstdc++") || lower.contains("libgcc_s_seh") || lower.contains("libwinpthread")) {
                    hasMinGw = true;
                }
                if (lower.startsWith("msvcp140") || lower.startsWith("vcruntime140")) {
                    hasMsvc = true;
                }
            }
            if (toolchain == "MSVC") {
                QVERIFY2(!hasMinGw, qPrintable(QString("%1 unexpectedly imports MinGW DLLs").arg(bin)));
            } else {
                QVERIFY2(!hasMsvc, qPrintable(QString("%1 unexpectedly imports MSVC CRT DLLs").arg(bin)));
            }
        }
    }

    void test_deployed_qt6_runtime_dlls_plugins_and_absence_of_foreign_crt() {
        const QString buildDir = TestSupport::suiteBuildDir();
        const QString toolchain = TestSupport::activeToolchain();

        const QStringList requiredFiles = {
            "Qt6Core.dll",
            "Qt6Gui.dll",
            "Qt6Widgets.dll",
            "Qt6PrintSupport.dll",
            "Qt6Test.dll",
            "platforms/qwindows.dll",
            "platforms/qoffscreen.dll",
            "styles/qmodernwindowsstyle.dll",
            "imageformats/qico.dll"
        };
        for (const QString& rel : requiredFiles) {
            const QString path = QDir(buildDir).filePath(rel);
            QVERIFY2(QFileInfo::exists(path), qPrintable(QString("Missing suite runtime dependency: %1").arg(path)));
        }

        if (toolchain == "MSVC") {
            QVERIFY(QFileInfo::exists(QDir(buildDir).filePath("msvcp140.dll")));
            QVERIFY(QFileInfo::exists(QDir(buildDir).filePath("vcruntime140.dll")));
            QVERIFY(!QFileInfo::exists(QDir(buildDir).filePath("libstdc++-6.dll")));
            QVERIFY(!QFileInfo::exists(QDir(buildDir).filePath("libgcc_s_seh-1.dll")));
            QVERIFY(!QFileInfo::exists(QDir(buildDir).filePath("libwinpthread-1.dll")));
        } else {
            QVERIFY(QFileInfo::exists(QDir(buildDir).filePath("libstdc++-6.dll")));
            QVERIFY(QFileInfo::exists(QDir(buildDir).filePath("libgcc_s_seh-1.dll")));
            QVERIFY(QFileInfo::exists(QDir(buildDir).filePath("libwinpthread-1.dll")));
            QVERIFY(!QFileInfo::exists(QDir(buildDir).filePath("msvcp140.dll")));
            QVERIFY(!QFileInfo::exists(QDir(buildDir).filePath("vcruntime140.dll")));
        }

        // No Inno Setup installer payloads inside runtime directory
        const QDir dir(buildDir);
        const QStringList setups = dir.entryList({"*setup*.exe", "*Setup*.exe", "vc_redist*.exe"}, QDir::Files);
        QVERIFY2(setups.isEmpty(), qPrintable(QString("Unexpected installer found: %1").arg(setups.join(", "))));
    }
};

ECONOMIA_TEST_MAIN(TestToolchainIsolationAndBinaries)
#include "test_09_toolchain_isolation_and_binaries.moc"
