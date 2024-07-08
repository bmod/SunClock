#pragma once

#include "config.h"

class Panel
{
public:
    explicit Panel(const Config& conf, const PanelData& data);

    void draw(sf::RenderWindow& window);

    void setRect(sf::Rect<float> rect);

protected:
    const Config& mConfig;
    const PanelData& mData;
    sf::RectangleShape mRectShape;
    sf::Text mBigText;
    sf::Text mSmallText;
};
