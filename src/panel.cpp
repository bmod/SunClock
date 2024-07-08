#include "panel.h"

#include "log.h"

#include <chrono>

Panel::Panel(const Config& conf, const PanelData& data): mConfig(conf), mData(data) {
    mSmallText.setFont(mConfig.fontLight());
    mBigText.setFont(mConfig.fontMedium());

    mRectShape.setOutlineColor(sf::Color(0xFF00FF55));
    mRectShape.setOutlineThickness(1);
    mRectShape.setFillColor(sf::Color(0x203040FF));
    mBigText.setString("Big");
    mSmallText.setString("Smol");
}

void Panel::draw(sf::RenderWindow& window) {

    window.draw(mRectShape);
    window.draw(mSmallText);
    window.draw(mBigText);
}

void Panel::setRect(sf::Rect<float> rect) {
    // LOG(DEBUG) << "Set rect: " << rect.left << ", " << rect.top << ", " << rect.width << ", " << rect.height;
    mRectShape.setPosition(rect.getPosition());
    mRectShape.setSize(rect.getSize());

    // Update text
    {
        auto time = std::chrono::system_clock::now();
        if (mData.hasTimeZone()) {
            time = std::chrono::zoned_time(mData.timeZoneName(), time);
        }

        mBigText.setString(std::format("{:%H}", time));
        mSmallText.setString(mData.timeZoneName());
    }

    // Update layout
    {
        auto textMargin = mConfig.textMargin();

        mBigText.setCharacterSize(200);
        auto bigBB = mBigText.getLocalBounds();
        mBigText.setPosition({
            rect.left + rect.width - bigBB.width - textMargin,
            rect.top + textMargin - bigBB.top
        });

        mSmallText.setCharacterSize(20);
        auto smallBB = mSmallText.getLocalBounds();
        mSmallText.setPosition({
            rect.left + textMargin,
            rect.top + rect.height - smallBB.top - smallBB.height - textMargin
        });
    }
}

