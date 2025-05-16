// Stolen from the blender-addon "sun_position": https://github.com/blender/blender-addons
#pragma once

// #include "SFML/System/Vector3.hpp"

#include <chrono>
#include <QVector3D>

namespace suncalc {

    class SunCoords {
    public:
        float azimuth;
        float elevation;
        SunCoords(const float azimuth = 0, const float elevation = 0) : azimuth(azimuth), elevation(elevation) {}
    };

    SunCoords sunCoords(const std::chrono::system_clock::time_point&, float latitude, float longitude, float altitude);

    QVector3D sunVector(const SunCoords& c);
}// namespace suncalc