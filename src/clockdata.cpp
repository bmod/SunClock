#include "clockdata.h"
#include <QtDebug>


ClockData::ClockData(const QString& timeZone) {
    mTimeZone = QTimeZone(timeZone.toLatin1());
    qWarning() << mTimeZone;
}

void ClockData::setTime(const QDateTime& timeUtc) {
    if (mCurrentTime == timeUtc)
        return;
    mCurrentTime = timeUtc;
    changed();
}
const QDateTime& ClockData::currentTime() const {
    return mCurrentTime;
}

QString ClockData::timezoneName() const {
    return QString::fromLatin1(mTimeZone.id());
}

const QTimeZone& ClockData::timeZone() const {
    return mTimeZone;
}
