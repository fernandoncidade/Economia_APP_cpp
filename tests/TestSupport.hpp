#ifndef TEST_SUPPORT_HPP
#define TEST_SUPPORT_HPP

#include <QAbstractButton>
#include <QApplication>
#include <QCoreApplication>
#include <QComboBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLocale>
#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>
#include <QVector>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>

#include "fca_01_FinancialCalculatorAPP.hpp"
#include "utils/MathRenderer.hpp"
#include "utils/SessionManager.hpp"
#include "utils/TextFormat.hpp"

namespace TestSupport {

class IsolatedEnvGuard {
public:
    IsolatedEnvGuard() {
        m_prevLocalAppData = qEnvironmentVariable("LOCALAPPDATA");
        m_hadLocalAppData = qEnvironmentVariableIsSet("LOCALAPPDATA");
        m_prevAppData = qEnvironmentVariable("APPDATA");
        m_hadAppData = qEnvironmentVariableIsSet("APPDATA");
        m_prevPlatform = qEnvironmentVariable("QT_QPA_PLATFORM");
        m_hadPlatform = qEnvironmentVariableIsSet("QT_QPA_PLATFORM");

        if (!m_hadPlatform || m_prevPlatform.isEmpty()) {
            qputenv("QT_QPA_PLATFORM", "offscreen");
        }

        if (m_tempDir.isValid()) {
            const QByteArray localBytes = QDir::toNativeSeparators(m_tempDir.filePath("LocalAppData")).toUtf8();
            const QByteArray roamingBytes = QDir::toNativeSeparators(m_tempDir.filePath("AppData")).toUtf8();
            QDir().mkpath(QString::fromUtf8(localBytes));
            QDir().mkpath(QString::fromUtf8(roamingBytes));
            qputenv("LOCALAPPDATA", localBytes);
            qputenv("APPDATA", roamingBytes);
        }
    }

    ~IsolatedEnvGuard() {
        if (m_hadLocalAppData) {
            qputenv("LOCALAPPDATA", m_prevLocalAppData.toUtf8());
        } else {
            qunsetenv("LOCALAPPDATA");
        }
        if (m_hadAppData) {
            qputenv("APPDATA", m_prevAppData.toUtf8());
        } else {
            qunsetenv("APPDATA");
        }
        if (m_hadPlatform) {
            qputenv("QT_QPA_PLATFORM", m_prevPlatform.toUtf8());
        } else {
            qunsetenv("QT_QPA_PLATFORM");
        }
    }

