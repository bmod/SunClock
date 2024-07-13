#include "clock.h"

#include <chrono>
#include <cmath>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "config.h"
#include "log.h"


int main(int argc, char* argv[]) {
    LOG(INFO) << "Clock 2 SFML";
    Config conf;
    Clock clock(conf);

    // Check
    // if (sf::Shader::isAvailable()) {
    //     LOG(INFO) << "No shaders available on this hardware";
    // }

    // Create the window
    sf::RenderWindow window(sf::VideoMode(conf.screenSize().x, conf.screenSize().y),
        "CLOCK",
        conf.startFullscreen() ? sf::Style::Fullscreen : sf::Style::Default);
    window.setVerticalSyncEnabled(true);
    LOG(INFO) << "Resolution: " << window.getSize().x << ", " << window.getSize().y;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                exit(0);
            } else if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::Resized) {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                conf.setScreenSize(event.size.width, event.size.height);
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                LOG(INFO) << "Right here:" << sf::Mouse::getPosition().x;
            }
            window.clear(sf::Color(0x334455FF));

            clock.update();
            clock.draw(window);

            window.display();
        }
    } //0.571428571
    return 0;
}