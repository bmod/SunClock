#pragma once

#include <QVBoxLayout>
#include <QGraphicsTextItem>
#include <QtOpenGL/QGLWidget>

#include <config.h>

#include "abstractclockface.h"
#include "graphicsview.h"

class ClockBasic : public AbstractClockFace
{
public:
    ClockBasic(ClockData& clock);

    ClockData& clockData() const { return mClockData; }
    void setTime(QDateTime timeUtc) override;
    QRectF itemsBounds() const;

private:
    ClockData& mClockData;
    QVBoxLayout mLayout;
    QGraphicsScene mScene;
    GraphicsView mGraphicsView;
    QGraphicsTextItem mHoursTextItem;
    QGraphicsTextItem mColonTextItem;
    QGraphicsTextItem mMinutesTextItem;
    QGraphicsTextItem mDateTextItem;
    QGraphicsTextItem mTimeZoneTextItem;
    QList<QGraphicsItem*> mItems;
    QGraphicsPixmapItem mBackgroundItem;
};
