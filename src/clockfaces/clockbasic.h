#pragma once

#include <QVBoxLayout>
#include <QGraphicsTextItem>

#include <config.h>

#include "abstractclockface.h"
#include "graphicsview.h"

class ClockBasic : public AbstractClockFace
{
public:
    ClockBasic(const ClockData& clock);

    void setTime(QDateTime timeUtc) override;

private:
    QVBoxLayout mLayout;
    QGraphicsScene mScene;
    GraphicsView mGraphicsView;
    QGraphicsTextItem mHoursTextItem;
    QGraphicsTextItem mColonTextItem;
    QGraphicsTextItem mMinutesTextItem;
    QGraphicsTextItem mDateTextItem;
    QGraphicsTextItem mTimeZoneTextItem;
};
