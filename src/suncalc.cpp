#include "suncalc.h"


qreal get_julian_day(int year, int month, int day) {
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    int A = floor(year / 100);
    int B = 2 - A + qFloor(A / 4.0);
    qreal jd = (qFloor((365.25 * (year + 4716.0))) + floor(30.6001 * (month + 1)) + day + B - 1524.5);
    return jd;
}

qreal julian_time_from_y2k(qreal utc_time, int year, int month, int day) {
    // Get the elapsed julian time since 1/1/2000 12:00 gmt
    // Y2k epoch (1/1/2000 12:00 gmt) is Julian day 2451545.0
    qreal century = 36525.0;  // Days in Julian Century
    qreal epoch = 2451545.0; // Julian Day for 1/1/2000 12:00 gmt
    qreal jd = get_julian_day(year, month, day);
    return ((jd + (utc_time / 24)) - epoch) / century;
}

qreal obliquity_of_ecliptic(qreal t) {
    return ((23.0 + 26.0 / 60 + (21.4480 - 46.8150) / 3600 * t -
             (0.00059 / 3600) * pow(t, 2) + (0.001813 / 3600) * pow(t, 3)));
}

qreal obliquity_correction(qreal t) {
    qreal ec = obliquity_of_ecliptic(t);
    qreal omega = 125.04 - 1934.136 * t;
    return (ec + 0.00256 * cos(qDegreesToRadians(omega)));
}

qreal mean_longitude_sun(qreal t) {
    return fmod((280.46646 + 36000.76983 * t + 0.0003032 * pow(t, 2)), 360);
}

qreal mean_anomaly_sun(qreal t) {
    return (357.52911 + t * (35999.05029 - 0.0001537 * t));
}

qreal equation_of_sun_center(qreal t) {
    qreal m = qDegreesToRadians(mean_anomaly_sun(t));
    qreal c = ((1.914602 - 0.004817 * t - 0.000014 * pow(t, 2)) * sin(m) +
               (0.019993 - 0.000101 * t) * sin(m * 2) +
               0.000289 * sin(m * 3));
    return c;
}

qreal true_longitude_of_sun(qreal t) {
    return (mean_longitude_sun(t) + equation_of_sun_center(t));
}


qreal apparent_longitude_of_sun(qreal t) {
    return (qDegreesToRadians(true_longitude_of_sun(t)
                              - 0.00569 - 0.00478 * sin(qDegreesToRadians(125.04 - 1934.136 * t))));
}

qreal sun_declination(qreal e, qreal L) {
    return asin(sin(e) * sin(L));
}

qreal eccentricity_earth_orbit(qreal t) {
    return (0.016708634 - 0.000042037 * t - 0.0000001267 * pow(t, 2));
}

qreal calc_equation_of_time(qreal t) {
    qreal epsilon = obliquity_correction(t);
    qreal ml = qDegreesToRadians(mean_longitude_sun(t));
    qreal e = eccentricity_earth_orbit(t);
    qreal m = qDegreesToRadians(mean_anomaly_sun(t));
    qreal y = tan(qDegreesToRadians(epsilon) / 2.0);
    y = y * y;
    qreal sin2ml = sin(2.0 * ml);
    qreal cos2ml = cos(2.0 * ml);
    qreal sin4ml = sin(4.0 * ml);
    qreal sinm = sin(m);
    qreal sin2m = sin(2.0 * m);
    qreal etime = (y * sin2ml - 2.0 * e * sinm + 4.0 * e * y *
                                                 sinm * cos2ml - 0.5 * pow(y, 2) * sin4ml - 1.25 * pow(e, 2) * sin2m);
    return (qRadiansToDegrees(etime) * 4);
}

suncalc::SunCoords suncalc::sunCoordinates(qreal localHoursDecimal, qreal latitude, qreal longitude,
                                           qreal utcZone, int month, int day, int year,
                                           bool useRefraction, qreal north_offset) {
    longitude *= -1; // for internal calculations
    qreal utc_time = localHoursDecimal + utcZone; // Set Greenwich Meridian Time

    qreal deg90 = 89.93;
    if (latitude > deg90) {
        latitude = qDegreesToRadians(deg90);
    } else if (latitude < -deg90) {
        latitude = qDegreesToRadians(-deg90);
    } else {
        latitude = qDegreesToRadians(latitude);
    }

    qreal t = julian_time_from_y2k(utc_time, year, month, day);

    qreal e = qDegreesToRadians(obliquity_correction(t));
    qreal L = apparent_longitude_of_sun(t);
    qreal solar_dec = sun_declination(e, L);
    qreal eqtime = calc_equation_of_time(t);

    qreal time_correction = (eqtime - 4 * longitude) + 60 * utcZone;
    qreal true_solar_time = fmod(((utc_time - utcZone) * 60.0 + time_correction), 1440);

    qreal hour_angle = true_solar_time / 4.0 - 180.0;
    if (hour_angle < -180)
        hour_angle += 360;

    qreal csz = (sin(latitude) * sin(solar_dec) +
                 cos(latitude) * cos(solar_dec) *
                 cos(qDegreesToRadians(hour_angle)));

    if (csz > 1) {
        csz = 1.0;
    } else if (csz < -1.0) {
        csz = -1.0;
    }

    qreal zenith = acos(csz);

    qreal az_denom = cos(latitude) * sin(zenith);

    qreal elevation;
    qreal azimuth;
    if (abs(az_denom) > 0.001) {
        qreal az_rad = ((sin(latitude) *
                         cos(zenith)) - sin(solar_dec)) / az_denom;
        if (abs(az_rad) > 1)
            az_rad = az_rad < 0 ? -1 : 1;
        azimuth = M_PI - acos(az_rad);
        if (hour_angle > 0)
            azimuth = -azimuth;
    } else {
        azimuth = latitude > 0 ? M_PI : 0;
    }

    if (azimuth < 0)
        azimuth += M_2_PI;

    qreal exoatm_elevation = 90 - qRadiansToDegrees(zenith);

    if (useRefraction) {
        qreal refraction_correction;
        if (exoatm_elevation <= 85) {
            refraction_correction = 0;
        } else {
            qreal te = tan(qDegreesToRadians(exoatm_elevation));
            if (exoatm_elevation > 5) {
                refraction_correction = (85.1f / te - 0.07f / pow(te, 3.f) + 0.000086f / pow(te, 5.f));
            } else if (exoatm_elevation > -0.575) {
                qreal s1 = -12.79f + exoatm_elevation * 0.711f;
                qreal s2 = 103.4f + exoatm_elevation * s1;
                qreal s3 = -518.2f + exoatm_elevation * s2;
                refraction_correction = 1735 + exoatm_elevation * s3;
            } else {
                refraction_correction = -20.774f / te;
            }
        }
        refraction_correction /= 3600;
        elevation = M_PI_2 - (zenith - qDegreesToRadians(refraction_correction));
    } else {
        elevation = M_PI_2 - zenith;
    }

    azimuth += north_offset;

    return {azimuth, elevation};
}

Vec3f suncalc::sunVector(const suncalc::SunCoords& c) {
    qreal phi = -c.azimuth;
    qreal theta = M_PI_2 - c.elevation;

    // y -> z
    // z -> x
    // x -> y

//    qreal loc_x = sin(phi) * sin(-theta);
//    qreal loc_y = sin(theta) * cos(phi);
//    qreal loc_z = cos(theta);

    qreal loc_x = sin(phi) * sin(-theta);
    qreal loc_y = sin(theta) * cos(phi);
    qreal loc_z = cos(theta);

    return {(float) loc_x, (float) loc_y, (float) loc_z};
}
