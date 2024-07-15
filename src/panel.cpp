#include "panel.h"

#include <cassert>
#include <chrono>


#include "date/tz.h"
#include "log.h"
#include "sfutil.h"
#include "suncalc.h"

using namespace std::chrono;



Panel::Panel(const Config& conf, const PanelData& data) : mConfig(conf), mData(data) {
    mSmallText.setFont(mConfig.fontLight());
    mBigText.setFont(mConfig.fontMedium());

    mRectShape.setOutlineColor(sf::Color(0xFF00FF55));
    mRectShape.setOutlineThickness(1);
    mRectShape.setFillColor(sf::Color(0x203040FF));
    mRectShape.setCornersRadius(10);
    mRectShape.setCornerPointCount(8);

    if (mData.hasTimeZone()) {
        if (!mShader.loadFromFile(sfutil::res("sky.vert"), sfutil::res("sky.frag"))) {
            LOG(FATAL) << "Shader load error";
            exit(-1);
        }
    }
}

void Panel::draw(sf::RenderWindow& window) {

    window.draw(mRectShape, &mShader);
    window.draw(mSmallText);

    // TODO: Remove this big guy altogether
    // window.draw(mBigTextShape);

    window.draw(mBigText);
}

void Panel::update(const TimePoint& currentTime, const sf::FloatRect& rect) {

    const auto coords = suncalc::sunCoords(currentTime, mData.geoCoordinate().first, mData.geoCoordinate().second, 1);
    const auto sunVector = suncalc::sunVector(coords);
    mShader.setUniform("sunDir", sunVector);

    mBigText.setString(bigText(currentTime));
    mSmallText.setString(mData.displayName());

    float diagonal = sfutil::length(rect.getSize());

    mRectShape.setPosition(rect.getPosition());
    mRectShape.setSize(rect.getSize());
    mRectShape.setTextureRect({0, 0, 1, 1});

    mShader.setUniform("resolution", rect.getSize());


    // Update layout
    {
        auto textMargin = mConfig.textMargin();
        sf::FloatRect contentRect{rect.left + textMargin, rect.top + textMargin, rect.width - textMargin * 2,
                                  rect.height - textMargin * 2};

        // Measure with widest characters
        mBigTextRect = sfutil::stringBounds(mBigText, stringForBounds());
        // LOG(INFO) << "String for bounds: " << stringForBounds() << " (text: " << mBigText.getString().toAnsiString()
        //           << ") --> bounds: " << mBigTextRect.left << ", " << mBigTextRect.top << ", " << mBigTextRect.width
        //           << ", " << mBigTextRect.height;

        mBigText.setCharacterSize(110);
        float px = contentRect.left + contentRect.width - mBigTextRect.width;
        float py = contentRect.top;

        px -= mBigTextRect.left;
        py -= mBigTextRect.top;

        mBigText.setPosition(px, py);

        mBigTextShape.setPosition(mBigText.getPosition());
        mBigTextShape.setSize(mBigTextRect.getSize());
        mBigTextShape.setOutlineThickness(1);
        mBigTextShape.setOutlineColor(sf::Color::Red);
        mBigTextShape.setFillColor(sf::Color::Transparent);


        mSmallTextRect = sfutil::stringBounds(mSmallText, mData.timeZoneName());

        mSmallText.setCharacterSize(45);
        const auto smallBB = mSmallTextRect;
        mSmallText.setPosition(rect.left + textMargin,
                               rect.top + rect.height - smallBB.top - smallBB.height - textMargin);
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
