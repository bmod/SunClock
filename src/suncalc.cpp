#include "suncalc.h"

#include "SolarAzEl.h"

#include <chrono>
#include <cmath>

float degreesToRadians(float d) {
    return d * (M_PI / 180.f);
}

float radiansToDegrees(float r) {
    return r * (180.0 / M_PI);
}

float get_julian_day(int year, int month, int day) {
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    const int A = floor(year / 100);
    const int B = 2 - A + floor(A / 4.0);
    const float jd = (floor((365.25 * (year + 4716.0))) + floor(30.6001 * (month + 1)) + day + B - 1524.5);
    return jd;
}

float julian_time_from_y2k(float utc_time, int year, int month, int day) {
    // Get the elapsed julian time since 1/1/2000 12:00 gmt
    // Y2k epoch (1/1/2000 12:00 gmt) is Julian day 2451545.0
    const float century = 36525.0;// Days in Julian Century
    const float epoch = 2451545.0;// Julian Day for 1/1/2000 12:00 gmt
    const float jd = get_julian_day(year, month, day);
    return ((jd + (utc_time / 24)) - epoch) / century;
}

float obliquity_of_ecliptic(float t) {
    return ((23.0 + 26.0 / 60 + (21.4480 - 46.8150) / 3600 * t - (0.00059 / 3600) * pow(t, 2) +
             (0.001813 / 3600) * pow(t, 3)));
}

float obliquity_correction(float t) {
    const float ec = obliquity_of_ecliptic(t);
    const float omega = 125.04 - 1934.136 * t;
    return (ec + 0.00256 * cos(degreesToRadians(omega)));
}

float mean_longitude_sun(float t) {
    return fmod((280.46646 + 36000.76983 * t + 0.0003032 * pow(t, 2)), 360);
}

float mean_anomaly_sun(float t) {
    return (357.52911 + t * (35999.05029 - 0.0001537 * t));
}

float equation_of_sun_center(float t) {
    const float m = degreesToRadians(mean_anomaly_sun(t));
    const float c = ((1.914602 - 0.004817 * t - 0.000014 * pow(t, 2)) * sin(m) +
                     (0.019993 - 0.000101 * t) * sin(m * 2) + 0.000289 * sin(m * 3));
    return c;
}

static float true_longitude_of_sun(float t) {
    return (mean_longitude_sun(t) + equation_of_sun_center(t));
}


static float apparent_longitude_of_sun(float t) {
    return (degreesToRadians(true_longitude_of_sun(t) - 0.00569 -
                             0.00478 * sin(degreesToRadians(125.04 - 1934.136 * t))));
}

static float sun_declination(float e, float L) {
    return asin(sin(e) * sin(L));
}

static float eccentricity_earth_orbit(float t) {
    return (0.016708634 - 0.000042037 * t - 0.0000001267 * pow(t, 2));
}

float calc_equation_of_time(float t) {
    const float epsilon = obliquity_correction(t);
    const float ml = degreesToRadians(mean_longitude_sun(t));
    const float e = eccentricity_earth_orbit(t);
    const float m = degreesToRadians(mean_anomaly_sun(t));
    float y = tan(degreesToRadians(epsilon) / 2.0);
    y = y * y;
    const float sin2ml = sin(2.0 * ml);
    const float cos2ml = cos(2.0 * ml);
    const float sin4ml = sin(4.0 * ml);
    const float sinm = sin(m);
    const float sin2m = sin(2.0 * m);
    const float etime = (y * sin2ml - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2ml - 0.5 * pow(y, 2) * sin4ml -
                         1.25 * pow(e, 2) * sin2m);
    return (radiansToDegrees(etime) * 4);
}

suncalc::SunCoords suncalc::sunCoords(const std::chrono::system_clock::time_point& time, const float latitude, const float longitude,
                                      const float altitude) {
    double azimuth;
    double elevation;
    SolarAzEl::SolarAzEl(std::chrono::system_clock::to_time_t(time), latitude, longitude, altitude, &azimuth, &elevation);
    return {static_cast<float>(azimuth), static_cast<float>(elevation)};
}

QVector3D suncalc::sunVector(const SunCoords& c) {
    const float phi = -degreesToRadians(c.azimuth);
    const float theta = M_PI_2 - degreesToRadians(c.elevation);

    const float locZ = sin(phi) * sin(-theta);
    const float locX = sin(theta) * cos(phi);
    const float locY = cos(theta);

    return {static_cast<float>(locX), static_cast<float>(locY), static_cast<float>(locZ)};
}
