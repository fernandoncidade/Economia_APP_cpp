#include "SessionManager.hpp"
#include "CaminhoPersistenteUtils.hpp"
#include "LogManager.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../ui/ui_23_history_container.hpp"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>

namespace SessionManager {

QString get_session_file_path() {
    QDir dir(obter_caminho_persistente());
    return dir.filePath("session_calculations.json");
}

bool has_saved_session() {
    QString path = get_session_file_path();
    QFileInfo fi(path);
    if (!fi.exists() || fi.size() < 10) {
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonObject root = doc.object();
    int total_entries = root.value("total_entries").toInt(0);
    if (total_entries > 0) {
        return true;
    }

    // Fallback: check if any entries array is non-empty
    QJsonObject tabsObj = root.value("tabs").toObject();
    for (auto it = tabsObj.begin(); it != tabsObj.end(); ++it) {
        if (it.key().endsWith("_entries") && it.value().isArray()) {
            if (!it.value().toArray().isEmpty()) {
                return true;
            }
        }
    }

    return false;
}

static void save_le(QJsonObject& obj, const QString& key, QLineEdit* le) {
    if (le) obj[key] = le->text();
}
static void load_le(const QJsonObject& obj, const QString& key, QLineEdit* le) {
    if (le && obj.contains(key)) le->setText(obj[key].toString());
}

static void save_cb(QJsonObject& obj, const QString& key, QComboBox* cb) {
    if (cb) obj[key] = cb->currentIndex();
}
static void load_cb(const QJsonObject& obj, const QString& key, QComboBox* cb) {
    if (cb && obj.contains(key)) cb->setCurrentIndex(obj[key].toInt());
}

static void save_chk(QJsonObject& obj, const QString& key, QCheckBox* chk) {
    if (chk) obj[key] = chk->isChecked();
}
static void load_chk(const QJsonObject& obj, const QString& key, QCheckBox* chk) {
    if (chk && obj.contains(key)) chk->setChecked(obj[key].toBool());
}

bool save_session(FinancialCalculatorApp* app) {
    if (!app) return false;

    try {
        QJsonObject root;
        root["version"] = 1;
        if (app->tabs) {
            root["active_tab"] = app->tabs->currentIndex();
        }

        QJsonObject tabsObj;
        int total_entries = 0;

        auto save_container = [&](const QString& key, HistoryContainer* hc) {
            if (hc) {
                QJsonArray arr = hc->toJsonArray();
                tabsObj[key] = arr;
                total_entries += arr.size();
            }
        };

        // Inputs & History per tab
        // Tab 1: Interest
        QJsonObject in_interest;
        save_le(in_interest, "p", app->interest_p);
        save_le(in_interest, "f", app->interest_f);
        save_le(in_interest, "i", app->interest_i);
        save_le(in_interest, "n", app->interest_n);
        save_cb(in_interest, "calc_type", app->interest_calc_type);
        save_cb(in_interest, "regime", app->interest_regime);
        tabsObj["interest_inputs"] = in_interest;
        save_container("interest_entries", app->interest_result);

        // Tab 2: Annuity
        QJsonObject in_annuity;
        save_le(in_annuity, "p", app->annuity_p);
        save_le(in_annuity, "a", app->annuity_a);
        save_le(in_annuity, "i", app->annuity_i);
        save_le(in_annuity, "n", app->annuity_n);
        save_cb(in_annuity, "calc_type", app->annuity_calc_type);
        save_cb(in_annuity, "type", app->annuity_type);
        tabsObj["annuity_inputs"] = in_annuity;
        save_container("annuity_entries", app->annuity_result);

        // Tab 3: Gradient
        QJsonObject in_grad;
        save_le(in_grad, "p", app->grad_p);
        save_le(in_grad, "a", app->grad_a);
        save_le(in_grad, "g", app->grad_g);
        save_le(in_grad, "i", app->grad_i);
        save_le(in_grad, "n", app->grad_n);
        save_le(in_grad, "k", app->grad_k);
        save_cb(in_grad, "calc_mode", app->grad_calc_mode);
        save_cb(in_grad, "type", app->grad_type);
        tabsObj["grad_inputs"] = in_grad;
        save_container("grad_entries", app->grad_result);

        // Tab 4: Rates
        QJsonObject in_rates;
        save_le(in_rates, "equiv_i", app->rate_equiv_i);
        save_le(in_rates, "equiv_cur_n", app->rate_equiv_current_n);
        save_le(in_rates, "equiv_tgt_n", app->rate_equiv_target_n);
        save_le(in_rates, "real_i", app->rate_real_i);
        save_le(in_rates, "real_inf", app->rate_real_inflation);
        save_le(in_rates, "real_r", app->rate_real_r);
        save_cb(in_rates, "real_calc_type", app->rate_real_calc_type);
        tabsObj["rates_inputs"] = in_rates;
        save_container("rate_equiv_entries", app->rate_equiv_result);
        save_container("rate_real_entries", app->rate_real_result);

        // Tab 5: Amortization
        QJsonObject in_amort;
        save_le(in_amort, "p", app->amort_p);
        save_le(in_amort, "i", app->amort_i);
        save_le(in_amort, "n", app->amort_n);
        save_le(in_amort, "e", app->amort_e);
        save_le(in_amort, "k", app->amort_k);
        save_le(in_amort, "carencia", app->amort_carencia);
        save_cb(in_amort, "system", app->amort_system);
        save_chk(in_amort, "capitalizados", app->amort_juros_capitalizados);
        tabsObj["amort_inputs"] = in_amort;
        save_container("amort_entries", app->amort_result);

        // Tab 6: Investment
        QJsonObject in_invest;
        save_le(in_invest, "initial", app->invest_initial);
        save_le(in_invest, "cashflow", app->invest_cashflow);
        save_le(in_invest, "revenue", app->invest_annual_revenue);
        save_le(in_invest, "cost", app->invest_annual_cost);
        save_le(in_invest, "sensitivity", app->invest_sensitivity_variation);
        save_le(in_invest, "tma", app->invest_tma);
        save_le(in_invest, "n", app->invest_n);
        save_cb(in_invest, "analysis_type", app->invest_analysis_type);
        tabsObj["invest_inputs"] = in_invest;
        save_container("invest_entries", app->invest_result);

        // Tab 7: Depreciation
        QJsonObject in_deprec;
        save_le(in_deprec, "p", app->deprec_p);
        save_le(in_deprec, "vre", app->deprec_vre);
        save_le(in_deprec, "n", app->deprec_n);
        save_le(in_deprec, "k", app->deprec_k);
        save_cb(in_deprec, "method", app->deprec_method);
        tabsObj["deprec_inputs"] = in_deprec;
        save_container("deprec_entries", app->deprec_result);

        // Tab 8: Effective Rate
        QJsonObject in_eff;
        save_le(in_eff, "nominal", app->eff_rate_nominal);
        save_le(in_eff, "p_nom", app->eff_rate_period_nominal);
        save_le(in_eff, "p_cap", app->eff_rate_period_cap);
        save_le(in_eff, "p_tgt", app->eff_rate_period_target);
        save_le(in_eff, "adv_rate", app->adv_int_rate);
        save_le(in_eff, "adv_nom", app->adv_int_nominal);
        save_le(in_eff, "real_glob", app->real_int_global_rate);
        save_le(in_eff, "real_inf", app->real_int_inflation);
        save_le(in_eff, "real_cap", app->real_int_capital);
        save_le(in_eff, "tma_rate", app->tma_rate);
        save_le(in_eff, "tma_per", app->tma_periods);
        save_le(in_eff, "tma_month", app->tma_monthly_rate);
        save_le(in_eff, "tma_cap", app->tma_capital);
        save_le(in_eff, "tax_glob_real", app->tax_global_real);
        save_le(in_eff, "tax_inf_m1", app->tax_global_inf_m1);
        save_le(in_eff, "tax_inf_m2", app->tax_global_inf_m2);
        save_le(in_eff, "tax_inf_m3", app->tax_global_inf_m3);
        save_le(in_eff, "tir_init", app->tir_initial);
        save_le(in_eff, "tir_ret", app->tir_return);
        save_le(in_eff, "tir_per", app->tir_periods);
        save_le(in_eff, "tirm_init", app->tirm_initial);
        save_le(in_eff, "tirm_ret", app->tirm_return);
        save_le(in_eff, "tirm_per", app->tirm_periods);
        save_le(in_eff, "tirm_cap", app->tirm_cap_rate);
        save_cb(in_eff, "calc_mode", app->eff_rate_calc_mode);
        tabsObj["eff_inputs"] = in_eff;
        save_container("eff_rate_entries", app->eff_rate_result);

        // Tab 9: Minimum Return
        QJsonObject in_min;
        save_le(in_min, "invest", app->min_return_investment);
        save_le(in_min, "tma", app->min_return_tma);
        save_le(in_min, "per", app->min_return_periods);
        tabsObj["min_return_inputs"] = in_min;
        save_container("min_return_entries", app->min_return_result);

        // Tab 10: Fisher
        QJsonObject in_fisher;
        save_le(in_fisher, "real", app->fisher_tma_real);
        save_le(in_fisher, "nom", app->fisher_tma_nominal);
        save_le(in_fisher, "inf", app->fisher_inflation);
        save_cb(in_fisher, "calc_type", app->fisher_calc_type);
        tabsObj["fisher_inputs"] = in_fisher;
        save_container("fisher_entries", app->fisher_result);

        // Tab 11: VPL Tax
        QJsonObject in_vpl;
        save_le(in_vpl, "invest", app->vpl_tax_investment);
        save_le(in_vpl, "profit", app->vpl_tax_annual_profit);
        save_le(in_vpl, "life", app->vpl_tax_useful_life);
        save_le(in_vpl, "irpj", app->vpl_tax_irpj);
        save_le(in_vpl, "csll", app->vpl_tax_csll);
        save_le(in_vpl, "tma", app->vpl_tax_tma);
        save_le(in_vpl, "fin_rate", app->vpl_tax_finance_rate);
        save_le(in_vpl, "fin_per", app->vpl_tax_finance_periods);
        save_le(in_vpl, "res_val", app->vpl_tax_residual_value);
        save_le(in_vpl, "sale_yr", app->vpl_tax_sale_year);
        save_le(in_vpl, "sale_val", app->vpl_tax_sale_value);
        save_chk(in_vpl, "financed", app->vpl_tax_financed);
        tabsObj["vpl_tax_inputs"] = in_vpl;
        save_container("vpl_tax_entries", app->vpl_tax_result);

        // Tab 12: CAUE
        QJsonObject in_caue;
        save_le(in_caue, "init_cost", app->caue_initial_cost);
        save_le(in_caue, "tma", app->caue_tma);
        save_le(in_caue, "max_years", app->caue_max_years);
        if (app->caue_input_table) {
            QJsonObject caueTbl;
            int rows = app->caue_input_table->rowCount();
            caueTbl["rows"] = rows;
            QJsonArray rowArr;
            for (int r = 0; r < rows; ++r) {
                QJsonObject rowObj;
                auto* it0 = app->caue_input_table->item(r, 0);
                auto* it1 = app->caue_input_table->item(r, 1);
                auto* it2 = app->caue_input_table->item(r, 2);
                rowObj["ano"] = it0 ? it0->text() : "";
                rowObj["vr"] = it1 ? it1->text() : "";
                rowObj["com"] = it2 ? it2->text() : "";
                rowArr.append(rowObj);
            }
            caueTbl["data"] = rowArr;
            in_caue["input_table"] = caueTbl;
        }
        tabsObj["caue_inputs"] = in_caue;
        save_container("caue_entries", app->caue_result);

        if (total_entries == 0) {
            clear_session();
            return true;
        }

        root["total_entries"] = total_entries;
        root["tabs"] = tabsObj;

        QString path = get_session_file_path();
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QJsonDocument doc(root);
            file.write(doc.toJson(QJsonDocument::Indented));
            file.close();
            LogManager::info(QString("Sessão salva com sucesso (%1 cálculos salvos)").arg(total_entries));
            return true;
        } else {
            LogManager::error("Falha ao abrir arquivo para salvar sessão: " + path);
            return false;
        }
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao salvar sessão: %1").arg(e.what()));
        return false;
    }
}

bool load_session(FinancialCalculatorApp* app) {
    if (!app) return false;

    try {
        QString path = get_session_file_path();
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data, &err);
        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            return false;
        }

