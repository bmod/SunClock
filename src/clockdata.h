#pragma once

#include <QTimeZone>

#include "geolocation.h"

class ClockData : public QObject {
    Q_OBJECT
public:
    ClockData(const QString& timeZone, const GeoLocation& location);
    void setTime(const QDateTime& timeUtc);
    const QDateTime& currentTime() const;
    void setCurrentTime(const QDateTime& time);
    QString timezoneName() const;
    const QTimeZone& timeZone() const;
    const GeoLocation& location() const;
Q_SIGNALS:
    void changed();

private:
    GeoLocation mLocation;
    QDateTime mCurrentTime;
    QTimeZone mTimeZone;
};
