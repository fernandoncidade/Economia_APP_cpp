#include "ui_28_exibir_sobre.hpp"
#include "ui_27_SobreDialog.hpp"
#include "ui_29_opcoes_sobre.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"
#include "../utils/LogManager.hpp"
#include "../utils/DialogHelper.hpp"

#include <QCoreApplication>
#include <QPointer>

namespace {
static QPointer<SobreDialog> s_sobre_dialog = nullptr;
}

void exibir_sobre(QWidget* app) {
    try {
        if (s_sobre_dialog != nullptr && s_sobre_dialog->isVisible()) {
            if (s_sobre_dialog->isMinimized()) {
                s_sobre_dialog->showNormal();
            }
            s_sobre_dialog->raise();
            s_sobre_dialog->activateWindow();
            return;
        }

        QString idioma = "pt_BR";
        if (app) {
            auto* gm = app->findChild<GerenciadorTraducao*>();
            if (gm) {
                idioma = gm->obter_idioma_atual();
            }
        }

        auto tr_str = [](const char* key, const QString& defVal = QString()) -> QString {
            QString translated = QCoreApplication::translate("App", key);
            return (translated == key && !defVal.isEmpty()) ? defVal : translated;
        };

        QString texto_sobre = (idioma == "pt_BR") ? OpcoesSobre::get_about_text_pt_br() : OpcoesSobre::get_about_text_en_us();
        QString texto_licenca = (idioma == "pt_BR") ? OpcoesSobre::get_license_text_pt_br() : OpcoesSobre::get_license_text_en_us();
        QString texto_aviso = (idioma == "pt_BR") ? OpcoesSobre::get_notice_text_pt_br() : OpcoesSobre::get_notice_text_en_us();
        QString texto_privacidade = (idioma == "pt_BR") ? OpcoesSobre::get_privacy_policy_pt_br() : OpcoesSobre::get_privacy_policy_en_us();
        QString texto_history = (idioma == "pt_BR") ? OpcoesSobre::get_history_app_pt_br() : OpcoesSobre::get_history_app_en_us();
        QString texto_release = (idioma == "pt_BR") ? OpcoesSobre::get_release_notes_pt_br() : OpcoesSobre::get_release_notes_en_us();

        QString cabecalho_fixo = QString(
            "<h3>ECONOMIA APP</h3>"
            "<p><b>%1:</b> 2026.10.7.0</p>"
            "<p><b>%2:</b> Fernando Nillsson Cidade</p>"
            "<p><b>%3:</b> %4</p>"
        ).arg(tr_str("version", "Version"),
              tr_str("authors", "Authors"),
              tr_str("description", "Description"),
              tr_str("description_text", "Calculadora de Economia de Engenharia"));

        SobreDialogParams params;
        params.titulo = QString("%1 - Calculadora para Economia").arg(tr_str("Sobre", "Sobre"));
        params.texto_fixo = cabecalho_fixo;
        params.texto_history = texto_history;
        params.detalhes = texto_sobre;
        params.licencas = texto_licenca;
        params.sites_licencas = OpcoesSobre::SITE_LICENSES;
        params.show_history_text = tr_str("show_history", "History");
        params.hide_history_text = tr_str("hide_history", "Hide history");
        params.show_details_text = tr_str("show_details", "Details");
        params.hide_details_text = tr_str("hide_details", "Hide details");
        params.show_licenses_text = tr_str("show_licenses", "Licenses");
        params.hide_licenses_text = tr_str("hide_licenses", "Hide licenses");
        params.ok_text = tr_str("OK", "OK");
        params.site_oficial_text = tr_str("site_oficial", "Official site");
        params.avisos = texto_aviso;
        params.show_notices_text = tr_str("show_notices", "Notices");
        params.hide_notices_text = tr_str("hide_notices", "Hide notices");
        params.privacy_policy = texto_privacidade;
        params.show_privacy_policy_text = tr_str("show_privacy_policy", "Privacy Policy");
        params.hide_privacy_policy_text = tr_str("hide_privacy_policy", "Hide privacy policy");
        params.info_not_available_text = tr_str("information_not_available", "Information not available");
        params.release_notes = texto_release;
        params.show_release_notes_text = tr_str("show_release_notes", "Release Notes");
        params.hide_release_notes_text = tr_str("hide_release_notes", "Hide Release Notes");

        auto* dialog = new SobreDialog(app, params);
        s_sobre_dialog = dialog;
        dialog->show();
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao exibir diálogo Sobre: %1").arg(e.what()));
        DialogHelper::critical(app, "Erro", QString("Erro ao exibir diálogo Sobre: %1").arg(e.what()));
    }
}
