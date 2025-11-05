#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

enum class UrbanElementType
{
    Tree,
    StreetLamp,
    Bench,
    BusStop,
    ParkingLot,
    Fountain,
    GrassPatch
};

class UrbanElement
{
public:
    UrbanElement(const sf::Vector2f& position, UrbanElementType type);

    void draw(sf::RenderWindow& window) const;
    const sf::Vector2f& get_position() const { return m_position; }
    UrbanElementType get_type() const { return m_type; }

private:
    sf::Vector2f m_position;
    UrbanElementType m_type;
    sf::CircleShape m_shape;
    sf::RectangleShape m_rect_shape;
    sf::Color m_color;
    bool m_use_circle;

    // Optional sprite-based rendering
    bool m_use_sprite{false};
    std::unique_ptr<sf::Sprite> m_sprite;
    std::shared_ptr<sf::Texture> m_texture;

    void setup_appearance();
};
