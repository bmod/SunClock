#pragma once

#include <QWidget>

#include <clockdata.h>

#include "config.h"

class AbstractClockFace : public QWidget {
    Q_OBJECT

public:
    AbstractClockFace(ClockData& clock);
    ClockData& clockData() const;
    virtual void setTime(QDateTime secondsUtc) = 0;

private:
    void onClockChanged();
    ClockData& mClock;
};
