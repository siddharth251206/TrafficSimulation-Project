#include "double_road.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

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
    const float lane_half = (m_width - divider_thickness) / 2.f;
    const sf::Color road_color(50, 50, 55);
    const sf::Color edge_color(35, 35, 40);

    // Helper to draw a quad given centerline endpoints and half-width
    auto draw_strip = [&](sf::Vector2f a, sf::Vector2f b, float half_w, const sf::Color& color)
    {
        const sf::Vector2f perp = { m_perp_dir.x * half_w, m_perp_dir.y * half_w };
        sf::ConvexShape quad(4);
        quad.setPoint(0, a + perp);
        quad.setPoint(1, a - perp);
        quad.setPoint(2, b - perp);
        quad.setPoint(3, b + perp);
        quad.setFillColor(color);
        window.draw(quad);
    };

    // Draw lane surfaces (two one-way lanes)
    draw_strip(m_forward->get_start(), m_forward->get_end(), lane_half, road_color);
    draw_strip(m_reverse->get_start(), m_reverse->get_end(), lane_half, road_color);

    // Optional central divider
    if (m_has_divider && divider_thickness > 0.f)
    {
        draw_strip(center_start, center_end, divider_thickness / 2.f, sf::Color(180, 140, 0));
    }

    // Light road edges for subtle outline
    draw_strip(m_forward->get_start(), m_forward->get_end(), 2.f, edge_color);
    draw_strip(m_reverse->get_start(), m_reverse->get_end(), 2.f, edge_color);

    // Draw cars on top
    m_forward->draw_cars(window);
    m_reverse->draw_cars(window);
}

void DoubleRoad::add_to_forward(std::unique_ptr<Car> car) { m_forward->add(std::move(car)); }

void DoubleRoad::add_to_reverse(std::unique_ptr<Car> car) { m_reverse->add(std::move(car)); }
