#include "sfutil.h"

#include "log.h"

#include <cmath>

sf::FloatRect sfutil::stringBounds(sf::Text& text, const std::string& s) {
    const auto oldString = text.getString();
    text.setString(s);
    const auto rect = text.getLocalBounds();
    text.setString(oldString);
    return rect;
}

std::string sfutil::res(const std::string& relPath) {
    auto inPath = "res/" + relPath;
    auto absPath = std::filesystem::absolute(inPath);
    if (!exists(absPath)) {
        LOG(FATAL) << "File not found: " << absPath;
        exit(-1);
    }
    return absPath;
}

float sfutil::manhattanLength(const sf::Vector2f& v) {
    return v.x * v.x + v.y * v.y;
}

float sfutil::length(const sf::Vector2f& v) {
    return std::sqrt(manhattanLength(v));
}