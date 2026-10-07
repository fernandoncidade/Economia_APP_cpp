#include "ui_26_menu_bar.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "ui_24_font_config_dialog.hpp"
#include "ui_25_export_pdf.hpp"
#include "ui_28_exibir_sobre.hpp"
#include "ui_33_exibir_manual.hpp"
#include "ui_23_history_container.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/FontManager.hpp"
#include "../utils/TextFormat.hpp"
#include "../utils/MathRenderer.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
#include <QKeySequence>
#include <QCoreApplication>
#include <QTextEdit>
#include <QRegularExpression>
#include <QStringList>

void setup_menu_bar(FinancialCalculatorApp* app) {
    if (app) {
        app->create_menu_bar();
    }
}

void FinancialCalculatorApp::create_menu_bar() {
    try {
        QMenuBar* menubar = new QMenuBar(this);

        // Menu Arquivos
        QMenu* file_menu = menubar->addMenu(QCoreApplication::translate("App", "Arquivos"));
        QAction* export_current_action = new QAction(QCoreApplication::translate("App", "Exportar Atual"), this);
        QAction* export_all_action = new QAction(QCoreApplication::translate("App", "Exportar Todos"), this);
        file_menu->addAction(export_current_action);
        file_menu->addAction(export_all_action);

        // Menu Configuração
        QMenu* config_menu = menubar->addMenu(QCoreApplication::translate("App", "Configuração"));
        QMenu* idiomas_menu = config_menu->addMenu(QCoreApplication::translate("App", "Idiomas"));

        QAction* action_pt = new QAction(QCoreApplication::translate("App", "Português (Brasil)"), this);
        QAction* action_en = new QAction(QCoreApplication::translate("App", "English (United States)"), this);
        action_pt->setCheckable(true);
        action_en->setCheckable(true);

        QActionGroup* ag = new QActionGroup(this);
        ag->setExclusive(true);
        ag->addAction(action_pt);
        ag->addAction(action_en);

        idiomas_menu->addAction(action_pt);
        idiomas_menu->addAction(action_en);

        auto update_language_checks = [action_pt, action_en](const QString& codigo) {
            action_pt->setChecked(codigo == "pt_BR");
            action_en->setChecked(codigo == "en_US");
        };

        if (gerenciador) {
            update_language_checks(gerenciador->obter_idioma_atual());
            connect(gerenciador, &GerenciadorTraducao::idioma_alterado, this, [action_pt, action_en](const QString& codigo) {
                action_pt->setChecked(codigo == "pt_BR");
                action_en->setChecked(codigo == "en_US");
            });
        }

        connect(action_pt, &QAction::triggered, this, [this, update_language_checks]() {
            if (gerenciador) {
                gerenciador->definir_idioma("pt_BR");
                update_language_checks("pt_BR");
            }
        });

        connect(action_en, &QAction::triggered, this, [this, update_language_checks]() {
            if (gerenciador) {
                gerenciador->definir_idioma("en_US");
                update_language_checks("en_US");
            }
        });

        config_menu->addSeparator();

        QAction* font_config_action = new QAction(QCoreApplication::translate("App", "Configurar Fontes"), this);
        connect(font_config_action, &QAction::triggered, this, [this]() {
            try {
                FontConfigDialog dialog(this);
                dialog.exec();
            } catch (const std::exception& e) {
                LogManager::instance().log(QString("Erro ao abrir configuração de fontes: %1").arg(e.what()), LogManager::LogLevel::ERR);
            }
        });
        config_menu->addAction(font_config_action);

        // Menu Opções
        QMenu* options_menu = menubar->addMenu(QCoreApplication::translate("App", "Opções"));
        QAction* about_action = new QAction(QCoreApplication::translate("App", "Sobre"), this);
        about_action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+A")));
        connect(about_action, &QAction::triggered, this, [this]() {
            exibir_sobre(this);
        });
        options_menu->addAction(about_action);

        QAction* manual_action = new QAction(QCoreApplication::translate("App", "Manual"), this);
        manual_action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+M")));
        connect(manual_action, &QAction::triggered, this, [this]() {
            exibir_manual(this);
        });
        options_menu->addAction(manual_action);

        // Export All
        auto export_all = [this](const QString& suggested_name = "todos_calculos.pdf") {
            struct Section {
                QString title;
                QString text;
            };
            QList<Section> sections;

            auto add_section = [&sections](const QString& title, HistoryContainer* widget) {
                if (!widget) return;
                QString text = widget->toRawText().trimmed();
                if (!text.isEmpty()) {
                    sections.append({title, text});
                }
            };

            add_section(QCoreApplication::translate("App", "Juros (Simples/Compostos)"), interest_result);
            add_section(QCoreApplication::translate("App", "Anuidades"), annuity_result);
            add_section(QCoreApplication::translate("App", "Gradientes"), grad_result);
            add_section(QCoreApplication::translate("App", "Equivalência de Taxas"), rate_equiv_result);
            add_section(QCoreApplication::translate("App", "Taxa Real / Aparente"), rate_real_result);
            add_section(QCoreApplication::translate("App", "Taxa Efetiva / TIR / Taxa Global"), eff_rate_result);
            add_section(QCoreApplication::translate("App", "Amortização"), amort_result);
            add_section(QCoreApplication::translate("App", "Análise de Investimentos"), invest_result);
            add_section(QCoreApplication::translate("App", "Depreciação"), deprec_result);
            add_section(QCoreApplication::translate("App", "Retorno Mínimo (TMA)"), min_return_result);
            add_section(QCoreApplication::translate("App", "Equação de Fisher"), fisher_result);
            add_section(QCoreApplication::translate("App", "VPL com Tributos"), vpl_tax_result);
            add_section(QCoreApplication::translate("App", "CAUE - Vida Econômica"), caue_result);

            QString font_css = FontManager::get_html_style();
            QStringList html_parts;
            html_parts << "<html><head><meta charset='utf-8'>" << font_css;
            html_parts << "<style>body { margin: 12px; } pre.calc, pre { white-space: pre-wrap; overflow-wrap: break-word; word-break: break-word; } sub { font-size: 75%; vertical-align: sub; } sup { font-size: 75%; vertical-align: super; }</style></head><body>";
            html_parts << QString("<h1>%1</h1>").arg(QCoreApplication::translate("App", "Todos os Cálculos").toHtmlEscaped());

            for (const auto& sec : sections) {
                QString rich_sec = TextFormat::to_rich_html(sec.text);
                if (rich_sec.contains("<table", Qt::CaseInsensitive)) {
                    rich_sec.replace("<table", "</pre><table", Qt::CaseInsensitive);
                    rich_sec.replace("</table>", "</table><pre class=\"calc\">", Qt::CaseInsensitive);
                }
                html_parts << QString("<h2>%1</h2><pre class=\"calc\">%2</pre>").arg(sec.title.toHtmlEscaped(), rich_sec);
            }

            QString table_html = this->amort_table_to_html();
            if (!table_html.isEmpty()) {
                html_parts << table_html;
            }

            html_parts << "</body></html>";
            QString full_html = html_parts.join("\n");

            QTextEdit combined;
            combined.setReadOnly(true);
            combined.setHtml(full_html);
            MathRenderer::apply_radicals(combined.document());
            this->export_to_pdf(&combined, suggested_name);
        };

        // Export Current
        auto export_current = [this, export_all]() {
            int idx = tabs->currentIndex();
            QString tab_name = tabs->tabText(idx);

            auto normalize = [](const QString& s) -> QString {
                QString res = s.toLower();
                res.replace(QRegularExpression("[^0-9a-z]+"), " ");
                return res.trimmed();
            };

            QString tab_norm = normalize(tab_name);

            if (tab_norm == normalize(QCoreApplication::translate("App", "Amortização"))) {
                this->export_amortization_pdf("amortizacao.pdf");
                return;
            }

            if (tab_norm == normalize(QCoreApplication::translate("App", "Conversão de Taxas"))) {
                if (rate_real_result && !rate_real_result->toPlainText().trimmed().isEmpty()) {
                    this->export_to_pdf(rate_real_result, "taxa_real_aparente.pdf");
                    return;
                }
                if (rate_equiv_result && !rate_equiv_result->toPlainText().trimmed().isEmpty()) {
                    this->export_to_pdf(rate_equiv_result, "equivalencia_taxa.pdf");
                    return;
                }
                if (rate_equiv_result) {
                    this->export_to_pdf(rate_equiv_result, "equivalencia_taxa.pdf");
                    return;
                }
            }

            struct TabMap {
                QString title;
                HistoryContainer* widget;
                QString pdf_name;
            };

            QList<TabMap> mapping = {
                {QCoreApplication::translate("App", "Juros Simples e Compostos"), interest_result, "juros.pdf"},
                {QCoreApplication::translate("App", "Anuidades"), annuity_result, "anuidades.pdf"},
                {QCoreApplication::translate("App", "Gradientes"), grad_result, "gradiente.pdf"},
                {QCoreApplication::translate("App", "Análise de Investimentos"), invest_result, "investimento.pdf"},
                {QCoreApplication::translate("App", "Depreciação"), deprec_result, "depreciacao.pdf"},
                {QCoreApplication::translate("App", "Taxa Efetiva / TIR / Taxa Global"), eff_rate_result, "taxa_efetiva_tir.pdf"},
                {QCoreApplication::translate("App", "Retorno Mínimo (TMA)"), min_return_result, "tma_minima.pdf"},
                {QCoreApplication::translate("App", "Equação de Fisher"), fisher_result, "fisher.pdf"},
                {QCoreApplication::translate("App", "TMA Real e Nominal"), fisher_result, "fisher.pdf"},
                {QCoreApplication::translate("App", "VPL com Tributos"), vpl_tax_result, "vpl_tributos.pdf"},
                {QCoreApplication::translate("App", "VPL com Impostos"), vpl_tax_result, "vpl_tributos.pdf"},
                {QCoreApplication::translate("App", "CAUE - Vida Econômica"), caue_result, "caue.pdf"}
            };

            for (const auto& item : mapping) {
                if (normalize(item.title) == tab_norm) {
                    if (item.widget) {
                        this->export_to_pdf(item.widget, item.pdf_name);
                        return;
                    }
                }
            }

            QString safe_name = tab_name.toLower();
            safe_name.replace(QRegularExpression("[^0-9a-z]+"), "_");
            safe_name = safe_name.trimmed();
            if (safe_name.isEmpty()) {
                safe_name = "todos_calculos";
            }
            export_all(safe_name + ".pdf");
        };

        connect(export_current_action, &QAction::triggered, this, export_current);
        connect(export_all_action, &QAction::triggered, this, [export_all]() { export_all("todos_calculos.pdf"); });

        this->setMenuBar(menubar);

    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar menu: %1").arg(e.what()));
        throw;
    }
}
