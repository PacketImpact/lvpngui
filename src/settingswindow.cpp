#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "vpngui.h"
#include "config.h"

#include <QSet>


bool changed(const QVariant &a, const QVariant &b) {
    // Very basic but may be enough.
    return a.toString() != b.toString();
}

QSet<QString> settingsDiff(const QHash<QString, QVariant> &a, const QSettings &b) {
    QSet<QString> keys;

    foreach (QString k, a.keys()) {
        if (changed(a.value(k, QVariant()), b.value(k))) {
            keys.insert(k);
        }
    }
    foreach (QString k, b.allKeys()) {
        if (changed(a.value(k, QVariant()), b.value(k))) {
            keys.insert(k);
        }
    }

    return keys;
}

SettingsWindow::SettingsWindow(QWidget *parent, const VPNGUI &vpngui, QSettings &appSettings)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
    , m_vpngui(vpngui)
    , m_appSettings(appSettings)
{
    ui->setupUi(this);

    setWindowTitle(vpngui.getDisplayName() + " " + tr("Settings"));

    ui->aboutTitleLabel->setText(vpngui.getDisplayName());
    ui->aboutVersionLabel->setText(tr("Version %1").arg(vpngui.getFullVersion()));

    QString aboutBaseText(tr("%1 by %2"));
    aboutBaseText = aboutBaseText.arg("<a href='" VPNGUI_URL "'>" VPNGUI_DISPLAY_NAME "</a>",
                                      "<a href='" VPNGUI_ORGURL "'>" VPNGUI_ORGNAME "</a>");
    ui->aboutBaseLabel->setText(aboutBaseText);

    QString style("QLabel a { color: black; }");
    ui->aboutTitleLabel->setStyleSheet(style);
    ui->aboutVersionLabel->setStyleSheet(style);
    ui->aboutBaseLabel->setStyleSheet(style);

    connect(ui->cancelButton, SIGNAL(released()), this, SLOT(close()));
    connect(ui->saveButton, SIGNAL(released()), this, SLOT(saveAndClose()));
    connect(ui->uninstallButton, SIGNAL(released()), &m_vpngui, SLOT(confirmUninstall()));

    loadSettings();
}

SettingsWindow::~SettingsWindow() {
    delete ui;
}

void SettingsWindow::saveAndClose() {
    saveSettings();
    close();
}

void SettingsWindow::loadSettings() {
    const QSettings &providerSettings(m_vpngui.getBrandingSettings());
    QString currentProtocol(getCurrentProtocol(providerSettings, m_appSettings));

    // Protocol radio
    if (currentProtocol == "udp") {
        ui->protoUDP->setChecked(true);
    } else if (currentProtocol == "udpl") {
        ui->protoUDPL->setChecked(true);
    } else if (currentProtocol == "tcp") {
        ui->protoTCP->setChecked(true);
    }

    // IPv6
    if (providerSettings.value("enable_ipv6", true).toBool()) {
        ui->ipv6Checkbox->setChecked(m_appSettings.value("ipv6_tunnel", true).toBool());
    } else {
        ui->ipv6Checkbox->setVisible(false);
    }

    ui->startOnBootCheckbox->setChecked(m_appSettings.value("start_on_boot", false).toBool());

    // Autoconnect
    QString autoconnectSelected(m_appSettings.value("autoconnect").toString());
    ui->autoconnectBox->clear();
    ui->autoconnectBox->addItem("- " + tr("Disabled") + " -", QVariant(""));
    if (!m_vpngui.getGatewayList().isEmpty()) {
        foreach (VPNGateway gw, m_vpngui.getGatewayList()) {
            ui->autoconnectBox->addItem(gw.display_name, gw.hostname);
            if (autoconnectSelected == gw.hostname) {
                ui->autoconnectBox->setCurrentText(gw.display_name);
            }
        }
    } else {
        ui->autoconnectBox->setEnabled(false);
    }

    // Advanced settings
    ui->httpProxyEdit->setText(m_appSettings.value("http_proxy").toString());
    ui->dnsAPIEdit->setText(m_appSettings.value("dns_api").toString());
    ui->dnsSystemEdit->setText(m_appSettings.value("dns_system").toString());
}

void SettingsWindow::saveSettings() {
    // TODO: check input validity
    const QSettings &providerSettings(m_vpngui.getBrandingSettings());

    QHash<QString, QVariant> previousSettings;
    foreach (QString key, m_appSettings.allKeys()) {
        previousSettings[key] = m_appSettings.value(key);
    }


    // Protocol radio
    QString currentProtocol;
    if (ui->protoUDP->isChecked()) {
        currentProtocol = "udp";
    } else if (ui->protoUDPL->isChecked()) {
        currentProtocol = "udpl";
    } else if (ui->protoTCP->isChecked()) {
        currentProtocol = "tcp";
    }
    m_appSettings.setValue("protocol", currentProtocol);

    // IPv6
    if (providerSettings.value("enable_ipv6", true).toBool()) {
        m_appSettings.setValue("ipv6_tunnel", ui->ipv6Checkbox->isChecked());
    }

    m_appSettings.setValue("start_on_boot", ui->startOnBootCheckbox->isChecked());

    // Autoconnect
    m_appSettings.setValue("autoconnect", ui->autoconnectBox->currentData());

    // Advanced settings
    m_appSettings.setValue("http_proxy", ui->httpProxyEdit->text());
    m_appSettings.setValue("dns_api", ui->dnsAPIEdit->text());
    m_appSettings.setValue("dns_system", ui->dnsSystemEdit->text());

    QSet<QString> diff(settingsDiff(previousSettings, m_appSettings));
    emit settingsChanged(diff);
}
