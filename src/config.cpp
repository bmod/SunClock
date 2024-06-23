#include "config.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include "utils.h"

constexpr auto configFilePath = "config.json";
constexpr auto airportsPath = ":/airports.csv";// from resources.qrc

QJsonValue jChild(const QJsonObject& obj, const QString& key) {
    if (!obj.contains(key))
        qFatal("Expected '%s'", key.toStdString().c_str());

    return obj[key];
}

QJsonArray jArray(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isArray())
        qFatal("Not an array: '%s'", key.toStdString().c_str());
    return child.toArray();
}

QJsonObject jObject(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isObject())
        qFatal("Not an object: '%s'", key.toStdString().c_str());
    return child.toObject();
}
QString j_QString(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isString())
        qFatal("Not a string: '%s'", key.toStdString().c_str());
    return child.toString();
}

double j_double(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isDouble())
        qFatal("Not a number: '%s'", key.toStdString().c_str());
    return child.toDouble();
}

int j_int(const QJsonObject& obj, const QString& key) {
    const double d = j_double(obj, key);
    return qRound(d);
}

QColor j_QColor(const QJsonObject& obj, const QString& key) {
    return j_QString(obj, key);
}

QString absPath(QString filePath) {
    return QFileInfo(filePath).absoluteFilePath();
}

Config::Config() {
    loadConfig();
}

const apdata::LocationList& Config::locations() const {
    return mLocations;
}

void Config::loadConfig() {
    if (!QFile::exists(configFilePath))
        utils::ERR_AND_EXIT("File not found: %1", absPath(configFilePath));

    QFile configFile(configFilePath);

    if (!configFile.open(QIODevice::ReadOnly))
        utils::ERR_AND_EXIT("Failed to open %1", absPath(configFilePath));

    const auto data = configFile.readAll();

    QJsonParseError err;
    const auto doc = QJsonDocument::fromJson(data, &err);
    if (err.error)
        utils::ERR_AND_EXIT("Failed to parse Json %1: %2", absPath(configFilePath), err.errorString());

    const auto jRoot = doc.object();

    // Locations
    const auto jGeneral = jObject(jRoot, "general");
    auto jLocations = jArray(jGeneral, "locations");
    QStringList locations;
    for (const auto& jLocation: jLocations)
        locations << jLocation.toString();
    if (!loadAirports(airportsPath, locations, mLocations))
        utils::ERR_AND_EXIT("Failed to load airport data");

    // Style
    read_backgroundColor(jRoot);
    read_clockColorTime(jRoot);
    read_clockColorTimeDark(jRoot);
    read_clockColorLocation(jRoot);
    read_clockColorMinutes(jRoot);
    read_clockColorSeconds(jRoot);
    read_darkTextThreshold(jRoot);
    read_fontName(jRoot);
    read_margin(jRoot);
    read_boxSpacing(jRoot);
    read_boxCornerRadius(jRoot);
    read_clockTimerInterval(jRoot);
    read_imageTimerInterval(jRoot);
    read_skyResolutionScale(jRoot);
    read_skySamples(jRoot);
}
