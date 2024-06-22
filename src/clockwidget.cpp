#include "clockwidget.h"

#include <QColorSpace>
#include <QPainter>
#include <QPainterPath>
#include <QThreadPool>
#include <QtDebug>
#include <skycolor.h>
#include <suncalc.h>

Cellwidget::Cellwidget(const Config& conf, const apdata::Location* clock)
    : mConfig(conf),
      mLocation(clock) {
    // mImage.setColorSpace(QColorSpace::NamedColorSpace::SRgbLinear);
    mImage.setColorSpace(QColorSpace::NamedColorSpace::SRgb);
    clearImage();

    mLayout.setContentsMargins(10, 0, 0, 0);
    mLayout.setSpacing(0);
    setLayout(&mLayout);


    mTimeLabel.setAlignment(Qt::AlignRight);

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

QImage Cellwidget::image() {
    if (mImage.isNull()) {
        const QImage im(contentsRect().size(), QImage::Format_RGB32);
        mImage = im;
    }
    return mImage;
}

void Cellwidget::setImage(const QImage& im) {
    if (mImage == im)
        return;
    mImage = im;
    update();
}

void Cellwidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter ptr(this);
    ptr.setRenderHint(QPainter::SmoothPixmapTransform);
    ptr.setPen(Qt::NoPen);
    ptr.setBrush(mConfig.backgroundColor());
    const auto rec = rect().adjusted(0, 0, -1, -1);
    constexpr int radius = 16;
    ptr.drawRoundedRect(rec, radius, radius);
    if (!mImage.isNull()) {
        ptr.drawImage(contentsRect(), mImage);
    }
}

void Cellwidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    clearImage();
}

void Cellwidget::clearImage() {
    mImage = QImage(QSize(), QImage::Format_RGB32);
}

ClockWidget::ClockWidget(const Config& config)
    : mConfig(config), mMinuteWidget(config), mSecondWidget(config) {
    const int margin = config.margin();
    mLayout.setContentsMargins(margin, margin, margin, margin);
    mLayout.setSpacing(config.boxSpacing());
    setLayout(&mLayout);

    for (auto& loc: config.locations()) {
        auto block = new Cellwidget(config, loc.get());
        block->setStyleSheet("color: white");
        mZoneWidgets << block;
    }

    mLayout.addWidget(mZoneWidgets[0], 0, 0);
    mLayout.addWidget(mZoneWidgets[1], 0, 1);
    mLayout.addWidget(mZoneWidgets[2], 1, 0);
    mLayout.addWidget(mZoneWidgets[3], 1, 1);
    mLayout.addWidget(&mMinuteWidget, 0, 2);
    mLayout.addWidget(&mSecondWidget, 1, 2);

    mMinuteWidget.setStyleSheet("color: #666");
    mSecondWidget.setStyleSheet("color: #444");

    connect(&mTimer, &QTimer::timeout, this, &ClockWidget::updateTime);
    mTimer.setInterval(qRound(config.clockTimerInterval() * 1000));
    mTimer.start();

    updateTime();

    connect(&mImageTimer, &QTimer::timeout, this, &ClockWidget::updateImages);
    mImageTimer.setInterval(qRound(config.imageTimerInterval() * 100));
    mImageTimer.start();

    connect(this, &ClockWidget::imageLoaded, this, &ClockWidget::onImageLoaded, Qt::QueuedConnection);

    updateImages();
}

void drawCorners(QImage& image, int r, const QColor& color) {
    const int r2 = r * 2;
    const int w = image.width();
    const int h = image.height();
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
    ptr.fillPath(path, color);
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
            skycolor::renderCamera(sunDir, image, false);
            drawCorners(image, mConfig.boxCornerRadius(), mConfig.backgroundColor());
            imageLoaded(index, image);
        },
                                             Qt::LowEventPriority);
        ++index;
    }
}

void ClockWidget::onImageLoaded(const int zoneIndex, const QImage& im) {
    const auto widget = mZoneWidgets[zoneIndex];
    // scale image to final size, we rendered at lower resolution
    widget->setImage(im.scaled(widget->contentsRect().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void ClockWidget::updateTime() {
    const auto minuteStr = QDateTime::currentDateTimeUtc().toString(":mm");
    mMinuteWidget.setTime(minuteStr);

    const auto secondStr = QDateTime::currentDateTimeUtc().toString(":ss");
    mSecondWidget.setTime(secondStr);

    for (int i = 0, len = mZoneWidgets.count(); i < len; i++) {
        const auto& def = mConfig.locations()[i];
        const auto widget = mZoneWidgets[i];
        auto dt = QDateTime::currentDateTimeUtc().toTimeZone(def->timezone);
        auto hourStr = dt.time().toString("HH");
        widget->setTime(hourStr);
        widget->setText(def->iata);
    }
}
