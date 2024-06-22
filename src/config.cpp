#include "config.h"

#include <QApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QtDebug>

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
    double d = j_double(obj, key);
    return qRound(d);
}

const char* absPath(QString filePath) {
    return QFileInfo(configFilePath).absoluteFilePath().toStdString().c_str();
}

Config::Config() {
    loadConfig();
}

void Config::loadConfig() {
    if (!QFile::exists(configFilePath)) {
        qFatal("File not found: %s", absPath(configFilePath));
    }

    QFile configFile(configFilePath);

    if (!configFile.open(QIODevice::ReadOnly)) {
        qFatal("Failed to open %s", absPath(configFilePath));
    }

    auto data = configFile.readAll();

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(data, &err);

    auto jRoot = doc.object();

    // Locations
    auto jLocations = jArray(jRoot, "locations");
    QStringList locations;
    for (const auto& jLocation: jLocations)
        locations << jLocation.toString();
    if (!loadAirports(airportsPath, locations, mLocations))
        qFatal("Failed to load airport data");

    // Style
    const auto jStyle = jRoot["style"].toObject();
    mBackgroundColor = QColor(jStyle["backgroundColor"].toString());
    mFontName = j_QString(jStyle, "font");
    mMargin = j_int(jStyle, "margin");
    mBoxSpacing = j_int(jStyle, "boxSpacing");
    mBoxCornerRadius = j_int(jStyle, "boxCornerRadius");

    const auto jLogic = jRoot["logic"].toObject();
    mClockTimerInterval = j_double(jLogic, "clockTimerInterval");
    mImageTimerInterval = j_double(jLogic, "imageTimerInterval");
}
