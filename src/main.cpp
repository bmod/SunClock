#include <QApplication>
#include <QFontDatabase>
#include <QFile>

#include "config.h"
#include "mainwindow.h"


void applyStyle(QApplication& app) {
    QFile cssFile(":/style.css");
    if (!cssFile.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal("Failed to open css");

    app.setStyleSheet(cssFile.readAll());
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QFontDatabase::addApplicationFont(":/DMMono-Light.ttf");
    QFontDatabase::addApplicationFont(":/DMMono-Medium.ttf");
    QFontDatabase::addApplicationFont(":/DMMono-Regular.ttf");

    applyStyle(app);

    Config conf;

    MainWindow win(conf);
    // win.resize(800, 480);
    // win.show();
    win.showFullScreen();
    return app.exec();
}
