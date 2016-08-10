#ifndef INSTALLER_H
#define INSTALLER_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QMap>

class VPNGUI;

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
    Installer(const VPNGUI &vpngui);

    bool isInstalled();
    void install();

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
