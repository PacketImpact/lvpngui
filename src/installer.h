#ifndef INSTALLER_H
#define INSTALLER_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>

class VPNGUI;

bool versionHigherThan(const QString &va, const QString &vb);

/*
 * Manages the local installation.
 * install() extracts ressources somewhere in %APPDATA% (getDir())
 * and isInstalled() checks that. (missing files, integrity, ...)
 * Also puts there the current executable and tries to upgrade a previous
 * installation.
 */
class Installer
{
public:
    enum State {
        NotInstalled,
        Installed,
        HigherVersionFound,
    };

    Installer(const VPNGUI &vpngui);

    State getInstallState();
    State install();
    void installTAP() const;
    void uninstall(bool waitForOpenVPN=true);

    bool setStartOnBoot(bool enabled);

    inline QDir getDir() const { return m_baseDir; }

private:
    QString getArch();
    void loadIndex();

    const VPNGUI &m_vpngui;
    QDir m_baseDir;
    QMap<QString, QString> m_index;
};

#endif // INSTALLER_H
