#include "logwindow.h"
#include "ui_logwindow.h"
#include "vpngui.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>

LogWindow::LogWindow(QWidget *parent, const VPNGUI &vpngui, const OpenVPN &openvpn)
    : QWidget(parent)
    , m_openvpn(openvpn)
    , ui(new Ui::LogWindow)
{
    ui->setupUi(this);
    setWindowTitle(vpngui.getDisplayName() + " " + tr("Log"));

    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->copyButton, SIGNAL(clicked(bool)), this, SLOT(copyLog()));

    connect(&openvpn, SIGNAL(logUpdated(QString)), this, SLOT(newLogLine(QString)));
    connect(&openvpn, SIGNAL(statusUpdated(OpenVPN::Status)), this, SLOT(statusUpdated(OpenVPN::Status)));

    foreach (QString line, m_openvpn.getLog()) {
        appendLine(line);
    }

    statusUpdated(openvpn.getStatus());
}

LogWindow::~LogWindow()
{
    delete ui;
}

void LogWindow::appendLine(const QString &line) {
    if (line.startsWith('#')) {
        ui->log->setTextColor(QColor("#eeeeee"));
        ui->log->append(line);
    } else if (line.startsWith('>')) {
        ui->log->setTextColor(QColor("#aaaaaa"));
        ui->log->append(line);
    } else {
        ui->log->setTextColor(QColor("#ffbf00"));
        ui->log->append(line);
    }
}

void LogWindow::newLogLine(const QString &line) {
    appendLine(line);
}

void LogWindow::statusUpdated(OpenVPN::Status s) {
    ui->statusLabel->setText(getStatusString(s));
}

void LogWindow::copyLog() {
    QClipboard *clipboard = QApplication::clipboard();

    QString plaintextLog;
    foreach (QString line, m_openvpn.getLog()) {
        plaintextLog.append(line + "\n");
    }

    clipboard->setText(plaintextLog);
}
