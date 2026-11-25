#include "ui_01_create_layout.hpp"
#include "../fca_01_FinancialCalculatorAPP.hpp"
#include "../utils/LogManager.hpp"

#include <QHBoxLayout>

std::tuple<QWidget*, QFormLayout*, QVBoxLayout*> FinancialCalculatorApp::create_layout() {
    try {
        auto* widget = new QWidget(this);
        auto* main_h = new QHBoxLayout(widget);

        auto* form_widget = new QWidget(widget);
        auto* form_layout = new QFormLayout(form_widget);
        form_layout->setRowWrapPolicy(QFormLayout::RowWrapPolicy::WrapAllRows);
        form_widget->setMinimumWidth(320);
        main_h->addWidget(form_widget, 0);

        auto* right_widget = new QWidget(widget);
        auto* right_layout = new QVBoxLayout(right_widget);
        right_layout->setContentsMargins(0, 0, 0, 0);
        right_layout->setSpacing(6);
        right_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        main_h->addWidget(right_widget, 1);

        return {widget, form_layout, right_layout};
    } catch (const std::exception& e) {
        LogManager::error(QString("Erro ao criar layout: %1").arg(e.what()), true);
        throw;
    }
}
