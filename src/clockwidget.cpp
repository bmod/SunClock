#include "clockwidget.h"

#include <QColorSpace>
#include <QPainter>
#include <QPainterPath>
#include <QThreadPool>
#include <skycolor.h>
#include <suncalc.h>
#include <utils.h>

ClockWidget::ClockWidget(const Config& config)
    : mConfig(config), mMinuteWidget(config), mSecondWidget(config) {
    const int margin = config.margin();
    mLayout.setContentsMargins(margin, margin, margin, margin);
    mLayout.setSpacing(config.boxSpacing());
    setLayout(&mLayout);

    mMinuteWidget.setHourColor(config.clockColorMinutes());
    mSecondWidget.setHourColor(config.clockColorSeconds());

    for (auto& loc: config.locations()) {
        auto cell = new TileWidget(config, loc.get());
        cell->setLocationColor(config.clockColorLocation());
        cell->setHourColor(config.clockColorTime());
        mZoneWidgets << cell;
    }

    mLayout.addWidget(mZoneWidgets[0], 0, 0);
    mLayout.addWidget(mZoneWidgets[1], 0, 1);
    mLayout.addWidget(mZoneWidgets[2], 1, 0);
    mLayout.addWidget(mZoneWidgets[3], 1, 1);
    mLayout.addWidget(&mMinuteWidget, 0, 2);
    mLayout.addWidget(&mSecondWidget, 1, 2);

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

            const double skyResolutionScale = mConfig.skyResolutionScale();
            const auto destImage = widget->image();
            if (skyResolutionScale < 1)
                qFatal("Sky resolution must be > 1");

            QImage im(destImage.size() / mConfig.skyResolutionScale(), destImage.format());
            skycolor::renderCamera(sunDir, im, false);
            im = im.scaled(destImage.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            const auto& mini = im.scaled(QSize(3, 3), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            const float brightness = utils::averageBrightness(mini);
            drawCorners(im, mConfig.boxCornerRadius(), mConfig.backgroundColor());
            imageLoaded(index, im, brightness);
        },
                                             Qt::LowEventPriority);
        ++index;
    }
}

void ClockWidget::onImageLoaded(const int zoneIndex, const QImage& im, float brightness) {
    const auto widget = mZoneWidgets[zoneIndex];
    const auto textColor = (brightness > mConfig.darkTextThreshold()) ? mConfig.clockColorTimeDark() : mConfig.clockColorTime();
    widget->setHourColor(textColor);
    widget->setImage(im.scaled(widget->contentsRect().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void ClockWidget::updateTime() {
    const auto minuteStr = QDateTime::currentDateTimeUtc().toString(":mm");
    mMinuteWidget.setHours(minuteStr);

    const auto secondStr = QDateTime::currentDateTimeUtc().toString(":ss");
    mSecondWidget.setHours(secondStr);

    for (int i = 0, len = mZoneWidgets.count(); i < len; i++) {
        const auto& def = mConfig.locations()[i];
        const auto widget = mZoneWidgets[i];
        auto dt = QDateTime::currentDateTimeUtc().toTimeZone(def->timezone);
        auto hourStr = dt.time().toString("HH");
        widget->setHours(hourStr);
        widget->setText(def->iata);
    }
}
