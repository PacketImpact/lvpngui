#include "openvpn.h"
#include "vpngui.h"
#include "config.h"

#include <stdexcept>
#include <QDebug>
#include <QTimer>
#include <cryptopp/osrng.h>
#include <QTcpServer>
#include <QCoreApplication>
#include <QApplication>

int pickPort() {
    QTcpServer s;
    s.listen(QHostAddress::LocalHost, 0);
    int p = s.serverPort();
    s.close();
    return p;
}


OpenVPN::OpenVPN(VPNGUI *parent, QString openvpnPath)
    : QObject(parent)
    , m_vpngui(parent)
    , m_openvpnPath(openvpnPath)
    , m_openvpnProc(this)
    , m_status(Disconnected)
    , m_authFailed(false)
    , m_mgmtHost("127.0.0.1")
    , m_mgmtPort(0)
{
    if (parent == nullptr) {
        qDebug() << "OpenVPN(): *parent is required";
        throw std::runtime_error("OpenVPN(): *parent is required");
    }

    QObject::connect(&m_openvpnProc, SIGNAL(readyRead()), this, SLOT(procReadyRead()));
    QObject::connect(&m_openvpnProc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(procError(QProcess::ProcessError)));
    QObject::connect(&m_openvpnProc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(procFinished(int,QProcess::ExitStatus)));

    QObject::connect(&m_mgmtSocket, SIGNAL(readyRead()), this, SLOT(mgmtReadyRead()));
    QObject::connect(&m_mgmtSocket, SIGNAL(connected()), this, SLOT(mgmtConnected()));

    QObject::connect(&m_mgmtSocketTimer, SIGNAL(timeout()), this, SLOT(mgmtTryConnect()));
    m_mgmtSocketTimer.setInterval(100);

    logStatus(QString("%1 - %2 %3").arg(m_vpngui->getDisplayName(),
                                        m_vpngui->getName(),
                                        m_vpngui->getFullVersion()));
}

OpenVPN::~OpenVPN() {
    m_mgmtSocket.close();

    m_openvpnProc.terminate();
    m_openvpnProc.waitForFinished(1000);
    m_openvpnProc.kill();
}

bool OpenVPN::connect(const QString &configPath) {
    m_openvpnLog.clear();
    m_mgmtSocket.close();

    m_mgmtPort = pickPort();
    QString portStr(QString::number(m_mgmtPort));

    m_authFailed = false;
    setStatus(Connecting);

    logStatus(m_openvpnPath);
    logStatus("Management: " + m_mgmtHost + ":" + portStr);
    logStatus("Config: " + configPath);

    QStringList args;

    args.append("--config");
    args.append(configPath);

    args.append("--management");
    args.append(m_mgmtHost);
    args.append(portStr);

    args.append("--management-query-passwords");
    args.append("--auth-retry");
    args.append("interact");

    m_openvpnProc.start(m_openvpnPath, args);

    m_mgmtSocketTimer.start();

    return true;
}

void OpenVPN::disconnect() {
    setStatus(Disconnecting);

    if (m_openvpnProc.state() != QProcess::NotRunning) {
        if (m_mgmtSocket.isOpen()) {
            mgmtSend("signal SIGTERM");
        }
        m_openvpnProc.terminate();
        m_openvpnProc.waitForFinished(1000);
        m_openvpnProc.kill();
    }
}

void OpenVPN::logStatus(const QString &line) {
    qDebug() << "OpenVPN: status:" << line;
    m_openvpnLog.append("# " + line);
    emit logUpdated("# " + line);
}

bool OpenVPN::isUp() const {
    return getStatus() == Connected;
}

OpenVPN::Status OpenVPN::getStatus() const {
    return m_status;
}

void OpenVPN::setStatus(Status s) {
    m_status = s;
    emit statusUpdated(s);
    logStatus(tr("Status:") + " " + getStatusString(s));
}

