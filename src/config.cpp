#include "config.h"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
#include "log.h"

using json = nlohmann::json;

AirportDB::AirportDB(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        LOG(FATAL) << "File not found: " << filePath;
    }
    std::ifstream airportsStream(filePath);
    auto jAirports = nlohmann::json::parse(airportsStream);
    for (auto& jAirport: jAirports) {
        Airport ap;

        if (jAirport.contains("iata_code"))
            ap.iata = jAirport["iata_code"];

        if (jAirport.contains("timezone"))
            ap.timezone = jAirport["timezone"];

        if (jAirport.contains("latitude_deg")) {
            std::string lat = jAirport["latitude_deg"];
            ap.latitude = std::stod(lat);
        }

        if (jAirport.contains("longitude_deg")) {
            std::string lon = jAirport["longitude_deg"];
            ap.longitude = std::stod(lon);
        }
        mAirports.insert({ap.iata, ap});
    }
}

bool AirportDB::getAirport(const std::string& iata, Airport& ap) {
    auto it = mAirports.find(iata);
    if (it == mAirports.end())
        return false;

    ap.latitude = it->second.latitude;
    ap.longitude = it->second.longitude;
    ap.iata = it->second.iata;
    ap.timezone = it->second.timezone;
    return true;
}

PanelData::TimeUnit timeUnit(const std::string& str) {
    if (str == "hours")
        return PanelData::Hours;
    if (str == "minutes")
        return PanelData::Minutes;
    if (str == "seconds")
        return PanelData::Seconds;

    LOG(FATAL) << "Unknown time unit: " << str;
    return PanelData::Seconds;
}

PanelData::PanelData(const TimeUnit& unit, const Airport& airport)
    : mUnit(unit), mAirport(airport) {
}

const std::string& PanelData::timeZoneName() const {
    return mAirport.timezone;
}
const PanelData::TimeUnit& PanelData::timeUnit() const {
    return mUnit;
}
const std::string& PanelData::displayName() const {
    return mAirport.iata;
}
GeoLocation PanelData::geoCoordinate() const {
    return {mAirport.latitude, mAirport.longitude};
}

Config::Config() {
    loadConfig();

    loadFont(mFontLight, "res/DMMono-Light.ttf");
    loadFont(mFontRegular, "res/DMMono-Regular.ttf");
    loadFont(mFontMedium, "res/DMMono-Medium.ttf");
}

const sf::Vector2i& Config::screenSize() const {
    return mScreenSize;
}

void Config::setScreenSize(int width, int height) {
    mScreenSize = {width, height};
}

const sf::Vector2i& Config::panelCount() const {
    return mPanelCount;
}

const sf::Font& Config::fontLight() const {
    return mFontLight;
}

const sf::Font& Config::fontRegular() const {
    return mFontRegular;
}

const sf::Font& Config::fontMedium() const {
    return mFontMedium;
}

const Config::PanelDataList& Config::panelDatas() const {
    return mPanelTypes;
}

const bool Config::startFullscreen() const {
    return mStartFullScreen;
}

void Config::loadFont(sf::Font& font, const char* fileName) {
    LOG(INFO) << "Loading file: " << fileName;
    std::filesystem::path fontPath(fileName);
    if (!std::filesystem::exists(fileName)) {
        LOG(FATAL) << "File not found: " << fileName;
    }

    font.loadFromFile(fileName);
}

void Config::loadConfig() {
    AirportDB airports("res/airports.json");

    const auto filePath = "res/config.json";
    if (!std::filesystem::exists(filePath)) {
        LOG(FATAL) << "File not found: " << filePath;
    }
    std::ifstream is(filePath);
    const json& jData = json::parse(is);

    mScreenSize.x = jData["displaySize"][0];
    mScreenSize.y = jData["displaySize"][1];
    mPanelCount.x = jData["panelCount"][0];
    mPanelCount.y = jData["panelCount"][1];
    mStartFullScreen = jData["startFullscreen"];
    const json::array_t& jPanels = jData["panels"];

    if (jPanels.size() != mPanelCount.x * mPanelCount.y) {
        LOG(FATAL) << "Panel count mismatch (check panelCount and number of defined panels)";
    }

    for (int i = 0, len = jPanels.size(); i < len; i++) {
        const auto& jPanel = jPanels[i];
        auto unit = timeUnit(jPanel["timeUnit"]);

        std::string iata = jPanel.contains("iata") ? jPanel["iata"] : "";

        Airport ap;
        airports.getAirport(iata, ap);
        mPanelTypes.emplace_back(std::make_unique<PanelData>(unit, ap));
    }
}
