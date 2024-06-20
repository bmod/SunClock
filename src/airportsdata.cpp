#include "airportsdata.h"

#include <QFile>
#include <QtDebug>

static const QString Name = "name";
static const QString Iata = "iata_code";
static const QString Latitude = "latitude_deg";
static const QString Longitude = "longitude_deg";
static const QString Municipality = "municipality";
static const QString Timezone = "timezone";


void apdata::loadAirports(const QString& filePath, const QStringList& iataList, LocationList& outAirports) {

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open path" << filePath;
        return;
    }

    QTextStream inStream(&file);

    QStringList fields;
    // Read header
    if (!qcsv::readCsvRow(inStream, &fields)) {
        qWarning() << "Failed to read row";
        return;
    }

    const int fieldCount = fields.length();
    QMap<QString, int> fieldIndexes;
    for (int i = 0; i < fieldCount; i++)
        fieldIndexes.insert(fields[i], i);

    QStringList row;
    while (qcsv::readCsvRow(inStream, &row)) {
        const auto iata = row[fieldIndexes[Iata]];
        if (!iataList.contains(iata))
            continue;

        auto ap = std::make_unique<Location>();
        ap->name = row[fieldIndexes[Name]];
        ap->iata = iata;
        ap->timezone = QTimeZone(row[fieldIndexes[Timezone]].toLatin1());
        ap->municipality = row[fieldIndexes[Municipality]];
        ap->latitude = row[fieldIndexes[Latitude]].toDouble();
        ap->longitude = row[fieldIndexes[Longitude]].toDouble();

        outAirports.emplace_back(std::move(ap));
    }
}
