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

struct GeoLocation {
    GeoLocation(const float lat, const float lon) : latitude(lat), longitude(lon) {}
    float latitude;
    float longitude;
};

class PanelData {
public:
    enum TimeUnit { Hours, Minutes, Seconds };

    explicit PanelData(const TimeUnit& unit, const Airport& airport, const sf::Color& bigTextColor);

    [[nodiscard]] bool hasTimeZone() const {
        return !timeZoneName().empty();
    }
    [[nodiscard]] const std::string& timeZoneName() const;
    [[nodiscard]] const TimeUnit& timeUnit() const;
    [[nodiscard]] const std::string& displayName() const;
    [[nodiscard]] GeoLocation geoCoordinate() const;
    [[nodiscard]] sf::Color bigTextColor() const;

protected:
    const TimeUnit mUnit;
    const Airport mAirport;
    const sf::Color mBigTextColor;
};

class Config {
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
    sf::Vector2f skyRangeX() const;
    sf::Vector2f skyRangeY() const;
    float timeSpeed() const;
    bool isDragTimeEnabled() const;

    float panelMargin() const {
        return 16;
    }
    float panelSpacing() const {
        return 12;
    }
    float textMargin() const {
        return 18;
    }
    sf::Time updateInterval() const;
    sf::Time skyUpdateInterval() const;
    float skyResolutionScale() const;
    float skyResolutionScaleInteractive() const;
    int baseFrameRate() const;

private:
    static void loadFont(sf::Font& font, const char* fileName);
    void loadConfig();

    sf::Vector2i mPanelCount;
    sf::Vector2i mScreenSize;
    sf::Vector2f mSkyRangeX;
    sf::Vector2f mSkyRangeY;
    float mTimeSpeed;
    bool mTimeDragEnabled;
    float mSkyResolutionScale;
    float mSkyResolutionScaleInteractive;
    sf::Font mFontLight;
    sf::Font mFontRegular;
    sf::Font mFontMedium;
    PanelDataList mPanelTypes;
    bool mStartFullScreen;
    int mBaseFrameRate;
    sf::Time mUpdateInterval;
    sf::Time mSkyUpdateInterval;

};
