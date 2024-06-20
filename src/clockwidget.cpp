#include "clockwidget.h"
#include <QPainter>
#include <QThreadPool>
#include <QtDebug>
#include <skycolor.h>
#include <suncalc.h>

Cellwidget::Cellwidget(const apdata::Location* clock)
    : mLocation(clock),
      mImage(128, 128, QImage::Format_RGB32),
      mDefaultBgColor("#567") {

    mLayout.setContentsMargins(10, 0, 0, 0);
    mLayout.setSpacing(0);
    setLayout(&mLayout);

    mTimeLabel.setAlignment(Qt::AlignRight);
    mTimeLabel.setStyleSheet("color: white");
    mLayout.addWidget(&mTimeLabel, qRound(mTimeVsZoneWeight * 1000));
    mLayout.addWidget(&mZoneLabel, qRound((1 - mTimeVsZoneWeight) * 1000));
}

const apdata::Location* Cellwidget::location() const {
    return mLocation;
}

void Cellwidget::setTime(const QString& time) {
    mTimeLabel.setText(time);
}

void Cellwidget::setText(const QString& text) {
    mZoneLabel.setText(text);
}

QImage Cellwidget::image() const {
    return mImage;
}

void Cellwidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter ptr(this);
    ptr.setPen(Qt::NoPen);
    ptr.setBrush(mDefaultBgColor);
    const auto rec = rect().adjusted(0, 0, -1, -1);
    constexpr int radius = 16;
    ptr.drawRoundedRect(rec, radius, radius);
    if (!mImage.isNull()) {
        qDebug() << "Painting image!";
        ptr.drawImage(rect(), mImage);
    }
}

ClockWidget::ClockWidget(const apdata::LocationList& locations)
    : mLocations(locations) {
    const int margin = 4;
    mLayout.setContentsMargins(margin, margin, margin, margin);
    mLayout.setSpacing(8);
    setLayout(&mLayout);

    for (auto& loc: locations) {
        auto block = new Cellwidget(loc.get());
        mZoneWidgets << block;
    }

    mLayout.addWidget(mZoneWidgets[0], 0, 0);
    mLayout.addWidget(mZoneWidgets[1], 0, 1);
    mLayout.addWidget(mZoneWidgets[2], 1, 0);
    mLayout.addWidget(mZoneWidgets[3], 1, 1);
    mLayout.addWidget(&mMinuteWidget, 0, 2);

    connect(&mTimer, &QTimer::timeout, this, &ClockWidget::updateTime);
    mTimer.setInterval(1000);
    mTimer.start();

    updateTime();

    QTimer::singleShot(2000, this, &ClockWidget::updateImages);
}
void ClockWidget::updateImages() {

    for (auto widget: mZoneWidgets) {
        const auto& loc = widget->location();
        auto coords = suncalc::sunCoords(QDateTime::currentDateTimeUtc(),
                                         loc->latitude, loc->longitude, 0.1);

        qDebug() << "elevation:" << coords.elevation << "azimuth:" << coords.azimuth;

        auto sunDir = sunVector(coords);

        QImage im = widget->image();
        skycolor::renderCamera(sunDir, im);
        widget->update();
        // widget.setImage(im);
        //
        // QThreadPool::start([loc] {
        // },
        //                    Qt::LowEventPriority);
    }
}

void ClockWidget::updateTime() {
    auto minuteStr = QDateTime::currentDateTimeUtc().toString(":mm");
    mMinuteWidget.setTime(minuteStr);

    for (int i = 0, len = mZoneWidgets.count(); i < len; i++) {
        const auto& def = mLocations[i];
        const auto widget = mZoneWidgets[i];
        auto dt = QDateTime::currentDateTimeUtc().toTimeZone(def->timezone);
        auto hourStr = dt.time().toString("HH");
        widget->setTime(hourStr);
        widget->setText(def->iata);
    }
}
