#include "double_road.hpp"
#include "app_utility.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <optional>

// UPDATE: Constructor now accepts max_speed
DoubleRoad::DoubleRoad(const sf::Vector2f& start, const sf::Vector2f& end, float max_speed, float width, bool has_divider)
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

    // CRITICAL FIX: Pass max_speed to the single Road constructors!
    // This ensures the pathfinder knows this is a highway vs city street.
    m_forward = std::make_shared<Road>(start + offset, end + offset, max_speed);
    m_reverse = std::make_shared<Road>(end - offset, start - offset, max_speed);
    
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
        
        // (Procedural texture generation omitted for brevity, keeping your existing code here is fine)
        if (!loaded) { /* ... keep your existing generation code here ... */ } 
        
        if (sRoadTex && sRoadTex->getSize().x > 0) 
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
            rect.setTextureRect(sf::IntRect({0, 0}, { static_cast<int>(length), static_cast<int>(lane_width) }));
        }
        else
        {
            rect.setFillColor(sf::Color(50, 50, 55));
        }
        window.draw(rect);
    };

    draw_lane_sprite(m_forward);
    draw_lane_sprite(m_reverse);

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

    m_forward->draw_cars(window);
    m_reverse->draw_cars(window);
}

void DoubleRoad::add_to_forward(std::unique_ptr<Car> car) { m_forward->add(std::move(car)); }
void DoubleRoad::add_to_reverse(std::unique_ptr<Car> car) { m_reverse->add(std::move(car)); }