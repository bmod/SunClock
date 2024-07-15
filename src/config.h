#pragma once


#include <SFML/Graphics.hpp>
#include <memory>

#include <nlohmann/json.hpp>

struct Airport {
    float latitude = 0;
    float longitude = 0;
    std::string iata;
    std::string timezone;
};

class AirportDB {
public:
    AirportDB(const std::string& filePath);
    bool getAirport(const std::string& iata, Airport& ap);

private:
    std::unordered_map<std::string, Airport> mAirports;
};

class PanelData
{
public:
    using GeoLocation = std::pair<float, float>;

    enum TimeUnit {
        Hours, Minutes, Seconds
    };

    explicit PanelData(const TimeUnit& unit, const Airport& airport);

    [[nodiscard]] bool hasTimeZone() const { return !timeZoneName().empty(); }
    [[nodiscard]] const std::string& timeZoneName() const;
    [[nodiscard]] const TimeUnit& timeUnit() const;
    [[nodiscard]] const std::string& displayName() const;
    GeoLocation geoCoordinate() const;

protected:
    const TimeUnit mUnit;
    const Airport mAirport;
};

class Config
{
    using PanelDataList = std::vector<std::unique_ptr<PanelData>>;

public:
    Config();

    const sf::Vector2i& screenSize() const;
    void setScreenSize(int width, int height);
    const sf::Vector2i& panelCount() const;
    const sf::Font& fontLight() const;
    const sf::Font& fontRegular() const;
    const sf::Font& fontMedium() const;
    const PanelDataList& panelDatas() const;
    const bool startFullscreen() const;

    float margin() const { return 24; }
    float spacing() const { return 8; }
    float textMargin() const { return 18; }

private:
    static void loadFont(sf::Font& font, const char* fileName);
    void loadConfig();

    sf::Vector2i mPanelCount;
    sf::Vector2i mScreenSize;
    sf::Font mFontLight;
    sf::Font mFontRegular;
    sf::Font mFontMedium;
    PanelDataList mPanelTypes;
    bool mStartFullScreen;
};
