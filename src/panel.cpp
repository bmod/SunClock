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
    mBigText.setFillColor(mData.bigTextColor());
    mBigText.setFont(mConfig.fontMedium());

    mRectShape.setCornersRadius(10);
    mRectShape.setCornerPointCount(8);
    mRectShape.setFillColor(sf::Color::Black);

    if (mData.hasTimeZone()) {
        if (!mShader.loadFromFile(sfutil::res("sky.vert"), sfutil::res("sky.frag"))) {
            LOG(FATAL) << "Shader load error";
            exit(-1);
        }
        if (!mTexShader.loadFromFile(sfutil::res("skyTexture.vert"), sfutil::res("skyTexture.frag"))) {
            LOG(FATAL) << "Shader load error";
            exit(-1);
        }
    }
}

void Panel::renderSky() {
    const sf::Vector2 sizeInt = skyTextureSize();
    if (mSkyTexture.getSize() != sizeInt) {
        mSkyTexture.create(sizeInt.x, sizeInt.y);
        mSkyTexture.setSmooth(true);
    }
    sf::RectangleShape skyRect(mRectShape.getSize());
    skyRect.setPosition(0, 0);
    skyRect.setTextureRect({0, 0, 1, 1});
    mSkyTexture.draw(skyRect, &mShader);
}

void Panel::draw(sf::RenderTarget& renderTarget) {
    if (mSkyTextureDirty) {
        renderSky();
        mSkyTextureDirty = false;
    }

    mTexShader.setUniform("texture", mSkyTexture.getTexture());
    renderTarget.draw(mRectShape, &mTexShader);
    renderTarget.draw(mSmallText);
    renderTarget.draw(mBigText);
}

void Panel::update(const TimePoint& currentTime, const sf::FloatRect& rect) {
    // === Sky
    if (mSkyTextureClock.getElapsedTime() > mConfig.skyUpdateInterval()) {
        mSkyTextureDirty = true;// Queue for redraw
        mSkyTextureClock.restart();
    }


    // === Display Time:
    if (mData.hasTimeZone()) {
        const float lat = mData.geoCoordinate().latitude;
        const float lon = mData.geoCoordinate().longitude;
        const auto coords = suncalc::sunCoords(currentTime, lat, lon, 1);
        const auto sunVector = suncalc::sunVector(coords);
        mShader.setUniform("sunDir", sunVector);
        mShader.setUniform("skyRangeX", mConfig.skyRangeX());
        mShader.setUniform("skyRangeY", mConfig.skyRangeY());
    }

    mBigText.setString(bigText(currentTime));
    mSmallText.setString(mData.displayName());

    mRectShape.setPosition(rect.getPosition());
    mRectShape.setSize(rect.getSize());
    mRectShape.setTextureRect({0, 0, 1, 1});

    // Update layout
    {
        const auto textMargin = mConfig.textMargin();
        const sf::FloatRect contentRect{rect.left + textMargin, rect.top + textMargin, rect.width - textMargin * 2,
                                        rect.height - textMargin * 2};

        // Measure with widest characters
        mBigTextRect = sfutil::stringBounds(mBigText, stringForBounds());

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

void Panel::setSkyDirty() {
    mSkyTextureDirty = true;
}

void Panel::setResolutionScale(const float scale) {
    if (mSkyResolutionScale == scale)
        return;
    mSkyResolutionScale = scale;
    setSkyDirty();
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

std::string Panel::bigText(const TimePoint& time) const {
    switch (mData.timeUnit()) {
        case PanelData::Seconds:
            return date::format(":%S", time_point_cast<seconds>(time));

        case PanelData::Minutes:
            return date::format(":%M", time_point_cast<minutes>(time));

        case PanelData::Hours: {
            if (mData.hasTimeZone()) {
                const auto t = date::make_zoned(mData.timeZoneName(), time);
                return format("%H", t);
            }
            const auto nowHours = time_point_cast<hours>(time);
            return date::format(":%H", nowHours);
        }
    }
    return {};
}

sf::Vector2u Panel::skyTextureSize() const {
    const auto size = mRectShape.getSize();
    unsigned int width = std::round(size.x * mSkyResolutionScale);
    unsigned int height = std::round(size.y * mSkyResolutionScale);
    return {width, height};
}
