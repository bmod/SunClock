#pragma once

#include <QTimer>
#include <QVBoxLayout>

#include "scalinglabel.h"

#include <airportsdata.h>
#include <config.h>


class Cellwidget final : public QWidget {
    Q_OBJECT
public:
    explicit Cellwidget(const Config& conf, const apdata::Location* clock = nullptr);
    [[nodiscard]] const apdata::Location* location() const;
    void setTime(const QString& time);
    void setText(const QString& text);
    [[nodiscard]] QImage image();
    void setImage(const QImage& im);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void clearImage();
    const apdata::Location* mLocation = nullptr;

    const Config& mConfig;
    QVBoxLayout mLayout;
    QImage mImage;
    ScalingLabel mTimeLabel;
    ScalingLabel mZoneLabel;
    qreal mTimeVsZoneWeight = 0.8;
};

class ClockWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ClockWidget(const Config& config);

Q_SIGNALS:
    void imageLoaded(int zoneIndex, QImage im);

private:
    void updateImages();
    void onImageLoaded(int zoneIndex, const QImage& im);
    void updateTime();

    const Config& mConfig;
    QList<Cellwidget*> mZoneWidgets;
    QGridLayout mLayout;
    Cellwidget mMinuteWidget;
    Cellwidget mSecondWidget;
    QTimer mTimer;
    QTimer mImageTimer;// Images take a few seconds to render on my G15 i7 laptop
};