        QJsonObject root = doc.object();
        QJsonObject tabsObj = root.value("tabs").toObject();

        auto load_container = [&](const QString& key, HistoryContainer* hc) {
            if (hc && tabsObj.contains(key) && tabsObj[key].isArray()) {
                hc->loadFromJsonArray(tabsObj[key].toArray());
            }
        };

        // Tab 1: Interest
        if (tabsObj.contains("interest_inputs")) {
            QJsonObject in = tabsObj["interest_inputs"].toObject();
            load_le(in, "p", app->interest_p);
            load_le(in, "f", app->interest_f);
            load_le(in, "i", app->interest_i);
            load_le(in, "n", app->interest_n);
            load_cb(in, "calc_type", app->interest_calc_type);
            load_cb(in, "regime", app->interest_regime);
        }
        load_container("interest_entries", app->interest_result);

        // Tab 2: Annuity
        if (tabsObj.contains("annuity_inputs")) {
            QJsonObject in = tabsObj["annuity_inputs"].toObject();
            load_le(in, "p", app->annuity_p);
            load_le(in, "a", app->annuity_a);
            load_le(in, "i", app->annuity_i);
            load_le(in, "n", app->annuity_n);
            load_cb(in, "calc_type", app->annuity_calc_type);
            load_cb(in, "type", app->annuity_type);
        }
        load_container("annuity_entries", app->annuity_result);

