#include "airportdb.h"

// #include "log.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

AirportDB::AirportDB(const QString& filePath) {
    if (!QFile::exists(filePath)) {
        const char* fp = filePath.toLatin1();
        qFatal("File not found: %s", fp);
    }
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qFatal("Failed to open file for reading");
    }

    auto doc = QJsonDocument::fromJson(file.readAll());
    auto jAirports = doc.array();
    for (auto jAp: jAirports) {
        Airport ap;
        auto jAirport = jAp.toObject();

        if (jAirport.contains("iata_code"))
            ap.iata = jAirport["iata_code"].toString();

        if (jAirport.contains("timezone"))
            ap.timezone = jAirport["timezone"].toString();

        if (jAirport.contains("latitude_deg")) {
            ap.location.latitude = jAirport["latitude_deg"].toDouble();
        }

        if (jAirport.contains("longitude_deg")) {
            ap.location.longitude = jAirport["longitude_deg"].toDouble();
        }
        mAirports.insert(ap.iata, ap);
    }
}

bool AirportDB::getAirport(const QString& iata, Airport& ap) {
    const auto it = mAirports.find(iata);
    if (it == mAirports.end())
        return false;

    ap.location.latitude = it->location.latitude;
    ap.location.longitude = it->location.longitude;
    ap.iata = it->iata;
    ap.timezone = it->timezone;
    return true;
}