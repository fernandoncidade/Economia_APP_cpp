#include "DialogHelper.hpp"
#include "../language/tr_01_gerenciadorTraducao.hpp"
#include <QPushButton>

namespace DialogHelper {

void traduzir_botoes(QMessageBox* msgBox) {
    if (!msgBox) return;
    bool is_en = (GerenciadorTraducao::idioma_atual_global() == "en_US");

    if (auto* btn = msgBox->button(QMessageBox::Yes)) {
        btn->setText(is_en ? "Yes" : "Sim");
    }
    if (auto* btn = msgBox->button(QMessageBox::No)) {
        btn->setText(is_en ? "No" : "Não");
    }
    if (auto* btn = msgBox->button(QMessageBox::Ok)) {
        btn->setText("OK");
    }
    if (auto* btn = msgBox->button(QMessageBox::Cancel)) {
        btn->setText(is_en ? "Cancel" : "Cancelar");
    }
    if (auto* btn = msgBox->button(QMessageBox::Close)) {
        btn->setText(is_en ? "Close" : "Fechar");
    }
}

QMessageBox::StandardButton question(
    QWidget* parent,
    const QString& title,
    const QString& text,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton
) {
    bool is_en = (GerenciadorTraducao::idioma_atual_global() == "en_US");
    QMessageBox msgBox(QMessageBox::Question, title, text, QMessageBox::NoButton, parent);

    QPushButton* yesBtn = nullptr;
    QPushButton* noBtn = nullptr;
    QPushButton* okBtn = nullptr;
    QPushButton* cancelBtn = nullptr;

    if (buttons & QMessageBox::Yes) {
        yesBtn = msgBox.addButton(is_en ? "Yes" : "Sim", QMessageBox::YesRole);
    }
    if (buttons & QMessageBox::No) {
        noBtn = msgBox.addButton(is_en ? "No" : "Não", QMessageBox::NoRole);
    }
    if (buttons & QMessageBox::Ok) {
        okBtn = msgBox.addButton("OK", QMessageBox::AcceptRole);
    }
    if (buttons & QMessageBox::Cancel) {
        cancelBtn = msgBox.addButton(is_en ? "Cancel" : "Cancelar", QMessageBox::RejectRole);
    }

    if (defaultButton == QMessageBox::Yes && yesBtn) {
        msgBox.setDefaultButton(yesBtn);
    } else if (defaultButton == QMessageBox::No && noBtn) {
        msgBox.setDefaultButton(noBtn);
    } else if (defaultButton == QMessageBox::Ok && okBtn) {
        msgBox.setDefaultButton(okBtn);
    }

    msgBox.exec();

    QAbstractButton* clicked = msgBox.clickedButton();
    if (clicked == yesBtn) return QMessageBox::Yes;
    if (clicked == noBtn) return QMessageBox::No;
    if (clicked == okBtn) return QMessageBox::Ok;
    if (clicked == cancelBtn) return QMessageBox::Cancel;
    return QMessageBox::NoButton;
}

void information(
    QWidget* parent,
    const QString& title,
    const QString& text
) {
    QMessageBox msgBox(QMessageBox::Information, title, text, QMessageBox::NoButton, parent);
    msgBox.addButton("OK", QMessageBox::AcceptRole);
    msgBox.exec();
}

void warning(
    QWidget* parent,
    const QString& title,
    const QString& text
) {
    QMessageBox msgBox(QMessageBox::Warning, title, text, QMessageBox::NoButton, parent);
    msgBox.addButton("OK", QMessageBox::AcceptRole);
    msgBox.exec();
}

void critical(
    QWidget* parent,
    const QString& title,
    const QString& text
) {
    QMessageBox msgBox(QMessageBox::Critical, title, text, QMessageBox::NoButton, parent);
    msgBox.addButton("OK", QMessageBox::AcceptRole);
    msgBox.exec();
}

} // namespace DialogHelper
