#pragma once

#include "config.h"
#include "scalinglabel.h"
#include "tilewidget.h"

class ClockWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ClockWidget();

Q_SIGNALS:
    void imageLoaded(int zoneIndex, QImage im, float brightness);

private:
    void updateImages();
    void onImageLoaded(int zoneIndex, const QImage& im, float brightness);
    void updateTime();

    QList<TileWidget*> mZoneWidgets;
    QGridLayout mLayout;
    TileWidget mMinuteWidget;
    TileWidget mSecondWidget;
    QTimer mTimer;
    QTimer mImageTimer;// Images take a few seconds to render on my G15 i7 laptop
};
