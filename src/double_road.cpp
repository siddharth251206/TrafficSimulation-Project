#include "double_road.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <optional>
#include "app_utility.hpp"

DoubleRoad::DoubleRoad(const sf::Vector2f& start, const sf::Vector2f& end, float width, bool has_divider)
    : m_width(width)
{
    const sf::Vector2f diff = end - start;
    const float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
    if (len == 0.f)
        return;

    const sf::Vector2f dir = { diff.x / len, diff.y / len };
    m_perp_dir = { dir.y, -dir.x };
    m_lane_offset = (m_width / 2.f);
    const sf::Vector2f offset = { m_perp_dir.x * m_lane_offset, m_perp_dir.y * m_lane_offset };

    m_forward = std::make_shared<Road>(start + offset, end + offset);
    m_reverse = std::make_shared<Road>(end - offset, start - offset);
    m_has_divider = has_divider;
}

void DoubleRoad::update(sf::Time elapsed) const
{
    m_forward->update(elapsed);
    m_reverse->update(elapsed);
}

void DoubleRoad::draw(sf::RenderWindow& window) const
{
    // Compute centerline ends
    sf::Vector2f center_start = (m_forward->get_start() + m_reverse->get_end()) / 2.f;
    sf::Vector2f center_end   = (m_forward->get_end() + m_reverse->get_start()) / 2.f;

    // Visual parameters
    const float divider_thickness = m_has_divider ? (m_width * 0.12f) : 0.f;
    const float lane_width = (m_width - divider_thickness);

    // Load road texture once (static)
    static std::optional<sf::Texture> sRoadTex;
    if (!sRoadTex)
    {
        sRoadTex.emplace();
        bool loaded = AssetHelper::try_load_texture(*sRoadTex, "assets/road.png", "road");
        if (!loaded)
            loaded = AssetHelper::try_load_texture(*sRoadTex, "assets/road_tile.png", "road");
        if (!loaded)
        {
            // Generate a simple procedural road texture (asphalt + dashed center line)
            const unsigned texW = 512, texH = 64;
            sf::RenderTexture rt({ texW, texH });
            rt.clear(sf::Color(50, 50, 55));
            // Subtle speckle: draw thin transparent lines
            for (unsigned y = 0; y < texH; y += 8)
            {
                sf::RectangleShape speck({ static_cast<float>(texW), 1.f });
                speck.setPosition({ 0.f, static_cast<float>(y) });
                speck.setFillColor(sf::Color(60, 60, 65, 40));
                rt.draw(speck);
            }
            // Center dashed line
            const float dashLen = 24.f;
            const float gapLen = 16.f;
            float posx = 0.f;
            while (posx < texW)
            {
                sf::RectangleShape dash({ std::min(dashLen, static_cast<float>(texW) - posx), 4.f });
                dash.setOrigin({ 0.f, 2.f });
                dash.setPosition({ posx, texH / 2.f });
                dash.setFillColor(sf::Color(235, 220, 100));
                rt.draw(dash);
                posx += dashLen + gapLen;
            }
            rt.display();
            sRoadTex->loadFromImage(rt.getTexture().copyToImage());
        }
        if (sRoadTex)
            sRoadTex->setRepeated(true);
        else
            sRoadTex.reset();
    }

    auto draw_lane_sprite = [&](const std::shared_ptr<Road>& lane)
    {
        const float length = lane->getLength();
        const sf::Vector2f a = lane->get_start();
        const sf::Vector2f b = lane->get_end();
        const sf::Vector2f mid = (a + b) / 2.f;
        const sf::Vector2f dir = lane->get_direction();
        const float angle = std::atan2(dir.y, dir.x);

        sf::RectangleShape rect({ length, lane_width });
        rect.setOrigin({ length / 2.f, lane_width / 2.f });
        rect.setPosition(mid);
        rect.setRotation(sf::radians(angle));

        if (sRoadTex)
        {
            rect.setTexture(&*sRoadTex);
            // Tile texture along the length
            rect.setTextureRect(sf::IntRect({0, 0}, { static_cast<int>(length), static_cast<int>(lane_width) }));
        }
        else
        {
            rect.setFillColor(sf::Color(50, 50, 55));
        }
        window.draw(rect);
    };

    // Draw two lane sprites
    draw_lane_sprite(m_forward);
    draw_lane_sprite(m_reverse);

    // Optional central divider (simple yellow strip)
    if (m_has_divider && divider_thickness > 0.f)
    {
        const sf::Vector2f mid = (center_start + center_end) / 2.f;
        const sf::Vector2f diff = center_end - center_start;
        const float len = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        const float angle = std::atan2(diff.y, diff.x);
        sf::RectangleShape divider({ len, divider_thickness });
        divider.setOrigin({ len / 2.f, divider_thickness / 2.f });
        divider.setPosition(mid);
        divider.setRotation(sf::radians(angle));
        divider.setFillColor(sf::Color(180, 140, 0));
        window.draw(divider);
    }

    // Draw cars on top
    m_forward->draw_cars(window);
    m_reverse->draw_cars(window);
}

void DoubleRoad::add_to_forward(std::unique_ptr<Car> car) { m_forward->add(std::move(car)); }

void DoubleRoad::add_to_reverse(std::unique_ptr<Car> car) { m_reverse->add(std::move(car)); }
