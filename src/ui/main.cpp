#include "mainwindow.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <iostream>

int main( int argc, char *argv[] ) {
    QApplication app(argc, argv);

    QTranslator translator;
    const QStringList uiLangs = QLocale::system().uiLanguages();

    for ( const QString& locale : uiLangs ) {
        const QString baseName = "untitled_" + QLocale(locale).name();
        if (translator.load(":" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    MainWindow window;
    
    window.show();

    return app.exec();
}