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
        utils::ERR_AND_EXIT("Expected '%1'", key);

    return obj[key];
}

QJsonArray jArray(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isArray())
        utils::ERR_AND_EXIT("Not an array: '%1'", key);
    return child.toArray();
}

QJsonObject jObject(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isObject())
        utils::ERR_AND_EXIT("Not an object: '%1'", key);
    return child.toObject();
}

QString j_QString(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isString())
        utils::ERR_AND_EXIT("Not a string: '%1'", key);
    return child.toString();
}

double j_double(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isDouble())
        utils::ERR_AND_EXIT("Not a number: '%1'", key);
    return child.toDouble();
}

double j_float(const QJsonObject& obj, const QString& key) {
    return j_double(obj, key);
}

int j_int(const QJsonObject& obj, const QString& key) {
    const double d = j_double(obj, key);
    return qRound(d);
}

int j_bool(const QJsonObject& obj, const QString& key) {
    const QJsonValue child = jChild(obj, key);
    if (!child.isBool())
        utils::ERR_AND_EXIT("Not a boolean: '%1'", key);
    return child.toBool();
}

QColor j_QColor(const QJsonObject& obj, const QString& key) {
    return j_QString(obj, key);
}

QString absPath(const QString& filePath) {
    return QFileInfo(filePath).absoluteFilePath();
}

Config::Config() {
    loadConfig();
}

Config& Config::get() {
    static Config instance;
    return instance;
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
    for (const auto& jLocation: jLocations) {
        locations << jLocation.toString();
    }
    if (!loadAirports(airportsPath, locations, mLocations))
        utils::ERR_AND_EXIT("Failed to load airport data");

    // Style
    read_startFullscreen(jRoot);
    read_windowWidth(jRoot);
    read_windowHeight(jRoot);

    read_backgroundColor(jRoot);
    read_clockColorTime(jRoot);
    read_clockColorTimeDark(jRoot);
    read_clockColorLocation(jRoot);
    read_clockColorMinutes(jRoot);
    read_clockColorSeconds(jRoot);
    read_darkTextThreshold(jRoot);
    read_fontName(jRoot);
    read_margin(jRoot);
    read_tileMargin(jRoot);
    read_boxSpacing(jRoot);
    read_boxCornerRadius(jRoot);
    read_cameraFov(jRoot);
    read_stretchDown(jRoot);
    read_toneMap(jRoot);
    read_subjectHeight(jRoot);
    read_noiseAmount(jRoot);

    read_clockTimerInterval(jRoot);
    read_imageTimerInterval(jRoot);
    read_skyResolutionScale(jRoot);
    read_skySamples(jRoot);
}
