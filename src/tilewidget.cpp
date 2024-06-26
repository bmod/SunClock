#include "tilewidget.h"


#include <QColorSpace>
#include <QPainter>
#include <QPainterPath>

TileWidget::TileWidget(const apdata::Location* clock) : mLocation(clock) {
    mImage.setColorSpace(QColorSpace::NamedColorSpace::SRgb);
    clearImage();

    const int m = Config::get().tileMargin();
    mLayout.setContentsMargins(m, m, m, m);
    mLayout.setSpacing(0);
    setLayout(&mLayout);

    mHoursLabel.setAlignment(Qt::AlignRight);

    mLayout.addWidget(&mHoursLabel, qRound(mTimeVsZoneWeight * 1000));
    mLayout.addWidget(&mLocationLabel, qRound((1 - mTimeVsZoneWeight) * 1000));
}

const apdata::Location* TileWidget::location() const { return mLocation; }

void TileWidget::setHours(const QString& h) { mHoursLabel.setText(h); }

void TileWidget::setHourColor(const QColor& col) { setForegroundColor(mHoursLabel, col); }

void TileWidget::setText(const QString& text) { mLocationLabel.setText(text); }

void TileWidget::setLocationColor(const QColor& col) { setForegroundColor(mLocationLabel, col); }

QImage TileWidget::image() {
    if (mImage.isNull()) {
        const QImage im(contentsRect().size(), QImage::Format_RGB32);
        mImage = im;
    }
    return mImage;
}

void TileWidget::setImage(const QImage& im) {
    if (mImage == im) return;
    mImage = im;
    update();
}

ScalingLabel& TileWidget::hoursLabel() { return mHoursLabel; }

ScalingLabel& TileWidget::locationLabel() { return mLocationLabel; }

void TileWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter ptr(this);
    ptr.setRenderHint(QPainter::SmoothPixmapTransform);
    ptr.setPen(Qt::NoPen);
    ptr.setBrush(Config::get().backgroundColor());
    const auto rec = rect().adjusted(0, 0, -1, -1);
    const int radius = Config::get().boxCornerRadius();
    ptr.drawRoundedRect(rec, radius, radius);
    if (!mImage.isNull()) { ptr.drawImage(contentsRect(), mImage); }
}

void TileWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    clearImage();
}

void TileWidget::clearImage() { mImage = QImage(QSize(), QImage::Format_RGB32); }

void TileWidget::setForegroundColor(QWidget& widget, const QColor& color) {
    widget.setStyleSheet("color: " + color.name());
}
