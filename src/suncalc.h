// Stolen from the blender-addon "sun_position": https://github.com/blender/blender-addons
#pragma once

#include "vec3.h"

#include <qdatetime.h>

namespace suncalc {

    class SunCoords {
    public:
        qreal azimuth;
        qreal elevation;
        SunCoords(const qreal azimuth = 0, const qreal elevation = 0) : azimuth(azimuth), elevation(elevation) {}
    };

    SunCoords sunCoordinates(qreal localHoursDecimal, qreal latitude, qreal longitude, qreal utcZone,
                             int month, int day, int year, bool useRefraction = false, qreal north_offset = 0);

    SunCoords sunCoords(const QDateTime& time, qreal latitude, qreal longitude, qreal altitude);

    Vec3f sunVector(const SunCoords& c);
}