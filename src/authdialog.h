#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>

class VPNGUI;

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QWidget *parent, const VPNGUI &vpngui);
    ~AuthDialog();

    QString getUsername() const;
    QString getPassword() const;

private:
    Ui::AuthDialog *ui;
};

#endif // AUTHDIALOG_H
