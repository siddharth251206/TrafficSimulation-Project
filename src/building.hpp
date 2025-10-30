#pragma once
#include <SFML/Graphics.hpp>

class Building
{
public:
    Building(const sf::Vector2f& center, const sf::Vector2f& size, sf::Color color)
    {
        m_shape.setSize(size);
        m_shape.setOrigin(size / 2.f);
        m_shape.setPosition(center);
        m_shape.setFillColor(color);
        m_shape.setOutlineThickness(2.f);
        m_shape.setOutlineColor(sf::Color(30, 30, 35));
    }

    void draw(sf::RenderWindow& window) const { window.draw(m_shape); }

private:
    sf::RectangleShape m_shape;
};
