#include "panel.h"

#include <cassert>
#include <chrono>


#include "date/tz.h"
#include "log.h"
#include "sfutil.h"

using namespace std::chrono;

sf::FloatRect stringBounds(sf::Text& text, const std::string& s) {
    const auto oldString = text.getString();
    text.setString(s);
    const auto rect = text.getLocalBounds();
    text.setString(oldString);
    return rect;
}

Panel::Panel(const Config& conf, const PanelData& data) : mConfig(conf), mData(data) {
    mSmallText.setFont(mConfig.fontLight());
    mBigText.setFont(mConfig.fontMedium());

    mRectShape.setOutlineColor(sf::Color(0xFF00FF55));
    mRectShape.setOutlineThickness(1);
    mRectShape.setFillColor(sf::Color(0x203040FF));

    if (mData.hasTimeZone()) {
        mShader.loadFromFile(_res("sky.vert"), _res("sky.frag"));
    }
}

void Panel::draw(sf::RenderWindow& window) {

    window.draw(mRectShape, &mShader);
    window.draw(mSmallText);

    window.draw(mBigTextShape);

    window.draw(mBigText);
}

void Panel::updateText(const TimePoint currentTime) {
    mBigText.setString(bigText(currentTime));
    mSmallText.setString(mData.timeZoneName());
}
void Panel::setRect(sf::Rect<float> rect) {
    mRectShape.setPosition(rect.getPosition());
    mRectShape.setSize(rect.getSize());

    auto utcNow = std::chrono::system_clock::now();

    updateText(utcNow);

    // Update layout
    {
        auto textMargin = mConfig.textMargin();

        // Measure with widest characters
        mBigTextRect = stringBounds(mBigText, stringForBounds());
        // LOG(INFO) << "String for bounds: " << stringForBounds() << " (text: " << mBigText.getString().toAnsiString()
        //           << ") --> bounds: " << mBigTextRect.left << ", " << mBigTextRect.top << ", " << mBigTextRect.width
        //           << ", " << mBigTextRect.height;

        mBigText.setCharacterSize(200);
        mBigText.setPosition(
                {(rect.left + rect.width) - mBigTextRect.width - textMargin, rect.top + textMargin - mBigTextRect.top});

        mBigTextShape.setPosition(mBigText.getPosition());
        mBigTextShape.setSize(mBigTextRect.getSize());
        mBigTextShape.setOutlineThickness(1);
        mBigTextShape.setOutlineColor(sf::Color::Red);
        mBigTextShape.setFillColor(sf::Color::Transparent);


        mSmallTextRect = stringBounds(mSmallText, mData.timeZoneName());

        mSmallText.setCharacterSize(20);
        auto smallBB = mSmallTextRect;
        mSmallText.setPosition(
                {rect.left + textMargin, rect.top + rect.height - smallBB.top - smallBB.height - textMargin});
    }
}
std::string Panel::stringForBounds() const {
    switch (mData.timeUnit()) {
        case PanelData::Seconds:
        case PanelData::Minutes:
            return ":00";
        case PanelData::Hours:
            return "00";
    }
    return {};
}
std::string Panel::bigText(const TimePoint currentTime) {
    switch (mData.timeUnit()) {
        case PanelData::Seconds:
            return date::format(":%S", time_point_cast<seconds>(currentTime));

        case PanelData::Minutes:
            return date::format(":%M", time_point_cast<minutes>(currentTime));

        case PanelData::Hours: {
            if (mData.hasTimeZone()) {
                auto t = date::make_zoned(mData.timeZoneName(), currentTime);
                return date::format("%H", t);
            }
            auto nowHours = time_point_cast<hours>(currentTime);
            return date::format(":%H", nowHours);
        }
    }
    return {};
}
