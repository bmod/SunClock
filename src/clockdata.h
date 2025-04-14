#pragma once

#include <QTimeZone>

class ClockData : public QObject {
    Q_OBJECT
public:
    ClockData(const QString& timeZone);
    void setTime(const QDateTime& timeUtc);
    const QDateTime& currentTime() const;
    void setCurrentTime(const QDateTime& time);
    QString timezoneName() const;
    const QTimeZone& timeZone() const;
Q_SIGNALS:
    void changed();

private:
    QDateTime mCurrentTime;
    QTimeZone mTimeZone;
};
