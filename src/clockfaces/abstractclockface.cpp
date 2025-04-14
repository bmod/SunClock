#include "abstractclockface.h"

AbstractClockFace::AbstractClockFace(const ClockData& clock): mClock(clock) {
    connect(&mClock, &ClockData::changed, this, &AbstractClockFace::onClockChanged);
}

const ClockData& AbstractClockFace::clock() const {
    return mClock;
}
void AbstractClockFace::onClockChanged() {
    setTime(mClock.currentTime());
}