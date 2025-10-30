#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <variant>
#include <string>

class Building
{
public:
    // Rectangle fallback building
    Building(const sf::Vector2f& center, const sf::Vector2f& size, sf::Color color);

    // Sprite-based building (falls back to rect if texture cannot be loaded)
    explicit Building(const sf::Vector2f& center, const sf::Vector2f& size, const std::string& texturePath);

    void draw(sf::RenderWindow& window) const;

private:
    // If sprite is used, keep the texture alive inside the object
    std::optional<sf::Texture> m_texture;
    std::variant<sf::RectangleShape, sf::Sprite> m_visual;
};
