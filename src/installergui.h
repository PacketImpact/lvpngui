#ifndef INSTALLERGUI_H
#define INSTALLERGUI_H

#include <QObject>
#include <QLockFile>

#include "installer.h"

struct SilentError : public std::exception {};

struct InitializationError : public std::exception {
    InitializationError(const QString &title_, const QString &text_)
        : title(title_), text(text_)
    {}
    //~InitializationError() noexcept {}

    virtual const char *what() const noexcept {
        return "Initialization error";
    }

    QString title;
    QString text;
};

class InstallerGUI : public QObject
{
    Q_OBJECT
public:
    explicit InstallerGUI(Installer &installer, QLockFile &lockFile, QObject *parent = nullptr);

    void run();

signals:

public slots:

private:
    Installer &m_installer;
    QLockFile &m_lockFile;
};

#endif // INSTALLERGUI_H
