#include "clockapp.h"

#include "date/date.h"
#include "log.h"

float secondsSinceEpoch() {
    const auto now = std::chrono::system_clock::now();

    // transform the time into a duration since the epoch
    const auto epoch = now.time_since_epoch();

    // cast the duration into seconds
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

    // return the number of seconds
    return static_cast<float>(ms.count()) / 1000.0f;
}

ClockApp::ClockApp()
    : mClock(mConf), mWindow(sf::VideoMode(mConf.screenSize().x, mConf.screenSize().y), "CLOCK",
                             mConf.startFullscreen() ? sf::Style::Fullscreen : sf::Style::Default) {
    mWindow.setFramerateLimit(mConf.baseFrameRate());
}

bool ClockApp::isRunning() const {
    return mWindow.isOpen();
}

void ClockApp::update() {
    handleInput();
    draw();

    // WARNING: This also syncs to vblank.
    // If the display is not refreshed,
    // the game loop will saturate a CPU thread with nonsense
    mWindow.display();
}

void ClockApp::handleInput() {
    sf::Event event{};
    while (mWindow.pollEvent(event)) {
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
                    mWindow.setFramerateLimit(60);
                }
                break;
            }
            case sf::Event::MouseButtonReleased: {
                if (mConf.isDragTimeEnabled()) {
                    mIsDragging = false;
                    mClock.setResolutionScale(mConf.skyResolutionScale());
                    mWindow.setFramerateLimit(mConf.baseFrameRate());
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

TimePoint ClockApp::currentTime() const {
    if (mIsDragging) {
        const auto mousePos = sf::Mouse::getPosition(mWindow);
        const auto xNormalized = mousePos.x / static_cast<float>(mWindow.getSize().x);
        const TimePoint startOfDay = std::chrono::floor<date::days>(std::chrono::system_clock::now());
        return startOfDay + std::chrono::seconds(1) * static_cast<int>(xNormalized * secondsInDay / 0.9);
    }

    return std::chrono::system_clock::now();
}
void ClockApp::draw() {
    mWindow.clear(sf::Color::Black);
    mClock.draw(mWindow, currentTime());
}
