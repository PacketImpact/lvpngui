#ifndef INSTALLER_H
#define INSTALLER_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QUuid>

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

    Installer();

    State detectState();
    State install();
    void installTAP() const;
    void uninstall(bool waitForOpenVPN=true);

    bool setStartOnBoot(bool enabled);

    inline QDir getDir() const { return m_baseDir; }
    QUuid getUuid() const;
    QString getGuid() const;

private:
    QString getArch();
    void loadIndex();
    void extractFile(const QString &src, const QString &dst);

    QDir m_baseDir;
    QMap<QString, QString> m_index;
};

#endif // INSTALLER_H