        // Tab 3: Gradient
        if (tabsObj.contains("grad_inputs")) {
            QJsonObject in = tabsObj["grad_inputs"].toObject();
            load_le(in, "p", app->grad_p);
            load_le(in, "a", app->grad_a);
            load_le(in, "g", app->grad_g);
            load_le(in, "i", app->grad_i);
            load_le(in, "n", app->grad_n);
            load_le(in, "k", app->grad_k);
            load_cb(in, "calc_mode", app->grad_calc_mode);
            load_cb(in, "type", app->grad_type);
        }
        load_container("grad_entries", app->grad_result);

        // Tab 4: Rates
        if (tabsObj.contains("rates_inputs")) {
            QJsonObject in = tabsObj["rates_inputs"].toObject();
            load_le(in, "equiv_i", app->rate_equiv_i);
            load_le(in, "equiv_cur_n", app->rate_equiv_current_n);
            load_le(in, "equiv_tgt_n", app->rate_equiv_target_n);
            load_le(in, "real_i", app->rate_real_i);
            load_le(in, "real_inf", app->rate_real_inflation);
            load_le(in, "real_r", app->rate_real_r);
            load_cb(in, "real_calc_type", app->rate_real_calc_type);
        }
        load_container("rate_equiv_entries", app->rate_equiv_result);
        load_container("rate_real_entries", app->rate_real_result);

