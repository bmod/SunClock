#pragma once

#include <QTimeZone>
#include "csv.h"

namespace airportsdata {

    using LatLong = QPair<qreal, qreal>;

    struct Airport {
        QString iata;
        QString name;
        qreal latitude;
        qreal longitude;
        QTimeZone timezone;
        QString municipality;
    };

    class AirportsData {
    public:
        AirportsData(const QString& filePath);

        const Airport* airport(const QString& iana);

    private:
        void load(const QString& filePath);

        QList<Airport> mAirports;
    };

}