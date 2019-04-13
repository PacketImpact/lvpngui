#include "config.h"
#include "vpngui.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
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

    try {
        VPNGUI w;
        return a.exec();
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
