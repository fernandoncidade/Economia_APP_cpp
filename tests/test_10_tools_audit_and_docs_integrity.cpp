#include "TestSupport.hpp"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryDir>

class TestToolsAuditAndDocsIntegrity : public QObject {
    Q_OBJECT

private:
    static QString readUtf8(const QString& path) {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return {};
        }
        return QString::fromUtf8(f.readAll());
    }

    static QString detectActiveVersion(const QDir& root) {
        const QString iss = readUtf8(root.filePath("library/Economia_APP.iss"));
        const QRegularExpression rx(QStringLiteral("#define\\s+MyAppVersion\\s+\"([^\"]+)\""));
        const auto m = rx.match(iss);
        return m.hasMatch() ? m.captured(1).trimmed() : QStringLiteral("2026.11.22.0");
    }

    static bool copyDirRecursive(const QString& srcDir, const QString& dstDir) {
        QDir src(srcDir);
        if (!src.exists()) {
            return false;
        }
        QDir().mkpath(dstDir);
        QDirIterator it(srcDir, QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            const QString rel = src.relativeFilePath(it.filePath());
            const QString target = QDir(dstDir).filePath(rel);
            QDir().mkpath(QFileInfo(target).absolutePath());
            if (!QFile::copy(it.filePath(), target)) {
                return false;
            }
        }
        return true;
    }

