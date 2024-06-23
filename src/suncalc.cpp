#include "suncalc.h"

#include "SolarAzEl.h"

#include <QtDebug>
#include <QtMath>

qreal get_julian_day(int year, int month, int day) {
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    const int A = floor(year / 100);
    const int B = 2 - A + qFloor(A / 4.0);
    const qreal jd = (qFloor((365.25 * (year + 4716.0))) + floor(30.6001 * (month + 1)) + day + B - 1524.5);
    return jd;
}

qreal julian_time_from_y2k(qreal utc_time, int year, int month, int day) {
    // Get the elapsed julian time since 1/1/2000 12:00 gmt
    // Y2k epoch (1/1/2000 12:00 gmt) is Julian day 2451545.0
    const qreal century = 36525.0;// Days in Julian Century
    const qreal epoch = 2451545.0;// Julian Day for 1/1/2000 12:00 gmt
    const qreal jd = get_julian_day(year, month, day);
    return ((jd + (utc_time / 24)) - epoch) / century;
}

qreal obliquity_of_ecliptic(qreal t) {
    return ((23.0 + 26.0 / 60 + (21.4480 - 46.8150) / 3600 * t -
             (0.00059 / 3600) * pow(t, 2) + (0.001813 / 3600) * pow(t, 3)));
}

qreal obliquity_correction(qreal t) {
    const qreal ec = obliquity_of_ecliptic(t);
    const qreal omega = 125.04 - 1934.136 * t;
    return (ec + 0.00256 * cos(qDegreesToRadians(omega)));
}

qreal mean_longitude_sun(qreal t) {
    return fmod((280.46646 + 36000.76983 * t + 0.0003032 * pow(t, 2)), 360);
}

qreal mean_anomaly_sun(qreal t) {
    return (357.52911 + t * (35999.05029 - 0.0001537 * t));
}

qreal equation_of_sun_center(qreal t) {
    const qreal m = qDegreesToRadians(mean_anomaly_sun(t));
    const qreal c = ((1.914602 - 0.004817 * t - 0.000014 * pow(t, 2)) * sin(m) +
               (0.019993 - 0.000101 * t) * sin(m * 2) +
               0.000289 * sin(m * 3));
    return c;
}

qreal true_longitude_of_sun(qreal t) {
    return (mean_longitude_sun(t) + equation_of_sun_center(t));
}


qreal apparent_longitude_of_sun(qreal t) {
    return (qDegreesToRadians(true_longitude_of_sun(t) - 0.00569 - 0.00478 * sin(qDegreesToRadians(125.04 - 1934.136 * t))));
}

qreal sun_declination(qreal e, qreal L) {
    return asin(sin(e) * sin(L));
}

qreal eccentricity_earth_orbit(qreal t) {
    return (0.016708634 - 0.000042037 * t - 0.0000001267 * pow(t, 2));
}

qreal calc_equation_of_time(qreal t) {
    const qreal epsilon = obliquity_correction(t);
    const qreal ml = qDegreesToRadians(mean_longitude_sun(t));
    const qreal e = eccentricity_earth_orbit(t);
    const qreal m = qDegreesToRadians(mean_anomaly_sun(t));
    qreal y = tan(qDegreesToRadians(epsilon) / 2.0);
    y = y * y;
    const qreal sin2ml = sin(2.0 * ml);
    const qreal cos2ml = cos(2.0 * ml);
    const qreal sin4ml = sin(4.0 * ml);
    const qreal sinm = sin(m);
    const qreal sin2m = sin(2.0 * m);
    const qreal etime = (y * sin2ml - 2.0 * e * sinm + 4.0 * e * y * sinm * cos2ml - 0.5 * pow(y, 2) * sin4ml - 1.25 * pow(e, 2) * sin2m);
    return (qRadiansToDegrees(etime) * 4);
}

suncalc::SunCoords suncalc::sunCoords(const QDateTime& time, const qreal latitude, const qreal longitude, const qreal altitude) {
    SunCoords coords;
    SolarAzEl(time.toTime_t(), latitude, longitude, altitude, &coords.azimuth, &coords.elevation);
    return coords;
}

Vec3f suncalc::sunVector(const SunCoords& c) {
    const qreal phi = -qDegreesToRadians(c.azimuth);
    const qreal theta = M_PI_2 - qDegreesToRadians(c.elevation);

    const qreal locZ = sin(phi) * sin(-theta);
    const qreal locX = sin(theta) * cos(phi);
    const qreal locY = cos(theta);

    return {static_cast<float>(locX),
            static_cast<float>(locY),
            static_cast<float>(locZ)};
}
