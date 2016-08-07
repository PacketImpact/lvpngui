#include "installer.h"
#include "config.h"
#include "vpngui.h"

#include <stdexcept>

#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QSysInfo>
#include <QCoreApplication>
#include <QMessageBox>

#ifdef WIN32

#include "windows.h"
#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"

bool createLink(QString linkPath, QString destPath, QString desc) {
    HRESULT hres;
    IShellLink* psl;
    bool success = false;

    wchar_t* lpszPathObj = new wchar_t[destPath.length() + 1];
    destPath.toWCharArray(lpszPathObj);
    lpszPathObj[destPath.length()] = 0;

    wchar_t* lpszPathLink = new wchar_t[linkPath.length() + 1];
    linkPath.toWCharArray(lpszPathLink);
    lpszPathLink[linkPath.length()] = 0;

    wchar_t* lpszDesc = new wchar_t[desc.length() + 1];
    desc.toWCharArray(lpszDesc);
    lpszDesc[desc.length()] = 0;

    CoInitialize(NULL);

    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres)) {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description.
        psl->SetPath(lpszPathObj);
        psl->SetDescription(lpszDesc);

        // Query IShellLink for the IPersistFile interface, used for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres)) {
            // Save the link by calling IPersistFile::Save.
            hres = ppf->Save(lpszPathLink, TRUE);
            ppf->Release();

            success = true;
        }
        psl->Release();
    }

    delete[] lpszPathObj;
    delete[] lpszPathLink;
    delete[] lpszDesc;

    return success;
}

#endif

QByteArray hashFile(QString path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return QByteArray();
    }
    QCryptographicHash hasher(QCryptographicHash::Sha1);
    if (!hasher.addData(&f)) {
        return QByteArray();
    }
    return hasher.result();
}

Installer::Installer(const VPNGUI &vpngui)
    : m_vpngui(vpngui)
{
    m_baseDir = QDir(QDir(qgetenv("APPDATA")).filePath(VPNGUI_ORGNAME "/" + m_vpngui.getName()));
    loadIndex();
}

QString Installer::getArch() {
    QString arch = QSysInfo::currentCpuArchitecture();
    if (arch == "x86_64") {
        return "64";
    }
    if (arch == "i386") {
        return "32";
    }
    throw std::runtime_error("Unsupported arch: " + arch.toStdString());
}

bool Installer::isInstalled() {
    QMap<QString, QString>::iterator it;
    for(it=m_index.begin(); it != m_index.end(); ++it) {
        QString filename = it.key();
        QString hash = it.value();

        QString path = m_baseDir.filePath(filename);
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly)) {
            qDebug() << "Installer: cannot open: " << path;
            return false;
        }
        QCryptographicHash hasher(QCryptographicHash::Sha1);
        if (!hasher.addData(&f)) {
            qDebug() << "Installer: cannot hash: " << path;
            return false;
        }
        QString new_hash = QString(hasher.result().toHex());
        if (new_hash != hash) {
            qDebug() << "Installer: different hash for " << path << ": " << new_hash;
            return false;
        }
    }

    // Check current binary (upgrades)
    QString appSrcPath = QCoreApplication::applicationFilePath();
    QString appLocPath = m_baseDir.filePath(VPNGUI_EXENAME);
    QByteArray appSrcHash = hashFile(appSrcPath);
    QByteArray appLocHash = hashFile(appLocPath);
    if (appSrcHash.length() == 0 || appSrcHash != appLocHash) {
        qDebug() << "Installer: different hash for app binary";
        return false;
    }

    return true;
}

void Installer::install() {
    if (!m_baseDir.mkpath(".")) {
        throw std::runtime_error("Cannot mkdir: " + m_baseDir.path().toStdString());
    }

    QMap<QString, QString>::iterator it;
    for(it=m_index.begin(); it != m_index.end(); ++it) {
        QString filename = it.key();
        QString resPath(":/bin" + getArch() + "/" + filename);
        QString locPath(m_baseDir.filePath(filename));

        QFile resFile(resPath);
        if (!resFile.open(QIODevice::ReadOnly)) {
            throw std::runtime_error("Cannot read file: " + resPath.toStdString() + " -> " + locPath.toStdString());
        }

        QFile locFile(locPath);
        if (!locFile.open(QIODevice::WriteOnly)) {
            throw std::runtime_error("Cannot write file: " + resPath.toStdString() + " -> " + locPath.toStdString());
        }

        locFile.write(resFile.readAll());

        locFile.close();
        resFile.close();
    }

    // Copy current binary there too
    QString appSrcPath = QCoreApplication::applicationFilePath();
    QString appLocPath = m_baseDir.filePath(VPNGUI_EXENAME);
    if (QFile(appLocPath).exists()) {
        while (!QFile(appLocPath).remove()) {
            QMessageBox::StandardButton r;
            r = QMessageBox::warning(NULL, m_vpngui.getDisplayName(),
                                     m_vpngui.getDisplayName() + " is already running. Please close it to upgrade.",
                                     QMessageBox::Cancel | QMessageBox::Ok);
            if (r == QMessageBox::Cancel) {
                break;
            }
        }
        if (QFile(appLocPath).exists() && !QFile(appLocPath).remove()) {
            throw std::runtime_error("Cannot delete for upgrade: " + appLocPath.toStdString());
        }
    }
    if (!QFile::copy(appSrcPath, appLocPath)) {
        throw std::runtime_error("Cannot copy file: " + appSrcPath.toStdString() + " -> " + appLocPath.toStdString());
    }

#ifdef WIN32
    // Make a desktop shortcut
    QMessageBox::StandardButton r = QMessageBox::question(NULL, m_vpngui.getDisplayName(),
                                                          m_vpngui.getDisplayName() + " has been installed. Create a desktop shortcut?");
    if (r == QMessageBox::Yes) {
        QDir homeDir(QString(qgetenv("USERPROFILE")));
        QDir desktopDir(homeDir.filePath("Desktop"));
        QString shortcut(desktopDir.filePath(m_vpngui.getDisplayName() + ".lnk"));
        if (!createLink(shortcut, appLocPath, m_vpngui.getDisplayName())) {
            throw std::runtime_error("Failed to create link: " + shortcut.toStdString() + " -> " + appLocPath.toStdString());
        }
    }
#endif
}

void Installer::loadIndex() {
    QFile index(":/bin" + getArch() + "/index.txt");
    if (!index.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Cannot open index file");
    }
    QTextStream in(&index);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        QString line = in.readLine().toUtf8();
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        QStringList parts = line.split(" ");
        if (parts.size() != 2) {
            throw std::runtime_error("found invalid index entry");
        }
        m_index.insert(parts[1], parts[0]);
    }
    index.close();
}
