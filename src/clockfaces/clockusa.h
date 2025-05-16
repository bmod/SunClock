#pragma once

#include <QVBoxLayout>
#include <QLabel>

#include <config.h>

#include "abstractclockface.h"

class ClockUsa : public AbstractClockFace
{
public:
    ClockUsa(ClockData& clock);

    void setTime(QDateTime timeUtc) override;

private:
    QVBoxLayout mLayout;
    QLabel mTimezoneLabel;
    QLabel mTimeLabel;
};
