#include "airportsdata.h"

#include <QFile>
#include <QtDebug>

#include "csv.h"

static const QString Name = "name";
static const QString Iata = "iata_code";
static const QString Latitude = "latitude_deg";
static const QString Longitude = "longitude_deg";
static const QString Municipality = "municipality";
static const QString Timezone = "timezone";


bool apdata::loadAirports(const QString& filePath, const QStringList& iataList, LocationList& outAirports) {
    if (iataList.isEmpty()) {
        qWarning() << "No locations";
        return false;
    }


    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open path" << filePath;
        return false;
    }

    QTextStream inStream(&file);

    QStringList fields;
    // Read header
    if (!qcsv::readCsvRow(inStream, &fields)) {
        qWarning() << "Failed to read row";
        return false;
    }

    const int fieldCount = fields.length();
    QMap<QString, int> fieldIndexes;
    for (int i = 0; i < fieldCount; i++)
        fieldIndexes.insert(fields[i], i);


    QMap<QString, QStringList> rowMap;
    QStringList row;
    while (qcsv::readCsvRow(inStream, &row)) {
        const auto iata = row[fieldIndexes[Iata]].toUpper();

        if (!iataList.contains(iata))
            continue;

        rowMap.insert(iata, row);

    }

    for (const auto& iata : iataList) {
        row = rowMap[iata];
        auto ap = std::make_unique<Location>();
        ap->name = row[fieldIndexes[Name]];
        ap->iata = iata;
        ap->timezone = QTimeZone(row[fieldIndexes[Timezone]].toLatin1());
        ap->municipality = row[fieldIndexes[Municipality]];
        ap->latitude = row[fieldIndexes[Latitude]].toDouble();
        ap->longitude = row[fieldIndexes[Longitude]].toDouble();

        outAirports.emplace_back(std::move(ap));
    }
    return true;
}
