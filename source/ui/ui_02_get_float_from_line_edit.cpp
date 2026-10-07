#include "ui_02_get_float_from_line_edit.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

#include <QLineEdit>
#include <stdexcept>

double FinancialCalculatorApp::get_float_from_line_edit(QLineEdit* line_edit, bool is_percentage, std::optional<double> default_val) {
    try {
        if (!line_edit) {
            if (default_val.has_value()) return default_val.value();
            throw std::invalid_argument("Campo nulo.");
        }

        QString text = line_edit->text().trimmed();
        text.replace(',', '.');

        if (text.isEmpty()) {
            if (default_val.has_value()) {
                return default_val.value();
            }
            throw std::runtime_error("O campo não pode estar vazio.");
        }

        bool ok = false;
        double value = text.toDouble(&ok);
        if (!ok) {
            if (default_val.has_value()) {
                LogManager::warning(QString("Valor inválido no campo, usando padrão: %1").arg(default_val.value()));
                return default_val.value();
            }
            throw std::runtime_error(QString("Não foi possível converter '%1' para número.").arg(text).toStdString());
        }

        return is_percentage ? (value / 100.0) : value;
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao obter float do LineEdit: %1").arg(e.what()), true);
        throw;
    }
}
