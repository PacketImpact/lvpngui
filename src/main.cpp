#include "vpngui.h"
#include "config.h"
#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <stdexcept>
#include <fstream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("lvpngui"), QLatin1String("_"), QLatin1String(":/translations"))) {
        a.installTranslator(&translator);
    }


    try {
        VPNGUI w;
        return a.exec();
    }
    catch(std::exception &e) {
        qDebug() << "Exception: \n" << e.what();
        QString text = e.what();
        QMessageBox::critical(NULL, VPNGUI_NAME " Error", text);
        return 1;
    }
}
