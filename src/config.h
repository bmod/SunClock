#pragma once

#include <QJsonObject>
#include <airportsdata.h>
#include <qcolor.h>


#define SETTING(TYPE, NAME, GROUP)                             \
                                                               \
public:                                                        \
    [[nodiscard]] const TYPE NAME() const { return m_##NAME; } \
                                                               \
private:                                                       \
    TYPE m_##NAME;                                             \
    void read_##NAME(const QJsonObject& obj) {                 \
        const auto groupObject = obj[#GROUP].toObject();       \
        m_##NAME = j_##TYPE(groupObject, #NAME);               \
    }


static QJsonValue jChild(const QJsonObject& obj, const QString& key);
static QJsonArray jArray(const QJsonObject& obj, const QString& key);
static QJsonObject jObject(const QJsonObject& obj, const QString& key);
static QString j_QString(const QJsonObject& obj, const QString& key);
static double j_double(const QJsonObject& obj, const QString& key);
static double j_float(const QJsonObject& obj, const QString& key);
static int j_int(const QJsonObject& obj, const QString& key);
static int j_bool(const QJsonObject& obj, const QString& key);
static QColor j_QColor(const QJsonObject& obj, const QString& key);


class Config : public QObject {
    Q_OBJECT
    Config();
public:
    static Config& get();
    [[nodiscard]] const apdata::LocationList& locations() const;

    SETTING(bool, startFullscreen, general);
    SETTING(int, windowWidth, general);
    SETTING(int, windowHeight, general);

    SETTING(QColor, backgroundColor, style)
    SETTING(QColor, clockColorTime, style);
    SETTING(QColor, clockColorTimeDark, style);
    SETTING(QColor, clockColorLocation, style);
    SETTING(QColor, clockColorLocationDark, style);
    SETTING(QColor, clockColorMinutes, style);
    SETTING(QColor, clockColorSeconds, style);
    SETTING(float, darkTextThreshold, style);
    SETTING(QString, fontName, style)
    SETTING(int, margin, style)
    SETTING(int, tileMargin, style)
    SETTING(int, boxSpacing, style)
    SETTING(int, boxCornerRadius, style)
    SETTING(float, cameraFov, style)
    SETTING(float, stretchDown, style)
    SETTING(bool, toneMap, style)
    SETTING(float, subjectHeight, style)
    SETTING(float, noiseAmount, style)

    SETTING(float, clockTimerInterval, logic)
    SETTING(float, imageTimerInterval, logic)
    SETTING(float, skyResolutionScale, logic)
    SETTING(int, skySamples, logic)

private:
    void loadConfig();
    apdata::LocationList mLocations;
};