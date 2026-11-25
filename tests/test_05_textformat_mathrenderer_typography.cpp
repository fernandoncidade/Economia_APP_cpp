#include "TestSupport.hpp"
#include "utils/FontManager.hpp"
#include "utils/MathRenderer.hpp"
#include "utils/TextFormat.hpp"
#include "ui/ui_24_font_config_dialog.hpp"

#include <QImage>
#include <QPainter>
#include <QTextDocument>

class TestTextFormatMathRendererTypography : public QObject {
    Q_OBJECT

private slots:
    void test_text_format_subscripts_and_superscripts() {
        QCOMPARE(TextFormat::to_subscript("n"), QString(QChar(0x2099)));
        QCOMPARE(TextFormat::to_subscript("12"), QString::fromUtf8("\xE2\x82\x81\xE2\x82\x82"));
        QCOMPARE(TextFormat::to_superscript("2"), QString::fromUtf8("\xC2\xB2"));
        QCOMPARE(TextFormat::to_superscript_parens("1/12"),
                 QString::fromUtf8("\xE2\x81\xBD\xC2\xB9/\xC2\xB9\xC2\xB2\xE2\x81\xBE"));

        QCOMPARE(TextFormat::to_html_subscripts("TMA Real (i_r % ao ano):"),
                 QStringLiteral("TMA Real (i<sub>r</sub> % ao ano):"));
        QCOMPARE(TextFormat::to_unicode_subscripts("VR_n (R$)"),
                 QStringLiteral("VR") + QChar(0x2099) + QStringLiteral(" (R$)"));
        QCOMPARE(TextFormat::to_unicode_subscripts("Com_n (R$)"),
                 QStringLiteral("Com") + QChar(0x2099) + QStringLiteral(" (R$)"));
        QCOMPARE(TextFormat::to_unicode_subscripts("i_r"),
                 QStringLiteral("i") + QChar(0x1D63));

        QCOMPARE(TextFormat::from_unicode_subscripts(TextFormat::to_unicode_subscripts("x_10")),
                 QStringLiteral("x10"));
        QVERIFY(!TextFormat::format_currency(1234.56, 2).isEmpty());

        const auto frac = TextFormat::format_fraction("1 + i", "1 + r", "x = ");
        QVERIFY(frac[0].contains("1 + i"));
        QVERIFY(frac[1].contains(QChar(0x2500)));
        QVERIFY(frac[2].contains("1 + r"));
    }

    void test_math_renderer_vector_layout_and_qpainter_output() {
        MathRenderer::RadicalSpec specFrac;
        specFrac.prefix = "  TIR_m = ";
        specFrac.index = "5";
        specFrac.numerator = "15000,00";
        specFrac.denominator = "10000,00";
        specFrac.suffix = " - 1";

        const QString encoded = MathRenderer::encode_radical(specFrac);
        QVERIFY(encoded.contains("RADICAL"));

        const QString plain = MathRenderer::to_plain_text(encoded);
        QVERIFY(plain.contains("15000,00") && plain.contains("10000,00"));

        QTextDocument doc;
        MathRenderer::register_handler(&doc);
        doc.setHtml(QString("<p>%1</p>").arg(TextFormat::to_rich_html(encoded)));
        MathRenderer::apply_radicals(&doc);
        QVERIFY(doc.documentLayout()->handlerForObject(MathRenderer::RadicalFormatType) != nullptr);

        QImage canvas(480, 140, QImage::Format_ARGB32);
        canvas.fill(Qt::white);
        QPainter painter(&canvas);
        doc.setPageSize(QSizeF(480, 140));
        doc.drawContents(&painter, canvas.rect());
        painter.end();

        // Confirm vector drawing actually rendered non-white pixels into canvas
        int darkPixels = 0;
        for (int y = 0; y < canvas.height(); ++y) {
            for (int x = 0; x < canvas.width(); ++x) {
                if (canvas.pixelColor(x, y) != QColor(Qt::white)) {
                    ++darkPixels;
                }
            }
        }
        QVERIFY2(darkPixels > 40, qPrintable(QString("Expected painted radical pixels > 40, got %1").arg(darkPixels)));
    }

    void test_font_manager_persistence_and_dialog() {
        QVERIFY(FontManager::reset_to_default());
        const FontConfig def = FontManager::default_config();
        QCOMPARE(def.family, QStringLiteral("Courier New"));
        QCOMPARE(def.size, 10);

        FontConfig custom;
        custom.family = "Consolas";
        custom.size = 12;
        custom.bold = true;
        custom.italic = true;
        custom.underline = false;
        QVERIFY(FontManager::save_config(custom));

        const FontConfig loaded = FontManager::get_config();
        QCOMPARE(loaded.family, QStringLiteral("Consolas"));
        QCOMPARE(loaded.size, 12);
        QCOMPARE(loaded.bold, true);
        QCOMPARE(loaded.italic, true);
        QVERIFY(FontManager::get_html_style().contains("Consolas"));

        FontConfigDialog dlg;
        QVERIFY(dlg.findChild<QFontComboBox*>() != nullptr);
        QVERIFY(FontManager::reset_to_default());
    }
};

ECONOMIA_TEST_MAIN(TestTextFormatMathRendererTypography)
#include "test_05_textformat_mathrenderer_typography.moc"
