#include "config.h"

#include <QFile>
#include <QTimeZone>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

void loadClocks(std::vector<std::unique_ptr<ClockData>>& outClocks) {
    QFile jsonFile(":/config.json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal("Failed to open config file");

    const auto doc = QJsonDocument::fromJson(jsonFile.readAll());

    auto jClocks = doc["clocks"].toArray();
    for (auto jClock: jClocks) {
        auto jClockObj = jClock.toObject();
        auto timeZoneName = jClockObj["timezone"].toString();
        auto clock = std::make_unique<ClockData>(timeZoneName);
        outClocks.emplace_back(std::make_unique<ClockData>(timeZoneName));
    }
}

Config::Config() {
    loadClocks(mClocks);
}

const std::vector<std::unique_ptr<ClockData>>& Config::clocks() const {
    return mClocks;
}