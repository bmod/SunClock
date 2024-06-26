#pragma once

#include <QTimeZone>

#include <memory>

namespace apdata {

    using LatLong = QPair<qreal, qreal>;

    struct Location {
        QString iata;
        QString name;
        qreal latitude;
        qreal longitude;
        QTimeZone timezone;
        QString municipality;
    };

    using AirportPtr = std::unique_ptr<Location>;
    using LocationList = std::vector<AirportPtr>;

    bool loadAirports(const QString& filePath, const QStringList& iataList, LocationList& outAirports);

}// namespace airportsdata
