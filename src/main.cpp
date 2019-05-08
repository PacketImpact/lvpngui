#include "config.h"
#include "vpngui.h"
#include "installer.h"
#include "installergui.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QLockFile>
#include <QObject>
#include <QCommandLineParser>
#include <stdexcept>
#include <fstream>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    a.setApplicationName(VpnFeatures::name);
    a.setApplicationDisplayName(VpnFeatures::display_name);
    a.setApplicationVersion(VPNGUI_VERSION);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("lvpngui"), QLatin1String("_"), QLatin1String(":/translations"))) {
        a.installTranslator(&translator);
    }

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption uninstallOpt("uninstall");
    parser.addOption(uninstallOpt);
    QCommandLineOption checkInstallOpt("check-install");
    parser.addOption(checkInstallOpt);
    parser.process(a);

    try {
        Installer installer;
        QLockFile lockFile(installer.getDir().filePath("lvpngui.lock"));
        InstallerGUI installerGUI(installer, lockFile);

        if (parser.isSet(uninstallOpt)) {
            installerGUI.runUninstall();
            return 0;
        }
        if (parser.isSet(checkInstallOpt)) {
            installerGUI.runCheckInstall();
            return 0;
        }

        installerGUI.run();

        VPNGUI w(installer);
        return a.exec();
    }
    catch(SilentError) {
        return 1;
    }
    catch(InitializationError &e) {
        qDebug() << "InitializationError: \n" << e.title << "\n" << e.text;
        QMessageBox::warning(nullptr, e.title, e.text);
        return 1;
    }
    catch(std::exception &e) {
        qDebug() << "Exception: \n" << e.what();
        QString text = e.what();
        QMessageBox::critical(nullptr, VPNGUI_NAME " Error", text);
        return 1;
    }
}
