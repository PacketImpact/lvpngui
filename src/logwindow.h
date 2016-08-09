#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QWidget>

namespace Ui {
class LogWindow;
}

class VPNGUI;
class OpenVPN;

class LogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LogWindow(QWidget *parent, const VPNGUI &vpngui, const OpenVPN &openvpn);
    ~LogWindow();

public slots:
    void newLogLine(const QString &line);
    void copyLog();

private:
    void appendLine(const QString &line);

    const OpenVPN &m_openvpn;
    Ui::LogWindow *ui;
};

#endif // LOGWINDOW_H
