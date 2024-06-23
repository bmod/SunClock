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
static int j_int(const QJsonObject& obj, const QString& key);
static QColor j_QColor(const QJsonObject& obj, const QString& key);


class Config : public QObject {
    Q_OBJECT
public:
    Config();

    const apdata::LocationList& locations() const;

    SETTING(QColor, backgroundColor, style)
    SETTING(QColor, clockColorTime, style);
    SETTING(QColor, clockColorTimeDark, style);
    SETTING(QColor, clockColorLocation, style);
    SETTING(QColor, clockColorMinutes, style);
    SETTING(QColor, clockColorSeconds, style);
    SETTING(double, darkTextThreshold, style);
    SETTING(QString, fontName, style)
    SETTING(int, margin, style)
    SETTING(int, boxSpacing, style)
    SETTING(int, boxCornerRadius, style)
    SETTING(double, clockTimerInterval, logic)
    SETTING(double, imageTimerInterval, logic)
    SETTING(double, skyResolutionScale, logic)
    SETTING(int, skySamples, logic)

private:
    void loadConfig();
    apdata::LocationList mLocations;
};