#pragma once

#include "config.h"
#include "roundedrectangleshape.h"
#include "sfutil.h"


class Panel {
public:
    explicit Panel(const Config& conf, const PanelData& data);

    void draw(sf::RenderTarget& renderTarget);
    void update(const TimePoint& currentTime, const sf::FloatRect& rect);
    void setSkyDirty();
    void setResolutionScale(float scale);

protected:
    void renderSky();
    std::string stringForBounds() const;
    std::string bigText(const TimePoint& time) const;

private:
    sf::Vector2u skyTextureSize() const;

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
    sf::Shader mTexShader;
    sf::RenderTexture mSkyTexture;
    sf::Clock mSkyTextureClock;
    float mSkyResolutionScale;
    bool mSkyTextureDirty = true;
};