        // Tab 5: Amortization
        if (tabsObj.contains("amort_inputs")) {
            QJsonObject in = tabsObj["amort_inputs"].toObject();
            load_le(in, "p", app->amort_p);
            load_le(in, "i", app->amort_i);
            load_le(in, "n", app->amort_n);
            load_le(in, "e", app->amort_e);
            load_le(in, "k", app->amort_k);
            load_le(in, "carencia", app->amort_carencia);
            load_cb(in, "system", app->amort_system);
            load_chk(in, "capitalizados", app->amort_juros_capitalizados);
        }
        load_container("amort_entries", app->amort_result);

        // Tab 6: Investment
        if (tabsObj.contains("invest_inputs")) {
            QJsonObject in = tabsObj["invest_inputs"].toObject();
            load_le(in, "initial", app->invest_initial);
            load_le(in, "cashflow", app->invest_cashflow);
            load_le(in, "revenue", app->invest_annual_revenue);
            load_le(in, "cost", app->invest_annual_cost);
            load_le(in, "sensitivity", app->invest_sensitivity_variation);
            load_le(in, "tma", app->invest_tma);
            load_le(in, "n", app->invest_n);
            load_cb(in, "analysis_type", app->invest_analysis_type);
        }
        load_container("invest_entries", app->invest_result);

        // Tab 7: Depreciation
        if (tabsObj.contains("deprec_inputs")) {
            QJsonObject in = tabsObj["deprec_inputs"].toObject();
            load_le(in, "p", app->deprec_p);
            load_le(in, "vre", app->deprec_vre);
            load_le(in, "n", app->deprec_n);
            load_le(in, "k", app->deprec_k);
            load_cb(in, "method", app->deprec_method);
        }
        load_container("deprec_entries", app->deprec_result);

        // Tab 8: Effective Rate
        if (tabsObj.contains("eff_inputs")) {
            QJsonObject in = tabsObj["eff_inputs"].toObject();
            load_le(in, "nominal", app->eff_rate_nominal);
            load_le(in, "p_nom", app->eff_rate_period_nominal);
            load_le(in, "p_cap", app->eff_rate_period_cap);
            load_le(in, "p_tgt", app->eff_rate_period_target);
            load_le(in, "adv_rate", app->adv_int_rate);
            load_le(in, "adv_nom", app->adv_int_nominal);
            load_le(in, "real_glob", app->real_int_global_rate);
            load_le(in, "real_inf", app->real_int_inflation);
            load_le(in, "real_cap", app->real_int_capital);
            load_le(in, "tma_rate", app->tma_rate);
            load_le(in, "tma_per", app->tma_periods);
            load_le(in, "tma_month", app->tma_monthly_rate);
            load_le(in, "tma_cap", app->tma_capital);
            load_le(in, "tax_glob_real", app->tax_global_real);
            load_le(in, "tax_inf_m1", app->tax_global_inf_m1);
            load_le(in, "tax_inf_m2", app->tax_global_inf_m2);
            load_le(in, "tax_inf_m3", app->tax_global_inf_m3);
            load_le(in, "tir_init", app->tir_initial);
            load_le(in, "tir_ret", app->tir_return);
            load_le(in, "tir_per", app->tir_periods);
            load_le(in, "tirm_init", app->tirm_initial);
            load_le(in, "tirm_ret", app->tirm_return);
            load_le(in, "tirm_per", app->tirm_periods);
            load_le(in, "tirm_cap", app->tirm_cap_rate);
            load_cb(in, "calc_mode", app->eff_rate_calc_mode);
        }
        load_container("eff_rate_entries", app->eff_rate_result);

