#pragma once

#include "config.h"

#include <chrono>

class Panel
{
public:
    using TimePoint = std::chrono::system_clock::time_point;

    explicit Panel(const Config& conf, const PanelData& data);

    void draw(sf::RenderWindow& window);

    void updateText(TimePoint currentTime);
    void setRect(sf::Rect<float> rect);

protected:
    std::string stringForBounds() const;
    std::string bigText(TimePoint time);

    const Config& mConfig;
    const PanelData& mData;
    sf::RectangleShape mRectShape;
    sf::Text mBigText;
    sf::FloatRect mBigTextRect;
    sf::Text mSmallText;
    sf::FloatRect mSmallTextRect;
    sf::Shader mShader;
};