    QString rootPath() const { return m_tempDir.path(); }

private:
    QTemporaryDir m_tempDir;
    QString m_prevLocalAppData;
    QString m_prevAppData;
    QString m_prevPlatform;
    bool m_hadLocalAppData = false;
    bool m_hadAppData = false;
    bool m_hadPlatform = false;
};

inline QString projectRoot() {
#ifdef ECONOMIA_PROJECT_ROOT
    return QString::fromUtf8(ECONOMIA_PROJECT_ROOT);
#else
    return QDir::currentPath();
#endif
}

inline QString suiteBuildDir() {
#ifdef ECONOMIA_BUILD_DIR
    return QString::fromUtf8(ECONOMIA_BUILD_DIR);
#else
    return QCoreApplication::applicationDirPath();
#endif
}

inline QString activeToolchain() {
#ifdef ECONOMIA_TOOLCHAIN
    return QString::fromUtf8(ECONOMIA_TOOLCHAIN);
#elif defined(_MSC_VER)
    return QStringLiteral("MSVC");
#else
    return QStringLiteral("MinGW");
#endif
}

inline double parseNumber(const QString& rawToken, bool* ok = nullptr) {
    QString s = rawToken.trimmed();
    s.remove(QChar(' '));
    s.remove(QStringLiteral("R$"));
    s.remove(QChar('%'));

    const int lastComma = s.lastIndexOf(QChar(','));
    const int lastDot = s.lastIndexOf(QChar('.'));

    if (lastComma >= 0 && lastDot >= 0) {
        if (lastComma > lastDot) {
            // pt-BR: 1.234,56
            s.remove(QChar('.'));
            s.replace(QChar(','), QChar('.'));
        } else {
            // en-US: 1,234.56
            s.remove(QChar(','));
        }
    } else if (lastComma >= 0) {
        const int decimals = s.size() - lastComma - 1;
        if (decimals == 3 && s.count(QChar(',')) > 1) {
            s.remove(QChar(','));
        } else {
            s.replace(QChar(','), QChar('.'));
        }
    } else if (s.count(QChar('.')) > 1) {
        s.remove(QChar('.'));
    }

    bool parsedOk = false;
    const double value = s.toDouble(&parsedOk);
    if (ok) {
        *ok = parsedOk;
    }
    return value;
}

inline bool findLastNumber(const QString& text, double& outValue) {
    const QString plain = MathRenderer::to_plain_text(text);
    const QRegularExpression re(QStringLiteral("[-+]?(?:\\d{1,3}(?:\\.\\d{3})+(?:,\\d+)?|\\d+(?:[.,]\\d+)?)"));
    auto it = re.globalMatch(plain);
    bool found = false;
    while (it.hasNext()) {
        const auto m = it.next();
        bool ok = false;
        const double candidate = parseNumber(m.captured(0), &ok);
        if (ok) {
            outValue = candidate;
            found = true;
        }
    }
    return found;
}

inline bool findNumberNear(const QString& text, const QString& anchor, double& outValue) {
    const QString plain = MathRenderer::to_plain_text(text);
    const int pos = plain.indexOf(anchor, 0, Qt::CaseInsensitive);
    if (pos < 0) {
        return false;
    }
    const QString tail = plain.mid(pos + anchor.size(), 180);
    const QRegularExpression re(QStringLiteral("[-+]?(?:\\d{1,3}(?:\\.\\d{3})+(?:,\\d+)?|\\d+(?:[.,]\\d+)?)"));
    const auto m = re.match(tail);
    if (!m.hasMatch()) {
        return false;
    }
    bool ok = false;
    outValue = parseNumber(m.captured(0), &ok);
    return ok;
}

inline std::unique_ptr<FinancialCalculatorApp> createIsolatedApp() {
    SessionManager::clear_session();
    auto app = std::make_unique<FinancialCalculatorApp>();
    // Drain constructor QTimer::singleShot(100) cleanly under isolated empty %LOCALAPPDATA%.
    QTest::qWait(130);
    QCoreApplication::processEvents();
    return app;
}

inline void scheduleAutoDismissModal(QMessageBox::StandardButton button = QMessageBox::Ok, int maxWaitMs = 3000) {
    auto* timer = new QTimer(QCoreApplication::instance());
    const qint64 deadline = QDateTime::currentMSecsSinceEpoch() + maxWaitMs;
    QObject::connect(timer, &QTimer::timeout, timer, [timer, button, deadline]() {
        if (auto* modal = QApplication::activeModalWidget()) {
            if (auto* box = qobject_cast<QMessageBox*>(modal)) {
                QAbstractButton* targetBtn = box->button(button);
                if (!targetBtn) {
                    QMessageBox::ButtonRole wantRole = QMessageBox::InvalidRole;
                    if (button == QMessageBox::Yes) wantRole = QMessageBox::YesRole;
                    else if (button == QMessageBox::No) wantRole = QMessageBox::NoRole;
                    else if (button == QMessageBox::Ok) wantRole = QMessageBox::AcceptRole;
                    else if (button == QMessageBox::Cancel) wantRole = QMessageBox::RejectRole;
                    for (QAbstractButton* b : box->buttons()) {
                        if (box->buttonRole(b) == wantRole) {
                            targetBtn = b;
                            break;
                        }
                    }
                }
                if (targetBtn) {
                    targetBtn->click();
                } else {
                    box->done(static_cast<int>(button));
                }
            } else {
                modal->close();
            }
            timer->stop();
            timer->deleteLater();
            return;
        }
        if (QDateTime::currentMSecsSinceEpoch() >= deadline) {
            timer->stop();
            timer->deleteLater();
        }
    });
    timer->start(25);
}

struct Pe64Inspection {
    bool exists = false;
    bool validDos = false;
    bool validPe = false;
    bool isAmd64 = false;
    bool isPe32Plus = false;
    QStringList importedDlls;
    QString error;
};

inline Pe64Inspection inspectPe64(const QString& filePath) {
    Pe64Inspection res;
    QFile file(filePath);
    if (!file.exists()) {
        res.error = QStringLiteral("File does not exist: %1").arg(filePath);
        return res;
    }
    res.exists = true;
    if (!file.open(QIODevice::ReadOnly)) {
        res.error = QStringLiteral("Cannot open: %1").arg(filePath);
        return res;
    }
    const QByteArray bytes = file.readAll();
    if (bytes.size() < 0x120) {
        res.error = QStringLiteral("Binary too small: %1").arg(bytes.size());
        return res;
    }

    auto readU16 = [&](int off) -> uint16_t {
        uint16_t v = 0;
        std::memcpy(&v, bytes.constData() + off, sizeof(v));
        return v;
    };
    auto readU32 = [&](int off) -> uint32_t {
        uint32_t v = 0;
        std::memcpy(&v, bytes.constData() + off, sizeof(v));
        return v;
    };

    res.validDos = (readU16(0) == 0x5A4D); // MZ
    if (!res.validDos) {
        res.error = QStringLiteral("Missing MZ signature");
        return res;
    }

    const uint32_t peOffset = readU32(0x3C);
    if (peOffset + 24 + 120 >= static_cast<uint32_t>(bytes.size())) {
        res.error = QStringLiteral("Invalid e_lfanew offset");
        return res;
    }

    res.validPe = (readU32(static_cast<int>(peOffset)) == 0x00004550u); // PE\0\0
    if (!res.validPe) {
        res.error = QStringLiteral("Missing PE signature");
        return res;
    }

    const int coffOff = static_cast<int>(peOffset + 4);
    const uint16_t machine = readU16(coffOff);
    const uint16_t numSections = readU16(coffOff + 2);
    const uint16_t optHeaderSize = readU16(coffOff + 16);
    res.isAmd64 = (machine == 0x8664u);

    const int optOff = coffOff + 20;
    const uint16_t optMagic = readU16(optOff);
    res.isPe32Plus = (optMagic == 0x020Bu);

    if (res.isPe32Plus && optHeaderSize >= 128 && optOff + optHeaderSize <= bytes.size()) {
        // DataDirectory[1] = Import Table for PE32+ starts at optOff + 112 + 8
        const uint32_t importRva = readU32(optOff + 112 + 8);
        const int sectionOff = optOff + optHeaderSize;

        struct Sec {
            uint32_t vsize;
            uint32_t vaddr;
            uint32_t rawSize;
            uint32_t rawPtr;
        };
        QVector<Sec> sections;
        for (uint16_t s = 0; s < numSections; ++s) {
            const int base = sectionOff + static_cast<int>(s) * 40;
            if (base + 40 > bytes.size()) {
                break;
            }
            sections.push_back({
                readU32(base + 8),
                readU32(base + 12),
                readU32(base + 16),
                readU32(base + 20)
            });
        }

        auto rvaToOffset = [&](uint32_t rva) -> int {
            for (const auto& sec : sections) {
                const uint32_t span = qMax(sec.vsize, sec.rawSize);
                if (rva >= sec.vaddr && rva < sec.vaddr + span) {
                    const uint32_t delta = rva - sec.vaddr;
                    if (delta < sec.rawSize && sec.rawPtr + delta < static_cast<uint32_t>(bytes.size())) {
                        return static_cast<int>(sec.rawPtr + delta);
                    }
                }
            }
            return -1;
        };

        if (importRva != 0) {
            int descOff = rvaToOffset(importRva);
            while (descOff >= 0 && descOff + 20 <= bytes.size()) {
                const uint32_t origFirstThunk = readU32(descOff);
                const uint32_t nameRva = readU32(descOff + 12);
                const uint32_t firstThunk = readU32(descOff + 16);
                if (origFirstThunk == 0 && nameRva == 0 && firstThunk == 0) {
                    break;
                }
                const int nameOff = rvaToOffset(nameRva);
                if (nameOff >= 0 && nameOff < bytes.size()) {
                    const char* ptr = bytes.constData() + nameOff;
                    const int maxLen = bytes.size() - nameOff;
                    int len = 0;
                    while (len < maxLen && ptr[len] != '\0') {
                        ++len;
                    }
                    res.importedDlls.append(QString::fromLatin1(ptr, len));
                }
                descOff += 20;
            }
        }
    }

    return res;
}

} // namespace TestSupport

#define ECONOMIA_TEST_MAIN(TestObject)                               \
    int main(int argc, char* argv[]) {                               \
        TestSupport::IsolatedEnvGuard envGuard;                      \
        QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil)); \
        QApplication app(argc, argv);                                \
        app.setQuitOnLastWindowClosed(false);                        \
        TestObject tc;                                               \
        return QTest::qExec(&tc, argc, argv);                        \
    }

#endif // TEST_SUPPORT_HPP
