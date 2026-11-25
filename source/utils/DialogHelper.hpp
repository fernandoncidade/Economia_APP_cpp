#ifndef DIALOG_HELPER_HPP
#define DIALOG_HELPER_HPP

#include <QMessageBox>
#include <QString>
#include <QWidget>

namespace DialogHelper {

QMessageBox::StandardButton question(
    QWidget* parent,
    const QString& title,
    const QString& text,
    QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No),
    QMessageBox::StandardButton defaultButton = QMessageBox::Yes
);

void information(
    QWidget* parent,
    const QString& title,
    const QString& text
);

void warning(
    QWidget* parent,
    const QString& title,
    const QString& text
);

void critical(
    QWidget* parent,
    const QString& title,
    const QString& text
);

void traduzir_botoes(QMessageBox* msgBox);

} // namespace DialogHelper

#endif // DIALOG_HELPER_HPP
