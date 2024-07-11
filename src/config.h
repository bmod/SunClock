#pragma once


#include <memory>
#include <SFML/Graphics.hpp>


class PanelData
{
public:
    enum TimeUnit {
        Hours, Minutes, Seconds
    };

    explicit PanelData(const TimeUnit& unit, const std::string& tzName = "");

    bool hasTimeZone() const { return !timeZoneName().empty(); }
    [[nodiscard]] const std::string& timeZoneName() const;
    const TimeUnit& timeUnit() const { return mUnit; }

protected:
    const std::string mTzName;
    const TimeUnit mUnit;
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
};
