#include "building.hpp"
#include "app_utility.hpp"

Building::Building(const sf::Vector2f& center, const sf::Vector2f& size, sf::Color color)
{
    sf::RectangleShape rect(size);
    rect.setOrigin(size / 2.f);
    rect.setPosition(center);
    rect.setFillColor(color);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(30, 30, 35));
    m_visual.emplace<sf::RectangleShape>(rect);
}

Building::Building(const sf::Vector2f& center, const sf::Vector2f& size, const std::string& texturePath)
{
    // Try to load texture; if it fails, build a rectangle fallback
    if (auto full = AssetHelper::resolve_asset_path(texturePath))
    {
        m_texture.emplace();
        if (m_texture->loadFromFile(*full))
        {
            sf::Sprite sprite(*m_texture);
            const sf::Vector2u texSize = m_texture->getSize();
            sprite.setOrigin({ static_cast<float>(texSize.x) / 2.f, static_cast<float>(texSize.y) / 2.f });
            sprite.setPosition(center);
            // Scale to requested size
            if (texSize.x > 0u && texSize.y > 0u)
                sprite.setScale({ size.x / static_cast<float>(texSize.x), size.y / static_cast<float>(texSize.y) });
            m_visual.emplace<sf::Sprite>(sprite);
            return;
        }
        // If loading failed, discard the optional and fall back
        m_texture.reset();
    }

    // Fallback rectangle
    sf::RectangleShape rect(size);
    rect.setOrigin(size / 2.f);
    rect.setPosition(center);
    rect.setFillColor(sf::Color(90, 90, 110));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(30, 30, 35));
    m_visual.emplace<sf::RectangleShape>(rect);
}

void Building::draw(sf::RenderWindow& window) const
{
    if (const sf::Sprite* sp = std::get_if<sf::Sprite>(&m_visual))
        window.draw(*sp);
    else
        window.draw(std::get<sf::RectangleShape>(m_visual));
}
