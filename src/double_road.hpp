#pragma once
#include "road.hpp"
#include <memory>

class DoubleRoad
{
public:
    DoubleRoad(const sf::Vector2f& start, const sf::Vector2f& end, float width, bool has_divider);

    std::shared_ptr<Road> get_forward() { return m_forward; }
    std::shared_ptr<Road> get_reverse() { return m_reverse; }

    void update(sf::Time elapsed) const;
    void draw(sf::RenderWindow& window) const;

    void add_to_forward(std::unique_ptr<Car> car);
    void add_to_reverse(std::unique_ptr<Car> car);

    void set_lane_offset(float offset) { m_lane_offset = offset; }
    void set_perp_dir(const sf::Vector2f& perp) { m_perp_dir = perp; }

private:
    bool m_has_divider;
    std::shared_ptr<Road> m_forward;
    std::shared_ptr<Road> m_reverse;
    sf::Vector2f m_perp_dir;// Perpendicular for lane offset
    float m_lane_offset = 0.f;// Offset from centerline
    float m_width;
};