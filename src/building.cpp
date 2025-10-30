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
    m_texture.emplace();
    if (AssetHelper::try_load_texture(*m_texture, texturePath, "building"))
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

    // Generate a procedural building sprite (window grid)
    m_texture.emplace();
    const unsigned texW = 128, texH = 128;
    sf::RenderTexture rt({ texW, texH });
    // Base wall color
    sf::RectangleShape base({ static_cast<float>(texW), static_cast<float>(texH) });
    base.setFillColor(sf::Color(85, 85, 105));
    rt.draw(base);
    // Windows grid
    const unsigned cols = 6, rows = 6;
    for (unsigned r = 0; r < rows; ++r)
    {
        for (unsigned c = 0; c < cols; ++c)
        {
            float x0 = 8.f + c * ((texW - 16.f) / cols);
            float y0 = 8.f + r * ((texH - 16.f) / rows);
            float w = ((texW - 16.f) / cols) - 6.f;
            float h = ((texH - 16.f) / rows) - 6.f;
            sf::RectangleShape win({ w, h });
            win.setPosition({ x0, y0 });
            win.setFillColor(sf::Color(190, 195, 210));
            rt.draw(win);
        }
    }
    rt.display();
    if (m_texture->loadFromImage(rt.getTexture().copyToImage()))
    {
        sf::Sprite sprite(*m_texture);
        const sf::Vector2u texSize = m_texture->getSize();
        sprite.setOrigin({ static_cast<float>(texSize.x) / 2.f, static_cast<float>(texSize.y) / 2.f });
        sprite.setPosition(center);
        if (texSize.x > 0u && texSize.y > 0u)
            sprite.setScale({ size.x / static_cast<float>(texSize.x), size.y / static_cast<float>(texSize.y) });
        m_visual.emplace<sf::Sprite>(sprite);
        return;
    }

    // Final fallback rectangle
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
