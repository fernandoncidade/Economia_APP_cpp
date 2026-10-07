/**
 * @file MathRenderer.hpp
 * @brief Renderizador matemático nativo em Qt6 para equações e radicais via QTextObjectInterface e QPainter.
 * Desenha raízes quadradas e de qualquer ordem (com fração ou escalar) com linhas vetoriais perfeitas,
 * adaptando-se dinamicamente a fontes e temas Claro e Escuro do Windows 11 sem gerar imagens estáticas.
 */

#ifndef MATH_RENDERER_HPP
#define MATH_RENDERER_HPP

#include <QObject>
#include <QString>
#include <QFont>
#include <QColor>
#include <QImage>
#include <QTextFormat>
#include <QTextObjectInterface>
#include <QTextDocument>
#include <QPainter>
#include <optional>

namespace MathRenderer {

/// Tipo customizado do QTextFormat para objetos de radicais matemáticos
const int RadicalFormatType = QTextFormat::UserObject + 1;

/// Propriedades armazenadas no QTextCharFormat para desenhar o radical
enum RadicalProperties {
    PropIndex = 1,
    PropNumerator,
    PropDenominator,
    PropRadicand,
    PropPrefix,
    PropSuffix,
    PropIsFraction,
    PropFontFamily,
    PropFontSize,
    PropPenWidth,
    PropColor
};

/**
 * @brief Handler nativo do Qt6 (QTextObjectInterface) responsável por medir e desenhar
 * a raiz quadrada / radical diretamente via QPainter vetorial no QTextDocument / QTextEdit e exportação PDF.
 */
class RadicalObjectHandler : public QObject, public QTextObjectInterface {
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit RadicalObjectHandler(QObject* parent = nullptr);
    ~RadicalObjectHandler() override = default;

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;
    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

    enum class ScriptType {
        Normal,
        Subscript,
        Superscript
    };

    struct TextRun {
        QString text;
        ScriptType type = ScriptType::Normal;
    };

    struct LayoutMetrics {
        bool is_fraction = false;
        double prefix_w = 0;
        double suffix_w = 0;
        double index_w = 0;
        double index_h = 0;
        double index_reserve_w = 0;
        double num_w = 0;
        double num_h = 0;
        double den_w = 0;
        double den_h = 0;
        double rad_w = 0;
        double rad_h = 0;
        double rad_box_w = 0;
        double rad_box_h = 0;
        double radical_hook_w = 0;
        double bar_thickness = 1.8;
        double gap = 3.0;
        double pad_left = 4.0;
        double pad_right = 4.0;
        double pad_top = 3.0;
        double pad_bottom = 3.0;
        double total_w = 0;
        double total_h = 0;

        QList<TextRun> prefix_runs;
        QList<TextRun> suffix_runs;
        QList<TextRun> num_runs;
        QList<TextRun> den_runs;
        QList<TextRun> rad_runs;
    };

    static LayoutMetrics compute_metrics(const QTextFormat &format, const QFont &base_font);
};

/**
 * @brief Especificação para renderização de qualquer equação com raiz / radical.
 */
struct RadicalSpec {
    QString prefix;         ///< Prefixo antes do radical, ex.: "  TIRₘ = "
    QString index;          ///< Índice da raiz, ex.: "n", "5", ou vazio para raiz quadrada
    QString numerator;      ///< Numerador do radicando (se fração)
    QString denominator;    ///< Denominador do radicando (se fração)
    QString radicand;       ///< Radicando escalar (usado se denominador for vazio)
    QString suffix;         ///< Sufixo após o radical, ex.: "  - 1"
    
    std::optional<QFont> font;    ///< Fonte personalizada (opcional)
    std::optional<QColor> color;  ///< Cor personalizada (opcional, padrão herda tema)
    qreal scale = 1.0;
    qreal penWidth = 1.8;
};

/**
 * @brief Resultado da renderização compatível com a interface anterior.
 */
struct RenderResult {
    QImage image;           ///< QImage vazia (não usa imagens raster)
    int logicalWidth = 0;
    int logicalHeight = 0;
    QString htmlTag;        ///< Marcador nativo pronto para o QTextDocument
};

/**
 * @brief Codifica uma especificação de radical em um token estruturado imune a formatações HTML.
 */
QString encode_radical(const RadicalSpec& spec);

/**
 * @brief Registra o handler nativo no layout do documento.
 */
void register_handler(QTextDocument *doc);

/**
 * @brief Varre o QTextDocument e converte marcadores estruturados, expressões LaTeX ou radicais Unicode
 * em objetos nativos QTextObjectInterface desenhados via QPainter.
 */
void apply_radicals(QTextDocument *doc);

/**
 * @brief Converte marcadores estruturados em texto Unicode legível para clipboard, relatórios e cópia.
 */
QString to_plain_text(const QString &text);

/**
 * @brief Renderiza uma equação com radical retornando o objeto RenderResult.
 */
RenderResult render_radical(const RadicalSpec& spec);

/**
 * @brief Renderiza uma equação com radical retornando diretamente o código/marcador nativo.
 */
QString render_radical_html(const RadicalSpec& spec);

/**
 * @brief Helper de conveniência para renderizar um radical com fração em equações nativas do Qt6.
 * Formato: prefixo \sqrt[indice]{numerador / denominador} sufixo
 */
QString render_radical_fraction_html(
    const QString& index,
    const QString& numerator,
    const QString& denominator,
    const QString& prefix = "",
    const QString& suffix = "",
    const std::optional<QFont>& font = std::nullopt,
    const std::optional<QColor>& color = std::nullopt
);

/**
 * @brief Helper de conveniência para renderizar um radical escalar em equações nativas do Qt6.
 * Formato: prefixo \sqrt[indice]{radicando} sufixo
 */
QString render_radical_single_html(
    const QString& index,
    const QString& radicand,
    const QString& prefix = "",
    const QString& suffix = "",
    const std::optional<QFont>& font = std::nullopt,
    const std::optional<QColor>& color = std::nullopt
);

} // namespace MathRenderer

#endif // MATH_RENDERER_HPP
