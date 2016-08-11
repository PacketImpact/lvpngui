#include "authdialog.h"
#include "ui_authdialog.h"
#include "vpngui.h"

AuthDialog::AuthDialog(QWidget *parent, const VPNGUI &vpngui)
    : QDialog(parent)
    , ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
    setWindowTitle(vpngui.getDisplayName() + " " + tr("Authentication"));
}

AuthDialog::~AuthDialog() {
    delete ui;
}

QString AuthDialog::getUsername() const {
    return ui->usernameInput->text();
}

QString AuthDialog::getPassword() const {
    return ui->passwordInput->text();
}

bool AuthDialog::getRemember() const {
    return ui->rememberCheckbox->isChecked();
}
