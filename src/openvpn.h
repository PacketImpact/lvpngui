#ifndef OPENVPN_H
#define OPENVPN_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QProcess>
#include <QTimer>

class VPNGUI;

/*
 * Manages an OpenVPN client
 * Handles management socket & auth & reconnection
 *
 * TODO: Resolve host, put a "remote" line for every IP address in config
 * so when a connection fail it doesnt have to resolve it again.
 * TODO: Handle DNS, make sure it's fixed when OpenVPN fails.
 */
class OpenVPN : public QObject
{
    Q_OBJECT
public:
    enum Status {
        Disconnected,
        Connecting,
        Disconnecting,
        Connected,
    };

    explicit OpenVPN(VPNGUI *parent, QString openvpnPath);
    ~OpenVPN();

    bool connect(const QString &configPath);
    void disconnect();
    const QStringList &getLog() const;

    bool isUp() const;
    Status getStatus() const;

private slots:
    void procReadyRead();
    void procError(QProcess::ProcessError);
    void procFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void mgmtTryConnect();
    void mgmtReadyRead();
    void mgmtConnected();

signals:
    void statusUpdated(OpenVPN::Status s);
    void logUpdated(QString line);

    void connected();
    void disconnected();

private:
    void mgmtSend(const QString &line);
    void handleManagementCommand(const QString &line);
    //QString queryManagement(const QString &command);

    void logStatus(const QString &line);
    void setStatus(Status s);

    VPNGUI *m_vpngui;

    QString m_openvpnPath;
    QProcess m_openvpnProc;
    QStringList m_openvpnLog;

    QString m_username;
    QString m_password;

    Status m_status;
    bool m_authFailed;
    bool m_abort;

    QTcpSocket m_mgmtSocket;
    QString m_mgmtHost;
    int m_mgmtPort;
    QTimer m_mgmtSocketTimer;
};

QString getStatusString(OpenVPN::Status s);

#endif // OPENVPN_H
