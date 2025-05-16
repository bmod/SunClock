#include "clockusa.h"

#include <QFile>

ClockUsa::ClockUsa(ClockData& clock): AbstractClockFace(clock) {
    QFile file(":styleClockUsa.css");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qFatal("Could not open style file");
    }
    setStyleSheet(file.readAll());

    setLayout(&mLayout);
    mTimeLabel.setObjectName("time");
    mLayout.addWidget(&mTimeLabel);
    mTimezoneLabel.setObjectName("timeZone");
    mLayout.addWidget(&mTimezoneLabel);
    mTimezoneLabel.setText(clock.timezoneName());
}

void ClockUsa::setTime(QDateTime timeUtc) {
    const auto dtLocal = timeUtc.toTimeZone(clockData().timeZone());
    mTimeLabel.setText(dtLocal.toString("hh:mm:ss ap"));
}