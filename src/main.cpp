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
    if (sf::Shader::isAvailable())
    {
        LOG(INFO) << "No shaders available on this hardware";
    }

    // Create the window
    sf::RenderWindow window(sf::VideoMode(conf.screenSize().x, conf.screenSize().y), "CLOCK");
    window.setVerticalSyncEnabled(true);

    // run the program as long as the window is open
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                exit(0);
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        clock.update();
        clock.draw(window);

        window.display();

        // sf::sleep(sf::seconds(0.5f));
    }

    return 0;
}