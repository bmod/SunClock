#pragma once

#include "config.h"

#include <memory>

#include "panel.h"

class Clock
{
public:
    Clock(const Config& conf);

    void draw(sf::RenderWindow& window, const TimePoint& currentTime);
    void setSkyDirty();
    void setResolutionScale(float scale);

private:
    void createPanels();
    void updateText();

    sf::Rect<float> screenRect() const;
    sf::Rect<float> panelRectangle(int index) const;

    const Config& mConf;
    std::vector<std::unique_ptr<Panel>> mPanels;
};
