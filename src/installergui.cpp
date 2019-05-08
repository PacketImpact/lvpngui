#include "installergui.h"
#include "config.h"

#include <QMessageBox>

InstallerGUI::InstallerGUI(Installer &installer, QLockFile &lockFile, QObject *parent)
    : QObject(parent)
    , m_installer(installer)
    , m_lockFile(lockFile)
{}

void InstallerGUI::run() {
    // m_lockFile will need it before install() is called
    if (!m_installer.getDir().exists()) {
        m_installer.getDir().mkpath(".");
    }

    Installer::State installState = m_installer.detectState();

    QString already_running(tr("%1 is already running.").arg(VpnFeatures::name));

    if (installState == Installer::NotInstalled) {
        while (!m_lockFile.tryLock(100)) {
            // If we can't lock:
            // NotInstalled and already running means it's an upgrade.
            // We ask to close the older version so we can do the upgrade
            QString text(tr("Please close it and retry. "));
            auto r = QMessageBox::warning(nullptr, already_running,
                                          already_running + " " + text,
                                          QMessageBox::Cancel | QMessageBox::Retry,
                                          QMessageBox::Retry);

            if (r == QMessageBox::Cancel) {
                throw SilentError();
            }
        }

        m_installer.install();
        QString msg(tr("%1 is now installed! (version %2)"));
        msg = msg.arg(VpnFeatures::display_name, VPNGUI_VERSION);
        QMessageBox::information(nullptr, tr("Installed"), msg, QMessageBox::Ok);
        // TODO: do we start the new process here and throw SilentError()?
    }
    else if (installState == Installer::HigherVersionFound) {
        throw InitializationError(QString(VpnFeatures::name), tr("%1 is already installed with a higher version.").arg(VpnFeatures::name));
    }
    else {
        // The same version is installed
        if (!m_lockFile.tryLock(100)) {
            // The same version is already running.
            throw InitializationError(QString(VpnFeatures::name), already_running);
        }
    }
}
