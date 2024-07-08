#include "config.h"

#include <chrono>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>
#include "log.h"

using json = nlohmann::json;


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

PanelData::PanelData(const TimeUnit& unit, const std::string& tzName): mUnit(unit), mTzName(tzName) {
    // Assert proper time zone, will throw if timezone is incorrect
    if (!tzName.empty()) {
        std::chrono::zoned_time(tzName, std::chrono::system_clock::now());
        LOG(INFO) << "Using timezone: " << tzName;
    }
}

const std::string& PanelData::timeZoneName() const {
    return mTzName;
}

Config::Config() {
    loadConfig();

    loadFont(mFontLight, "res/DMMono-Light.ttf");
    loadFont(mFontRegular, "res/DMMono-Regular.ttf");
    loadFont(mFontMedium, "res/DMMono-Medium.ttf");
}

const sf::Vector2i& Config::screenSize() const { return mScreenSize; }

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

void Config::loadFont(sf::Font& font, const char* fileName) {
    LOG(INFO) << "Loading file: " << fileName;
    std::filesystem::path fontPath(fileName);
    if (!std::filesystem::exists(fileName)) {
        LOG(FATAL) << "File not found: " << fileName;
    }

    font.loadFromFile(fileName);
}

void Config::loadConfig() {
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
    const json::array_t& jPanels = jData["panels"];

    if (jPanels.size() != mPanelCount.x * mPanelCount.y) {
        LOG(FATAL) << "Panel count mismatch (check panelCount and number of defined panels)";
    }

    for (int i = 0, len = jPanels.size(); i < len; i++) {
        const auto& jPanel = jPanels[i];
        auto unit = timeUnit(jPanel["timeUnit"]);

        std::string tz;
        if (jPanel.contains("timeZone"))
            tz = jPanel["timeZone"].get<std::string>();

        mPanelTypes.emplace_back(std::make_unique<PanelData>(unit, tz));
    }
}