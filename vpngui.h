#ifndef VPNGUI_H
#define VPNGUI_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QSettings>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QList>
#include <QString>
#include <QSignalMapper>

#include "installer.h"
#include "openvpn.h"
#include "pwstore.h"
#include "logwindow.h"

struct VPNCreds {
    QString username;
    QString password;

    ~VPNCreds();
    void clear();
};

struct VPNGateway {
    QString display_name;
    QString hostname;
};

/*
 * Main app logic and notifications.
 * Calls everything else at the right time.
 */
class VPNGUI : public QObject
{
    Q_OBJECT
public:
    explicit VPNGUI(QObject *parent = 0);
    ~VPNGUI();

    VPNCreds handleAuth(bool failed=false);

    void queryGateways();
    void updateGatewaysList();
    QString makeOpenVPNConfig(const QString &hostname);
    QStringList safeResolve(const QString &hostname);
    const QSettings &getBrandingSettings() const;

    QString getName() const;
    QString getDisplayName() const;
    QString getFullVersion() const;

signals:

public slots:
    void vpnConnect(QString hostname);
    void vpnDisconnect();
    void gatewaysQueryFinished();
    void openLogWindow();
    void vpnConnected();
    void vpnDisconnected();

private:
    bool readSavedCredentials(VPNCreds &c);
    void saveCredentials(const VPNCreds &c);
    bool promptCredentials(VPNCreds &c);

    QMenu *m_connectMenu;
    QAction *m_disconnectAction;
    QSignalMapper *m_connectMapper;

    QMenu m_trayMenu;
    QSystemTrayIcon m_trayIcon;

    QNetworkReply *m_gatewaysReply;
    QList<VPNGateway> m_gateways;

    QSettings m_brandSettings;
    QSettings m_appSettings;

    QNetworkAccessManager m_qnam;
    Installer m_installer;
    OpenVPN m_openvpn;

    LogWindow *m_logWindow;

    QDir m_configDir;
};

#endif // VPNGUI_H
