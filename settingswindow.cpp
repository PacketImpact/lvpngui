#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "vpngui.h"
#include "config.h"

#include <QSet>

SettingsWindow::SettingsWindow(QWidget *parent, const VPNGUI &vpngui, QSettings &appSettings)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
    , m_vpngui(vpngui)
    , m_appSettings(appSettings)
{
    ui->setupUi(this);

    setWindowTitle(vpngui.getDisplayName() + " Settings");

    ui->aboutTitleLabel->setText(vpngui.getDisplayName());
    ui->aboutVersionLabel->setText("Version " + vpngui.getFullVersion());

    QString aboutBaseText("<a href='" VPNGUI_URL "'>" VPNGUI_DISPLAY_NAME "</a>");
    aboutBaseText.append(" by ");
    aboutBaseText.append("<a href='" VPNGUI_ORGURL "'>" VPNGUI_ORGNAME "</a>");
    ui->aboutBaseLabel->setText(aboutBaseText);

    QString style("QLabel a { color: black; }");
    ui->aboutTitleLabel->setStyleSheet(style);
    ui->aboutVersionLabel->setStyleSheet(style);
    ui->aboutBaseLabel->setStyleSheet(style);

    connect(ui->cancelButton, SIGNAL(released()), this, SLOT(close()));
    connect(ui->saveButton, SIGNAL(released()), this, SLOT(saveAndClose()));

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

    // Advanced settings
    ui->httpProxyEdit->setText(m_appSettings.value("http_proxy").toString());
    ui->dnsAPIEdit->setText(m_appSettings.value("dns_api").toString());
    ui->dnsSystemEdit->setText(m_appSettings.value("dns_system").toString());
}

void SettingsWindow::saveSettings() {
    // TODO: check input validity
    const QSettings &providerSettings(m_vpngui.getBrandingSettings());

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

    // Advanced settings
    m_appSettings.setValue("http_proxy", ui->httpProxyEdit->text());
    m_appSettings.setValue("dns_api", ui->dnsAPIEdit->text());
    m_appSettings.setValue("dns_system", ui->dnsSystemEdit->text());
}
