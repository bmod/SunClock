#include <iostream>

#include <QtDebug>
#include <QApplication>
#include <QMainWindow>
#include <QFontDatabase>
#include <QtMath>

#include "clockwidget.h"
#include "vec3.h"
#include "skycolor.h"
#include "csv.h"
#include "airportsdata.h"
#include "suncalc.h"

class ClockWindow : public QMainWindow {
public:
    ClockWindow(const QList<ClockDef>& timeZones) : mClockWidget(timeZones) {
        setCentralWidget(&mClockWidget);
    }

private:
    ClockWidget mClockWidget;
};

qreal toDecimalHour(const QTime& time) {
    const qreal h = time.hour();
    const qreal m = time.minute() / 60.0;
    const qreal s = time.second() / (60.0 * 60.0);
    const qreal ms = time.msec() / (60.0 * 60.0 * 1000.0);
    return h + m + s + ms;
}

void dumpTestImage() {
    float t = -0.5;
    float angle = t * M_PI * 0.6f;

    airportsdata::AirportsData apData(":/airports.csv");
    auto airport = apData.airport("LAX");

    auto dt = QDateTime::currentDateTimeUtc().toTimeZone(airport->timezone);
    auto localHours = toDecimalHour(dt.time());
    auto utcOffset = dt.offsetFromUtc() / (60.0 * 60.0 * 24.0);
    qDebug() << "Timezone" << airport->timezone.displayName(dt);
    qDebug() << "lat:" << airport->latitude << "long:" << airport->longitude;
    qDebug() << "Timespec" << dt.timeSpec();
    qDebug() << "UTC Offset:" << utcOffset;
//    auto coords = suncalc::sunCoordinates(
//            localHours,
//            airport->latitude,
//            airport->longitude,
//            airport->timezone.offsetFromUtc(dt),
//            dt.date().month(),
//            dt.date().day(),
//            dt.date().year()
//    );

    auto coords = suncalc::sunCoordinates(
            22.5,
            33.9425,
            -118.408,
            -6,
            6,
            18,
            2024
    );

    qDebug() << "elevation:" << qRadiansToDegrees(coords.elevation) << "azimuth:" << qRadiansToDegrees(coords.azimuth);

    auto sunDir = suncalc::sunVector(coords);

//    Vec3f sunDir(0, cos(angle), -sin(angle));
//    auto im = skycolor::renderSkydome(sunDir);
    auto im = skycolor::renderCamera(sunDir, {128, 128});
    im.save("C:/Users/bkorsmit/Documents/SunClock/test.png");
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);


    // Regions
    const QList<ClockDef> clockDefs({
                                            {"America/Los_Angeles", "LAX"},
                                            {"Europe/Amsterdam",    "AMS"},
                                            {"Asia/Singapore",      "SIN"},
                                            {"Australia/Sydney",    "SYD"},
                                    });


    dumpTestImage();
    return 0;

    // Font
    QFontDatabase::addApplicationFont(":/DMMono-Medium.ttf");
    QFont font("DM Mono Medium");
    QApplication::setFont(font);

    // Style
    app.setStyleSheet("QMainWindow { background-color: black; }");


    // Get!
    ClockWindow win(clockDefs);
    win.resize(1280, 720);
    win.show();

    return app.exec();
}
