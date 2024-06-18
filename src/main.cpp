#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include <QFontDatabase>
#include <QtMath>

#include "clockwidget.h"
#include "vec3.h"
#include "skycolor.h"

class ClockWindow : public QMainWindow {
public:
    ClockWindow(const QList<ClockDef>& timeZones) : mClockWidget(timeZones)  {
        setCentralWidget(&mClockWidget);
    }
private:
    ClockWidget mClockWidget;
};

void dumpTestImage() {
    float t = 0.8;
    float angle = t * M_PI * 0.6;

    Vec3f sunDir(0, qCos(angle), -qSin(angle));
//    auto im = skycolor::renderSkydome(sunDir);
    auto im = skycolor::renderCamera(sunDir);
    im.save("C:/Users/bkorsmit/Documents/SunClock/test.png");
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    dumpTestImage();
    return 0;

    // Font
    QFontDatabase::addApplicationFont(":/DMMono-Regular.ttf");
    QFont font("DM Mono Medium");
    QApplication::setFont(font);

    // Style
    app.setStyleSheet("QMainWindow { background-color: black; }");

    // Display
    const QList<ClockDef> clockDefs({
                                            {"America/Los_Angeles", "LAX"},
                                            {"Europe/Amsterdam",    "AMS"},
                                            {"Asia/Singapore",      "SIN"},
                                            {"Australia/Sydney",    "SYD"},
                                    });

    // Get!
    ClockWindow win(clockDefs);
    win.resize(1280, 720);
    win.show();

    return app.exec();
}
