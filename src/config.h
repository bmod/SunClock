#pragma once

#include <QJsonObject>
#include <airportsdata.h>
#include <qcolor.h>

#define SETTING(TYPE, NAME, GROUP) \
    private: \
    TYPE m_##NAME; \
    [[nodiscard]] const TYPE NAME() const { return m_##NAME; } \
    void read_##NAME(const QJsonObject& obj) { m_##NAME = j_##TYPE(obj, #NAME); }


static QJsonValue jChild(const QJsonObject& obj, const QString& key);
static QJsonArray jArray(const QJsonObject& obj, const QString& key);
static QString j_QString(const QJsonObject& obj, const QString& key);
static double j_double(const QJsonObject& obj, const QString& key);
static int j_int(const QJsonObject& obj, const QString& key);


class Config {
public:
    Config();

    [[nodiscard]] const apdata::LocationList& locations() const { return mLocations; }
    [[nodiscard]] const QColor& backgroundColor() const { return mBackgroundColor; }
    [[nodiscard]] const QString& fontName() const { return mFontName; }
    [[nodiscard]] int margin() const { return mMargin; }
    [[nodiscard]] int boxSpacing() const { return mBoxSpacing; }
    [[nodiscard]] int boxCornerRadius() const { return mBoxCornerRadius; }
    [[nodiscard]] double clockTimerInterval() const { return mClockTimerInterval; }
    [[nodiscard]] double imageTimerInterval() const { return mImageTimerInterval; }

    SETTING(double, count, logic)

    // void read_count(const QJsonObject& obj) {
    //     m_count = j_double(obj, "count");
    // }

private:
    void loadConfig();

    apdata::LocationList mLocations;
    QColor mBackgroundColor;
    QString mFontName;
    int mBoxSpacing = 0;
    int mBoxCornerRadius = 0;
    int mMargin = 0;
    double mClockTimerInterval;
    double mImageTimerInterval;
};