void OpenVPN::procReadyRead() {
    while (m_openvpnProc.canReadLine()) {
        QString line(QString::fromLocal8Bit(m_openvpnProc.readLine()));

        if (line.endsWith("\r\n")) {
            line = line.left(line.size() - 2);
        }

        m_openvpnLog.append(line);
        qDebug() << "ovpn:" << line;

        emit logUpdated(line);

        if (line.contains("Initialization Sequence Completed")) {
            setStatus(Connected);
        }
    }
}

void OpenVPN::procError(QProcess::ProcessError) {
    logStatus(tr("Error:") + " " + m_openvpnProc.errorString());
}

void OpenVPN::procFinished(int exitCode, QProcess::ExitStatus) {
    logStatus(tr("Finished:") + " code=" + QString::number(exitCode));

    setStatus(Disconnected);
}

const QStringList &OpenVPN::getLog() const {
    return m_openvpnLog;
}

void OpenVPN::mgmtTryConnect() {
    qDebug() << "mgmtTryConnect(): " << m_mgmtSocket.state();

    if (m_mgmtSocket.state() == QTcpSocket::ConnectedState) {
        m_mgmtSocketTimer.stop();
        return;
    }

    // Abort and try again, hoping it's ready this time
    m_mgmtSocket.abort();
    m_mgmtSocket.close();

    m_mgmtSocket.connectToHost(m_mgmtHost, m_mgmtPort);
}

void OpenVPN::mgmtConnected() {
    m_mgmtSocketTimer.stop();
    logStatus("Management socket ready");
    //mgmtSend("state all");
}

void OpenVPN::mgmtReadyRead() {
    if (m_status != Connected && m_status != Connecting) {
        return;
    }
    while (m_mgmtSocket.canReadLine()) {
        QString line(QString::fromLocal8Bit(m_mgmtSocket.readLine()));

        if (line.endsWith("\r\n")) {
            line = line.left(line.size() - 2);
        }

        if (line.isEmpty()) {
            continue;
        }

        qDebug() << "OpenVPN: mgmt:" << line;

        // Logging
        QString logLine;
        if (line.startsWith('>')) {
            logLine = "> " + line.mid(1);
        } else {
            // Display responses with ">>"
            logLine = ">> " + line;
        }

        m_openvpnLog.append(logLine);
        emit logUpdated(logLine);

        // Handling
        if (!line.startsWith('>')) {
            // TODO: to handle command returning things, append that to a
            // QStringList
            continue;
        }
        line = line.mid(1);

        handleManagementCommand(line);
    }
}

void OpenVPN::mgmtSend(const QString &line) {
    m_mgmtSocket.write((line + "\n").toLocal8Bit());
}

void OpenVPN::handleManagementCommand(const QString &line) {
    if (line.startsWith("PASSWORD:Need ")) {
        QStringList parts(line.split('\''));
        if (parts.size() != 3) {
            return;
        }

        VPNCreds c = m_vpngui->handleAuth(m_authFailed);
        while (c.username.isEmpty()) {
            if (m_status != Connected && m_status != Connecting) {
                // "Cancel" button calls disconnect() that sets to Disconnecting
                // openvpn will be killed and the socket too.
                return;
            }
            c = m_vpngui->handleAuth(true);
        }

        QString type(parts[1]);
        mgmtSend("username \"" + type + "\" " + c.username);
        mgmtSend("password \"" + type + "\" " + c.password);
        return;
    }
    if (line.startsWith("PASSWORD:Verification Failed: ")) {
        m_authFailed = true;
    }
}


QString getStatusString(OpenVPN::Status s) {
    QString statusText;
    if (s == OpenVPN::Connected) {
        statusText = QApplication::tr("Connected.");
    } else if (s == OpenVPN::Connecting) {
        statusText = QApplication::tr("Connecting...");
    } else if (s == OpenVPN::Disconnected) {
        statusText = QApplication::tr("Disconnected.");
    } else if (s == OpenVPN::Disconnecting) {
        statusText = QApplication::tr("Disconnecting...");
    } else {
        statusText = "";
    }
    return statusText;
}
