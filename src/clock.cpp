#include "clock.h"

#include <cmath>

Clock::Clock(const Config& conf): mConf(conf) {
    createPanels();
}

void Clock::update() {
    for (int i = 0, len = mPanels.size(); i < len; i++) {
        mPanels[i]->setRect(panelRectangle(i));
    }
}

void Clock::draw(sf::RenderWindow& window) {
    for (auto& p: mPanels) {
        p->draw(window);
    }
}

void Clock::createPanels() {
    for (auto& pd: mConf.panelDatas())
        mPanels.emplace_back(std::make_unique<Panel>(mConf, *pd));
}

sf::Rect<float> Clock::screenRect() const {
    return {
            0,
            0,
            static_cast<float>(mConf.screenSize().x),
            static_cast<float>(mConf.screenSize().y)
    };
}


sf::Rect<float> Clock::panelRectangle(int i) const {
    const float margin = mConf.margin();
    // const float margin = 0;
    const float spacing = mConf.spacing();
    // const float spacing = 0;
    const int panelCountX = mConf.panelCount().x;
    const int panelCountY = mConf.panelCount().y;

    sf::Rect<float> contRect{
        margin,
        margin,
        static_cast<float>(mConf.screenSize().x) - (margin * 2),
        static_cast<float>(mConf.screenSize().y) - (margin * 2)
    };

    const int contWidth = mConf.screenSize().x;
    const int contHeight = mConf.screenSize().y;
    const float x = i % panelCountX;
    const float y = std::floor(i / float(panelCountX));

    const sf::Vector2f panelSize(
            contRect.width / panelCountX - (spacing * (panelCountX - 1)) / (float) panelCountX,
            contRect.height / panelCountY - (spacing * (panelCountY - 1)) / (float) panelCountX
            );


    return {
            margin + x * panelSize.x + spacing * x,
            margin + y * panelSize.y + spacing * y,
            panelSize.x,
            panelSize.y
    };
}