        // Tab 9: Minimum Return
        if (tabsObj.contains("min_return_inputs")) {
            QJsonObject in = tabsObj["min_return_inputs"].toObject();
            load_le(in, "invest", app->min_return_investment);
            load_le(in, "tma", app->min_return_tma);
            load_le(in, "per", app->min_return_periods);
        }
        load_container("min_return_entries", app->min_return_result);

        // Tab 10: Fisher
        if (tabsObj.contains("fisher_inputs")) {
            QJsonObject in = tabsObj["fisher_inputs"].toObject();
            load_le(in, "real", app->fisher_tma_real);
            load_le(in, "nom", app->fisher_tma_nominal);
            load_le(in, "inf", app->fisher_inflation);
            load_cb(in, "calc_type", app->fisher_calc_type);
        }
        load_container("fisher_entries", app->fisher_result);

        // Tab 11: VPL Tax
        if (tabsObj.contains("vpl_tax_inputs")) {
            QJsonObject in = tabsObj["vpl_tax_inputs"].toObject();
            load_le(in, "invest", app->vpl_tax_investment);
            load_le(in, "profit", app->vpl_tax_annual_profit);
            load_le(in, "life", app->vpl_tax_useful_life);
            load_le(in, "irpj", app->vpl_tax_irpj);
            load_le(in, "csll", app->vpl_tax_csll);
            load_le(in, "tma", app->vpl_tax_tma);
            load_le(in, "fin_rate", app->vpl_tax_finance_rate);
            load_le(in, "fin_per", app->vpl_tax_finance_periods);
            load_le(in, "res_val", app->vpl_tax_residual_value);
            load_le(in, "sale_yr", app->vpl_tax_sale_year);
            load_le(in, "sale_val", app->vpl_tax_sale_value);
            load_chk(in, "financed", app->vpl_tax_financed);
        }
        load_container("vpl_tax_entries", app->vpl_tax_result);

        // Tab 12: CAUE
        if (tabsObj.contains("caue_inputs")) {
            QJsonObject in = tabsObj["caue_inputs"].toObject();
            load_le(in, "init_cost", app->caue_initial_cost);
            load_le(in, "tma", app->caue_tma);
            load_le(in, "max_years", app->caue_max_years);
            if (app->caue_input_table && in.contains("input_table")) {
                QJsonObject caueTbl = in["input_table"].toObject();
                int rows = caueTbl["rows"].toInt();
                QJsonArray rowArr = caueTbl["data"].toArray();
                app->caue_input_table->setRowCount(rows);
                for (int r = 0; r < rows; ++r) {
                    QJsonObject rowObj = rowArr.at(r).toObject();
                    app->caue_input_table->setItem(r, 0, new QTableWidgetItem(rowObj["ano"].toString()));
                    app->caue_input_table->setItem(r, 1, new QTableWidgetItem(rowObj["vr"].toString()));
                    app->caue_input_table->setItem(r, 2, new QTableWidgetItem(rowObj["com"].toString()));
                }
            }
        }
        load_container("caue_entries", app->caue_result);

        if (app->tabs && root.contains("active_tab")) {
            app->tabs->setCurrentIndex(root["active_tab"].toInt());
        }

        LogManager::info("Sessão anterior carregada com sucesso na interface.");
        return true;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao carregar sessão anterior: %1").arg(e.what()));
        return false;
    }
}

void clear_session() {
    QString path = get_session_file_path();
    QFile file(path);
    if (file.exists()) {
        file.remove();
        LogManager::info("Arquivo de sessão anterior removido com sucesso: " + path);
    }
}

} // namespace SessionManager
