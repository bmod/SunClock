#include "airportsdata.h"

#include <QtDebug>
#include <QFile>

static const QString Name = "name";
static const QString Iata = "iata_code";
static const QString Latitude = "latitude_deg";
static const QString Longitude = "longitude_deg";
static const QString Municipality = "municipality";
static const QString Timezone = "timezone";

airportsdata::AirportsData::AirportsData(const QString& filePath) {
    load(filePath);
}

void airportsdata::AirportsData::load(const QString& filePath) {
    mAirports.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open path" << filePath;
        return;
    }

    QTextStream inStream(&file);

    QStringList fields;
    try {
        // Read header
        if (!qcsv::readCsvRow(inStream, &fields)) {
            qWarning() << "Failed to read row";
            return;
        }

        int fieldCount = fields.length();
        QMap<QString, int> fieldIndexes;
        for (int i = 0; i < fieldCount; i++)
            fieldIndexes.insert(fields[i], i);

        QStringList row;
        while (qcsv::readCsvRow(inStream, &row)) {
            Airport ap;
            ap.name = row[fieldIndexes[Name]];
            ap.iata = row[fieldIndexes[Iata]];
            ap.timezone = QTimeZone(row[fieldIndexes[Timezone]].toLatin1());
            ap.municipality = row[fieldIndexes[Municipality]];
            ap.latitude = row[fieldIndexes[Latitude]].toDouble();
            ap.longitude = row[fieldIndexes[Longitude]].toDouble();

            mAirports << ap;
        }
    } catch (const std::runtime_error& err) {
        qWarning() << "Failed to load CSV" << filePath << ", error: " << err.what();
    }
}

const airportsdata::Airport* airportsdata::AirportsData::airport(const QString& iana) {
    const auto& ianaUpper = iana.toUpper();
    for (const auto& ap: mAirports) {
        if (ap.iata == ianaUpper)
            return &ap;
    }
    return nullptr;
}

