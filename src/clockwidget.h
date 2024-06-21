#pragma once

#include <QTimer>
#include <QVBoxLayout>

#include "scalinglabel.h"

#include <airportsdata.h>


class Cellwidget final : public QWidget {
    Q_OBJECT
public:
    explicit Cellwidget(const apdata::Location* clock = nullptr);
    [[nodiscard]] const apdata::Location* location() const;
    void setTime(const QString& time);
    void setText(const QString& text);
    [[nodiscard]] QImage image() const;
    void setImage(QImage im) { mImage = im; update(); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    const apdata::Location* mLocation = nullptr;

    QVBoxLayout mLayout;
    QImage mImage;
    QColor mDefaultBgColor;
    ScalingLabel mTimeLabel;
    ScalingLabel mZoneLabel;
    qreal mTimeVsZoneWeight = 0.8;
};

class ClockWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ClockWidget(const apdata::LocationList& locations);

Q_SIGNALS:
    void imageLoaded(int zoneIndex, QImage im);

private:
    void updateImages();
    void onImageLoaded(int zoneIndex, QImage im);
    void updateTime();

    const apdata::LocationList& mLocations;
    QList<Cellwidget*> mZoneWidgets;
    QGridLayout mLayout;
    Cellwidget mMinuteWidget;
    Cellwidget mSecondWidget;
    QTimer mTimer;
    QTimer mImageTimer; // Images take a few seconds to render on my G15 i7 laptop
};
