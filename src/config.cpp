#include "config.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <airportdb.h>


#include <nlohmann/json.hpp>

using json = nlohmann::json;


static void loadClocks(std::vector<std::unique_ptr<ClockData>>& outClocks) {
    QFile jsonFile(":/config.json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
        qFatal("Failed to open config file");

    AirportDB airportsDb(":/airports.json");

    const auto doc = QJsonDocument::fromJson(jsonFile.readAll());

    auto jClocks = doc["clocks"].toArray();
    for (auto jClock: jClocks) {
        auto jClockObj = jClock.toObject();
        auto timeZoneName = jClockObj["timezone"].toString();
        auto iata = jClockObj["iata"].toString();
        Airport airport;
        if (!airportsDb.getAirport(iata, airport)) {
            const char* cIata = iata.toLatin1();
            qFatal("Failed to get airport: %s", cIata);
        }
        outClocks.emplace_back(std::make_unique<ClockData>(timeZoneName, airport.location));
    }
}

Config::Config() : mAirportDB(":airports.json") {
    loadClocks(mClocks);
}

const std::vector<std::unique_ptr<ClockData>>& Config::clocks() const {
    return mClocks;
}


// sf::Color parseColorStr(const std::string& colStr) {
//     unsigned int number;
//     std::stringstream ss;
//     ss << std::hex << colStr;
//     ss >> number;
//     const int r = number >> 16 & 0xFF;
//     const int g = number >> 8 & 0xFF;
//     const int b = number & 0xFF;
//
//     return sf::Color(r, g, b);
// }