private slots:
    void test_find_version_editor_check_passes() {
        const QString exe = QDir(TestSupport::suiteBuildDir()).filePath("find_VersionEditor.exe");
        QVERIFY2(QFileInfo::exists(exe), qPrintable(exe));

        QProcess proc;
        proc.setWorkingDirectory(TestSupport::projectRoot());
        proc.start(exe, {"--check"});
        QVERIFY2(proc.waitForFinished(30000), "find_VersionEditor --check timed out");
        const QString output = QString::fromUtf8(proc.readAllStandardOutput()) + QString::fromUtf8(proc.readAllStandardError());
        QVERIFY2(proc.exitCode() == 0,
                 qPrintable(QString("find_VersionEditor --check failed (%1):\n%2").arg(proc.exitCode()).arg(output)));
    }

    void test_library_moved_files_and_complete_bilingual_docs() {
        const QDir root(TestSupport::projectRoot());

        // Verify 5 packaging files moved from repo/ to library/
        for (const QString& moved : {
                 "AppxManifest.xml",
                 "Economia_APP_cpp.txt",
                 "Economia_APP.iss",
                 "Economia_APP_MSVC.iss",
                 "Economia_APP_MinGW.iss"
             }) {
            QVERIFY2(QFileInfo::exists(root.filePath("library/" + moved)),
                     qPrintable(QString("Missing moved file in library/: %1").arg(moved)));
            QVERIFY2(!QFileInfo::exists(root.filePath("repo/" + moved)),
                     qPrintable(QString("Old copy still present in repo/: %1").arg(moved)));
        }
        const QString activeVer = detectActiveVersion(root);
        QVERIFY2(!activeVer.isEmpty(), "Failed to detect MyAppVersion from library/Economia_APP.iss");
        QVERIFY(readUtf8(root.filePath("library/AppxManifest.xml")).contains(QString("Version=\"%1\"").arg(activeVer)));

        // Mandatory root documentation and governance files
        for (const QString& rel : {
                 "LICENSE", "CODE_OF_CONDUCT.md", "CONTRIBUTING.md", "SECURITY.md",
                 "README.md", "MANUAL.md", "OBSERVACAO.md"
             }) {
            QVERIFY2(QFileInfo::exists(root.filePath(rel)), qPrintable(QString("Missing root doc: %1").arg(rel)));
        }

        // Complete docs/ tree (cli-reference.md intentionally excluded)
        const QString releaseDocRel = QString("docs/release-v%1.md").arg(activeVer);
        for (const QString& rel : {
                 QStringLiteral("docs/architecture.md"),
                 QStringLiteral("docs/compliance.md"),
                 QStringLiteral("docs/licensing.md"),
                 QStringLiteral("docs/privacy-policy.md"),
                 QStringLiteral("docs/security-model.md"),
                 QStringLiteral("docs/user-guide.md"),
                 releaseDocRel,
                 QStringLiteral("docs/suggested-issues.md"),
                 QStringLiteral("docs/technical-audit-and-roadmap.md"),
                 QStringLiteral("docs/images/banner.png"),
                 QStringLiteral("docs/images/logo.png")
             }) {
            QVERIFY2(QFileInfo::exists(root.filePath(rel)), qPrintable(QString("Missing doc/asset: %1").arg(rel)));
        }
        QVERIFY2(!QFileInfo::exists(root.filePath("docs/cli-reference.md")),
                 "docs/cli-reference.md must NOT exist in Economia_APP_cpp");

        // Dual licensing check across LICENSE, licensing.md, README.md, and source/assets/
        const QString agplAsset = root.filePath("source/assets/LICENSES/GNU AFFERO GENERAL PUBLIC LICENSE Version3.txt");
        QVERIFY2(QFileInfo::exists(agplAsset) && QFileInfo(agplAsset).size() > 10000, "Missing AGPLv3 asset copy");

        const QString licensingDoc = readUtf8(root.filePath("docs/licensing.md"));
        QVERIFY(licensingDoc.contains("AGPL") && licensingDoc.contains("Microsoft Store") && licensingDoc.contains("EULA"));

        const QString readmeDoc = readUtf8(root.filePath("README.md"));
        QVERIFY(readmeDoc.contains("AGPL") && readmeDoc.contains("Microsoft Store"));
        QVERIFY(!readmeDoc.contains("repo/LISTAGEM") && !readmeDoc.contains("repo\\LISTAGEM"));

        // Verify all 14 GUI screenshots under images/interfaces/{pt_BR,en_US} exist and are documented in README.md
        const QStringList ptScreenshots = {
            "Captura de tela 2025-10-21 154812.png",
            "Captura de tela 2025-10-21 154832.png",
            "Captura de tela 2025-10-21 154849.png",
            "Captura de tela 2025-10-21 154905.png",
            "Captura de tela 2025-10-21 154924.png",
            "Captura de tela 2025-10-21 154944.png",
            "Captura de tela 2025-10-21 155001.png"
        };
        const QStringList enScreenshots = {
            "Captura de tela 2025-10-21 155020.png",
            "Captura de tela 2025-10-21 155041.png",
            "Captura de tela 2025-10-21 155104.png",
            "Captura de tela 2025-10-21 155119.png",
            "Captura de tela 2025-10-21 155137.png",
            "Captura de tela 2025-10-21 155155.png",
            "Captura de tela 2025-10-21 155215.png"
        };
        for (const QString& name : ptScreenshots) {
            QVERIFY2(QFileInfo::exists(root.filePath("images/interfaces/pt_BR/" + name)), qPrintable(name));
            QString encoded = name;
            encoded.replace(' ', "%20");
            QVERIFY2(readmeDoc.contains("images/interfaces/pt_BR/" + encoded), qPrintable("Missing README link: " + encoded));
        }
        for (const QString& name : enScreenshots) {
            QVERIFY2(QFileInfo::exists(root.filePath("images/interfaces/en_US/" + name)), qPrintable(name));
            QString encoded = name;
            encoded.replace(' ', "%20");
            QVERIFY2(readmeDoc.contains("images/interfaces/en_US/" + encoded), qPrintable("Missing README link: " + encoded));
        }

        const QString eulaPt = readUtf8(root.filePath("source/assets/EULA/EULA_pt_BR - Economia.txt"));
        QVERIFY(eulaPt.contains("Microsoft Store") && eulaPt.contains("AGPL"));
    }

    void test_find_version_editor_apply_and_release_rename_in_sandbox() {
        const QDir root(TestSupport::projectRoot());
        QTemporaryDir sandbox;
        QVERIFY(sandbox.isValid());
        const QDir dst(sandbox.path());

        // Populate isolated sandbox mirror of all version-monitored files
        QVERIFY(copyDirRecursive(root.filePath("source/assets"), dst.filePath("source/assets")));
        QVERIFY(copyDirRecursive(root.filePath("docs"), dst.filePath("docs")));
        QVERIFY(copyDirRecursive(root.filePath("library"), dst.filePath("library")));
        dst.mkpath("source/ui");
        dst.mkpath("mocks");
        for (const QString& rel : {
                 "CMakeLists.txt",
                 "README.md",
                 "MANUAL.md",
                 "OBSERVACAO.md",
                 "CODE_OF_CONDUCT.md",
                 "CONTRIBUTING.md",
                 "SECURITY.md",
                 "source/ui/ui_27_SobreDialog.cpp",
                 "source/ui/ui_28_exibir_sobre.cpp"
             }) {
            QVERIFY2(QFile::copy(root.filePath(rel), dst.filePath(rel)),
                     qPrintable(QString("Failed to mirror: %1").arg(rel)));
        }

        const QString exe = QDir(TestSupport::suiteBuildDir()).filePath("find_VersionEditor.exe");
        QVERIFY2(QFileInfo::exists(exe), qPrintable(exe));

        QProcess applyProc;
        applyProc.setWorkingDirectory(dst.path());
        applyProc.start(exe, {
            "--apply",
            "2026.12.31.0",
            "31 de dezembro de 2026",
            "2026.12.31.0",
            "December 31, 2026",
            dst.path()
        });
        QVERIFY2(applyProc.waitForFinished(30000), "find_VersionEditor --apply timed out");
        const QString applyOut = QString::fromUtf8(applyProc.readAllStandardOutput()) + QString::fromUtf8(applyProc.readAllStandardError());
        QVERIFY2(applyProc.exitCode() == 0,
                 qPrintable(QString("find_VersionEditor --apply failed (%1):\n%2").arg(applyProc.exitCode()).arg(applyOut)));

        // Release markdown renamed + content updated
        const QString activeVer = detectActiveVersion(root);
        QVERIFY(!QFileInfo::exists(dst.filePath(QString("docs/release-v%1.md").arg(activeVer))));
        QVERIFY(QFileInfo::exists(dst.filePath("docs/release-v2026.12.31.0.md")));
        const QString relDoc = readUtf8(dst.filePath("docs/release-v2026.12.31.0.md"));
        QVERIFY(relDoc.contains("v2026.12.31.0") && relDoc.contains("31 de dezembro de 2026") && relDoc.contains("December 31, 2026"));

        // AppxManifest + Economia_APP_cpp.txt + ISS files updated cleanly
        QVERIFY(readUtf8(dst.filePath("library/AppxManifest.xml")).contains("Version=\"2026.12.31.0\""));
        const QString libNotes = readUtf8(dst.filePath("library/Economia_APP_cpp.txt"));
        QVERIFY(libNotes.contains("Versão 2026.12.31.0") && libNotes.contains("Data: 31 de dezembro de 2026"));
        QVERIFY(libNotes.contains("Version 2026.12.31.0") && libNotes.contains("Date: December 31, 2026"));
        QVERIFY(libNotes.contains("release-v2026.12.31.0.md"));

        // Non-CalVer versions such as Qt 6.12.0, SDK 10.0.28000.0, and GCC 15.1.0 remain untouched
        const QString archDoc = readUtf8(dst.filePath("docs/architecture.md"));
        QVERIFY(archDoc.contains("Qt 6.12.0") && archDoc.contains("10.0.28000.0") && archDoc.contains("2026.12.31.0"));
        const QString secDoc = readUtf8(dst.filePath("docs/security-model.md"));
        QVERIFY(secDoc.contains("15.1.0") && secDoc.contains("2026.12.31.0"));

        // Root markdown governance docs updated
        for (const QString& md : {"CODE_OF_CONDUCT.md", "CONTRIBUTING.md", "SECURITY.md", "README.md", "MANUAL.md", "OBSERVACAO.md"}) {
            const QString txt = readUtf8(dst.filePath(md));
            QVERIFY2(txt.contains("2026.12.31.0"), qPrintable(QString("%1 not updated").arg(md)));
        }

        // Running --check on the updated sandbox must pass 100%
        QProcess recheck;
        recheck.setWorkingDirectory(dst.path());
        recheck.start(exe, {"--check", dst.path()});
        QVERIFY(recheck.waitForFinished(30000));
        const QString recheckOut = QString::fromUtf8(recheck.readAllStandardOutput()) + QString::fromUtf8(recheck.readAllStandardError());
        QVERIFY2(recheck.exitCode() == 0, qPrintable(recheckOut));
    }

    void test_build_presets_and_vscode_suite_tasks_consistency() {
        const QDir root(TestSupport::projectRoot());
        const QString presets = readUtf8(root.filePath("CMakePresets.json"));
        const QString tasks = readUtf8(root.filePath(".vscode/tasks.json"));

        QVERIFY(!presets.contains("build_Economia_APP_msvc_ninja"));
        QVERIFY(!presets.contains("build_mocks_msvc_ninja"));
        QVERIFY(presets.contains("build_Economia_APP_msvc"));
        QVERIFY(presets.contains("build_mocks_msvc"));
        QVERIFY(presets.contains("suite-msvc-release"));
        QVERIFY(presets.contains("suite-mingw-release"));

        QVERIFY(tasks.contains("Qt6: Compilar e Executar SUITE (MSVC)"));
        QVERIFY(tasks.contains("Qt6: Compilar e Executar SUITE (MinGW)"));
        QVERIFY(!tasks.contains("build_Economia_APP_msvc_ninja"));
        QVERIFY(!tasks.contains("build_mocks_msvc_ninja"));
    }
};

ECONOMIA_TEST_MAIN(TestToolsAuditAndDocsIntegrity)
#include "test_10_tools_audit_and_docs_integrity.moc"
