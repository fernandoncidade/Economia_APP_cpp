#ifndef TR_01_GERENCIADOR_TRADUCAO_HPP
#define TR_01_GERENCIADOR_TRADUCAO_HPP

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QPair>
#include <QTranslator>
#include <memory>

class QDialog;

class GerenciadorTraducao : public QObject {
    Q_OBJECT

public:
    explicit GerenciadorTraducao(QObject* parent = nullptr);
    ~GerenciadorTraducao();

    void carregar_configuracao_idioma();
    void salvar_configuracao_idioma();
    QString obter_caminho_configuracao() const;

    bool aplicar_traducao();
    bool definir_idioma(const QString& codigo_idioma);
    QString obter_idioma_atual() const;

    void traduzir_botoes_padrao(QDialog* dialogo);

    const QMap<QString, QString>& idiomas_disponiveis() const { return m_idiomas_disponiveis; }

    static QString traduzir_texto(const QString& texto, const QString& idioma_destino);
    static void carregar_dicionario_se_necessario();
    static QString idioma_atual_global();

signals:
    void idioma_alterado(const QString& codigo_idioma);

private:
    void remover_tradutor_instalado();

    QString m_idioma_atual;
    QString m_idioma_padrao;
    QString m_dir_traducoes;
    QMap<QString, QString> m_idiomas_disponiveis;
    std::unique_ptr<QTranslator> m_tradutor;

    static QList<QPair<QString, QString>> s_pt_to_en;
    static QList<QPair<QString, QString>> s_en_to_pt;
    static bool s_dicionario_carregado;
};

#endif // TR_01_GERENCIADOR_TRADUCAO_HPP
