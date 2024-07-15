#pragma once

#include <filesystem>

#include <SFML/Graphics.hpp>

using TimePoint = std::chrono::system_clock::time_point;

namespace sf {
    class Text;
}

namespace sfutil {
    sf::FloatRect stringBounds(sf::Text& text, const std::string& s);
    std::string res(const std::string& relPath);
    float manhattanLength(const sf::Vector2f& v);
    float length(const sf::Vector2f& v);
}


