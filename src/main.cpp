#include "clock.h"

#include <filesystem>
#include <utility>
#include <chrono>

#include <SFML/Graphics.hpp>

#include "config.h"
#include "panel.h"
#include "log.h"

#include <cmath>

int main(int argc, char* argv[]) {
    LOG(INFO) << "Clock 2 SFML";
    Config conf;
    Clock clock(conf);


    // create the window
    sf::RenderWindow window(sf::VideoMode(conf.screenSize().x, conf.screenSize().y), "CLOCK");
    window.setVerticalSyncEnabled(true);

    // run the program as long as the window is open
    while (window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) exit(0);
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        // clear the window with black color
        window.clear(sf::Color::Black);

        clock.update();
        clock.draw(window);

        // draw everything here...
        // sf::Text textHour;
        // textHour.setFont(conf.fontLight());
        // textHour.setString("21");
        // textHour.setCharacterSize(250);
        // textHour.setFillColor(sf::Color::Cyan);
        // window.draw(textHour);
        //
        // sf::Text textMin;
        // textMin.setFont(conf.fontMedium());
        // textMin.setString(":brown");
        // textMin.setCharacterSize(250);
        // textMin.setFillColor(sf::Color::Cyan);
        // textMin.setPosition(textHour.getPosition() + sf::Vector2f(textHour.getGlobalBounds().width, 0));
        // window.draw(textMin);
        //
        // sf::RectangleShape rect(sf::Vector2f(120.f, 50.f));
        // rect.setFillColor(sf::Color::Transparent);
        // rect.setOutlineThickness(1.f);
        // rect.setOutlineColor(sf::Color::Red);
        // rect.setPosition(textHour.getGlobalBounds().getPosition());
        // rect.setSize(textHour.getGlobalBounds().getSize());
        // window.draw(rect);

        // end the current frame
        window.display();
    }

    return 0;
}