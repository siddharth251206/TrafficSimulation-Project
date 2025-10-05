#include "double_road.hpp"
#include <SFML/Graphics.hpp>

DoubleRoad::DoubleRoad(const sf::Vector2f &start, const sf::Vector2f &end, float width)
    : m_width(width)
{
    const sf::Vector2f diff = end - start;
    if (diff.length() == 0.f)
        return;

    m_perp_dir = {diff.normalized().y, -diff.normalized().x};
    m_lane_offset = (m_width / 2.f);
    sf::Vector2f offset = (m_perp_dir * m_lane_offset);

    m_forward = std::make_shared<Road>(start + offset, end + offset);
    m_reverse = std::make_shared<Road>(end - offset, start - offset);
}

void DoubleRoad::update(sf::Time elapsed) const
{
    m_forward->update(elapsed);
    m_reverse->update(elapsed);
}

void DoubleRoad::draw(sf::RenderWindow &window, bool draw_divider) const
{
    sf::Vector2f start = (m_forward->get_start() + m_reverse->get_end()) / 2.f;
    sf::Vector2f end = (m_forward->get_end() + m_reverse->get_start()) / 2.f;

    if (draw_divider)
    {
        // Thick divider (yellow, 10.f wide)
        float divider_thickness = m_width / 4;
        sf::Vector2f divider_perp = m_perp_dir * (divider_thickness / 2.f);
        sf::ConvexShape divider(4);
        divider.setPoint(0, start + divider_perp);
        divider.setPoint(1, start - divider_perp);
        divider.setPoint(2, end - divider_perp);
        divider.setPoint(3, end + divider_perp);
        divider.setFillColor(sf::Color::Yellow);
        window.draw(divider);
    }

    // Draw each road and cars
    m_forward->draw(window);
    m_reverse->draw(window);
}

void DoubleRoad::add_to_forward(std::unique_ptr<Car> car) { m_forward->add(std::move(car)); }

void DoubleRoad::add_to_reverse(std::unique_ptr<Car> car) { m_reverse->add(std::move(car)); }
