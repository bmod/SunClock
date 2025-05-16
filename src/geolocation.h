#pragma once

struct GeoLocation {
    GeoLocation(const float lat, const float lon) : latitude(lat), longitude(lon) {}
    float latitude;
    float longitude;
};
