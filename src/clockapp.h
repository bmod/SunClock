#include "clock.h"

#include <chrono>
#include <SFML/Graphics.hpp>

#include "config.h"

#pragma once

constexpr int secondsInDay = 86400;


class ClockApp {
public:
    ClockApp();

    bool isRunning();
    void update();

private:
    void updateFlags();
    void draw();
    void handleInput();

    Config mConf;
    TimePoint mCurrentTime;
    Clock mClock;
    sf::RenderWindow mWindow;

    sf::Clock mDrawTimer;
    sf::Clock mSkyTimer;

    bool mIsDisplayDirty = true;
    bool mIsDragging = false && mConf.isDragTimeEnabled();
    bool mIsLeftButtonPressed = false;
};