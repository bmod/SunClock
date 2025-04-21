#include <QApplication>
#include <QFontDatabase>
#include <QCommandLineParser>
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

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("fullscreen", "Start application in fullscreen, on by default.", "fullscreen", "1"));
    if (!parser.parse(app.arguments()))
        qFatal(parser.errorText().toLatin1());
    const auto fullscreenResult = parser.value("fullscreen");
    bool ok;
    const bool fullscreen = static_cast<bool>(fullscreenResult.toInt(&ok));
    if (!ok)
        qFatal("Failed to interpret fullscreen argument");

    MainWindow win(conf);

    if (fullscreen) {
        win.showFullScreen();
    } else {
        win.resize(800, 480);
        win.show();
    }

    return app.exec();
}
