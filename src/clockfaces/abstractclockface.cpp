#include "abstractclockface.h"

AbstractClockFace::AbstractClockFace(ClockData& clock) : mClock(clock) {
    connect(&mClock, &ClockData::changed, this, &AbstractClockFace::onClockChanged);
}

ClockData& AbstractClockFace::clockData() const {
    return mClock;
}
void AbstractClockFace::onClockChanged() {
    setTime(mClock.currentTime());
}