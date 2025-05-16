
#pragma once

#include <QMap>

#include "geolocation.h"


struct Airport {
    GeoLocation location = {0, 0};
    QString iata;
    QString timezone;
};

class AirportDB {
public:
    AirportDB(const QString& filePath);
    bool getAirport(const QString& iata, Airport& ap);

private:
    QMap<QString, Airport> mAirports;
};
