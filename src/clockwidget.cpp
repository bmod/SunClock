#include "clockwidget.h"
#include <QPainter>
#include <QPainterPath>
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

    mZoneLabel.setStyleSheet("color: #666");

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
    ptr.setRenderHint(QPainter::SmoothPixmapTransform);
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
    mLayout.addWidget(&mSecondWidget, 1, 2);

    connect(&mTimer, &QTimer::timeout, this, &ClockWidget::updateTime);
    mTimer.setInterval(1000);
    mTimer.start();

    updateTime();

    connect(&mImageTimer, &QTimer::timeout, this, &ClockWidget::updateImages);
    mImageTimer.setInterval(10 * 1000);
    mImageTimer.start();

    connect(this, &ClockWidget::imageLoaded, this, &ClockWidget::onImageLoaded, Qt::QueuedConnection);

    updateImages();
}
void ClockWidget::updateImages() {

    int index = 0;
    for (auto widget: mZoneWidgets) {

        QThreadPool::globalInstance()->start([this, index, widget] {
            const auto loc = widget->location();
            const auto coords = suncalc::sunCoords(QDateTime::currentDateTimeUtc(),
                                                   loc->latitude, loc->longitude, 10);

            const auto sunDir = sunVector(coords);

            auto image = widget->image();
            skycolor::renderCamera(sunDir, image);
            image = image.scaled(512, 512);
            {
                // TODO: Correct corner aspect
                int r = 32;
                int r2 = r * 2;
                int w = image.width();
                int h = image.height();
                QPainterPath path;
                path.moveTo(0, 0);
                path.lineTo(0, r);
                path.arcTo(0, 0, r2, r2, 180, -90);
                path.lineTo(0, 0);

                path.moveTo(w, 0);
                path.lineTo(w, r);
                path.arcTo(w - r2, 0, r2, r2, 0, 90);
                path.lineTo(w, 0);

                path.moveTo(w, h);
                path.lineTo(w - r, h);
                path.arcTo(w - r2, h - r2, r2, r2, -90, 90);
                path.lineTo(w, h);

                path.moveTo(0, h);
                path.lineTo(r, h);
                path.arcTo(0, h - r2, r2, r2, -90, -90);
                path.lineTo(0, h);

                QPainter ptr(&image);
                ptr.setRenderHint(QPainter::Antialiasing, true);
                ptr.fillPath(path, Qt::black);
            }

            imageLoaded(index, image);
        },
                                             Qt::LowEventPriority);
        ++index;
    }
}


void ClockWidget::onImageLoaded(int zoneIndex, QImage im) {
    auto widget = mZoneWidgets[zoneIndex];
    widget->setImage(im.scaled(512, 512, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void ClockWidget::updateTime() {
    auto minuteStr = QDateTime::currentDateTimeUtc().toString(":mm");
    mMinuteWidget.setTime(minuteStr);
    auto secondStr = QDateTime::currentDateTimeUtc().toString(":ss");
    mSecondWidget.setTime(secondStr);

    for (int i = 0, len = mZoneWidgets.count(); i < len; i++) {
        const auto& def = mLocations[i];
        const auto widget = mZoneWidgets[i];
        auto dt = QDateTime::currentDateTimeUtc().toTimeZone(def->timezone);
        auto hourStr = dt.time().toString("HH");
        widget->setTime(hourStr);
        widget->setText(def->iata);
    }
}
