#include "TestSupport.hpp"
#include "utils/ApplicationPathUtils.hpp"
#include "utils/IconUtils.hpp"

#include <QFileInfo>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QPrinterInfo>
#include <QStyleFactory>

#include <expected>
#include <format>
#include <ranges>
#include <span>
#include <vector>
#include <version>

class TestQtPluginsModulesAndCpp23 : public QObject {
    Q_OBJECT

private slots:
    void test_cpp23_standard_features_and_abi() {
        static_assert(sizeof(void*) == 8, "Economia_APP must target 64-bit ABI");
#if defined(_MSC_VER)
        static_assert(_MSVC_LANG >= 202004L, "MSVC must compile with C++23 (/std:c++latest or /std:c++23preview)");
        QCOMPARE(TestSupport::activeToolchain(), QStringLiteral("MSVC"));
#elif defined(__MINGW32__)
        static_assert(__cplusplus >= 202302L, "MinGW GCC must compile with -std=c++23");
        QCOMPARE(TestSupport::activeToolchain(), QStringLiteral("MinGW"));
#endif

        // Exercise <expected>, <ranges>, <format>, and <span> at runtime
        auto computeMargin = [](double rev, double cost) -> std::expected<double, QString> {
            if (rev <= 0.0) {
                return std::unexpected(QStringLiteral("invalid revenue"));
            }
            return (rev - cost) / rev;
        };
        const auto okVal = computeMargin(200.0, 50.0);
        QVERIFY(okVal.has_value() && std::abs(*okVal - 0.75) < 1e-9);
        QVERIFY(!computeMargin(0.0, 10.0).has_value());

        const std::vector<int> nums = {1, 2, 3, 4, 5};
        std::span<const int> viewSpan(nums);
        int evenSum = 0;
        for (int v : viewSpan | std::views::filter([](int x) { return x % 2 == 0; })) {
            evenSum += v;
        }
        QCOMPARE(evenSum, 6);

        const std::string formatted = std::format("Qt-{}-{}", 6, 12);
        QCOMPARE(QString::fromStdString(formatted), QStringLiteral("Qt-6-12"));
    }

    void test_qt612_modules_plugins_styles_and_icons() {
        const QString runtimeVer = QString::fromLatin1(qVersion());
        QVERIFY2(runtimeVer.startsWith("6.12."), qPrintable(QString("Expected Qt 6.12.x, got %1").arg(runtimeVer)));

        // Platform plugin active in offscreen suite
        QVERIFY(!QGuiApplication::platformName().isEmpty());

        // Styles plugin (modern windows / fusion)
        const QStringList styles = QStyleFactory::keys();
        QVERIFY(!styles.isEmpty());

        // Image format plugins (ico for app icon, png for docs/UI)
        const QList<QByteArray> formats = QImageReader::supportedImageFormats();
        QVERIFY2(formats.contains("ico"), "Missing qico imageformat plugin");
        QVERIFY2(formats.contains("png"), "Missing png imageformat support");

        // Icon resolution from source/assets/icones via ApplicationPathUtils / IconUtils
        const QString icoPath = IconUtils::get_icon_path("economia.ico");
        QVERIFY2(QFileInfo::exists(icoPath), qPrintable(QString("Missing icon: %1").arg(icoPath)));
        QIcon icon(icoPath);
        QVERIFY(!icon.isNull());

        // PrintSupport module linkage check
        const auto printers = QPrinterInfo::availablePrinterNames();
        Q_UNUSED(printers);
    }
};

ECONOMIA_TEST_MAIN(TestQtPluginsModulesAndCpp23)
#include "test_08_qt_plugins_modules_and_cpp23.moc"
