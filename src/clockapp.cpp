#include "clockapp.h"

#include "date/date.h"

float secondsSinceEpoch() {
    const auto now = std::chrono::system_clock::now();

    // transform the time into a duration since the epoch
    const auto epoch = now.time_since_epoch();

    // cast the duration into seconds
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

    // return the number of seconds
    return (float) ms.count() / 1000.0f;
}

ClockApp::ClockApp()
    : mClock(mConf), mWindow(sf::VideoMode(mConf.screenSize().x, mConf.screenSize().y), "CLOCK",
                             mConf.startFullscreen() ? sf::Style::Fullscreen : sf::Style::Default) {
    mWindow.setFramerateLimit(mConf.baseFrameRate());
}
bool ClockApp::isRunning() {
    return mWindow.isOpen();
}

void ClockApp::update() {
    handleInput();
    updateFlags();
    draw();
}
void ClockApp::updateFlags() {
    if (mIsDragging) {
        mCurrentTime = std::chrono::system_clock::now();

        // Allow dragging to change the time quickly
        const auto mousePos = sf::Mouse::getPosition(mWindow);
        const auto xNormalized = mousePos.x / static_cast<float>(mWindow.getSize().x);

        TimePoint startOfDay = std::chrono::floor<date::days>(mCurrentTime);
        // TimePoint startOfDay = std::chrono::floor<date::days>(mCurrentTime);

        mCurrentTime = startOfDay + std::chrono::seconds(1) * static_cast<int>(xNormalized * secondsInDay / 0.9);
        mClock.setSkyDirty();// faster sky update while interacting

        mIsDisplayDirty = true;
    }

    if (mDrawTimer.getElapsedTime() > mConf.updateInterval()) {
        mDrawTimer.restart();

        mCurrentTime = std::chrono::system_clock::now();
        mIsDisplayDirty = true;
    }

    if (mSkyTimer.getElapsedTime() > mConf.skyUpdateInterval()) {
        mSkyTimer.restart();
        mIsDisplayDirty = true;
        mClock.setSkyDirty();
    }
}
void ClockApp::draw() {
    if (mIsDisplayDirty) {
        mWindow.clear(sf::Color::Black);
        mClock.draw(mWindow, mCurrentTime);
        mWindow.display();
        mIsDisplayDirty = false;
    }
}
void ClockApp::handleInput() {
    sf::Event event{};
    while (mWindow.pollEvent(event)) {
        // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        //     window.close();

        switch (event.type) {
            case sf::Event::Closed: {
                mWindow.close();
                break;
            }
            case sf::Event::Resized: {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                mWindow.setView(sf::View(visibleArea));
                mConf.setScreenSize(event.size.width, event.size.height);
                break;
            }
            case sf::Event::MouseButtonPressed: {
                if (mConf.isDragTimeEnabled()) {
                    mIsDragging = true;
                    mClock.setResolutionScale(mConf.skyResolutionScaleInteractive());
                }
                break;
            }
            case sf::Event::MouseButtonReleased: {
                if (mConf.isDragTimeEnabled()) {
                    mIsDragging = false;
                    mClock.setResolutionScale(mConf.skyResolutionScale());
                }
                break;
            }
            case sf::Event::KeyPressed: {
                if (event.key.code == sf::Keyboard::Escape) {
                    mWindow.close();
                }
                break;
            }
        }
    }
}