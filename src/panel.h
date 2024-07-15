#pragma once

#include "config.h"
#include "roundedrectangleshape.h"
#include "sfutil.h"


class Panel
{
public:

    explicit Panel(const Config& conf, const PanelData& data);

    void draw(sf::RenderWindow& window);

    void update(const TimePoint& currentTime, const sf::FloatRect& rect);

protected:
    std::string stringForBounds() const;
    std::string bigText(TimePoint time);

    const Config& mConfig;
    const PanelData& mData;
    sf::RoundedRectangleShape mRectShape;
    sf::Text mBigText;
    sf::FloatRect mBigTextRect;
    sf::RectangleShape mBigTextShape;
    sf::Text mSmallText;
    sf::FloatRect mSmallTextRect;
    sf::RectangleShape mSmallTextShape;
    sf::Shader mShader;
};
