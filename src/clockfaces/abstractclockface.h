#pragma once

#include <QWidget>

#include <clockdata.h>

#include "config.h"

class AbstractClockFace : public QWidget {
    Q_OBJECT

public:
    AbstractClockFace(const ClockData& clock);
    const ClockData& clock() const;
    virtual void setTime(QDateTime secondsUtc) = 0;

private:
    void onClockChanged();
    const ClockData& mClock;
};
