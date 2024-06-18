#pragma once

#include <QTimeZone>

class ClockDef {
public:
    ClockDef(const QString& timeZone, QString  displayName);

    QTimeZone timeZone() const;
    QString displayName() const;

private:
    const QTimeZone mTimeZone;
    const QString mName;
};