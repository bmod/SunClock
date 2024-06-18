#include "clockdef.h"

#include <utility>


ClockDef::ClockDef(const QString& timeZone, QString displayName)
        : mTimeZone(timeZone.toLatin1()), mName(std::move(displayName)) {

}

QTimeZone ClockDef::timeZone() const {
    return mTimeZone;
}

QString ClockDef::displayName() const {
    return mName;
}
