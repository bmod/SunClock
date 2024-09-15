#include "clock.h"

#include <SFML/Graphics.hpp>
#include <chrono>

#include "config.h"

#pragma once

constexpr int secondsInDay = 86400;

class Timer {
public:
    Timer(const sf::Time& interval) : mInterval(interval) {}

    void restart() {
        mClock.restart();
    }

    bool hasElapsed() const {
        return mClock.getElapsedTime() > mInterval;
    }

private:
    sf::Time mInterval;
    sf::Clock mClock;
};

class ClockApp {
public:
    ClockApp();

    bool isRunning() const;
    void update();

private:
    void handleInput();
    void updateFlags();
    void draw();

    Config mConf;
    TimePoint mCurrentTime;
    Clock mClock;
    sf::RenderWindow mWindow;

    sf::Clock mSkyTimer;

    bool mIsDragging = false && mConf.isDragTimeEnabled();
    bool mIsLeftButtonPressed = false;
};