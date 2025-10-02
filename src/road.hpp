#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "car.hpp"

class Junction;

class Road
{
public:
    Road(const sf::Vector2f& start, const sf::Vector2f& end);

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

    void add(std::unique_ptr<Car> car);// Road owns cars
    float getLength() const { return m_length; }
    const sf::Vector2f& get_direction() const { return m_direction; }

    std::weak_ptr<Junction> getEndJunction() const;
    void setEndJunction(const std::shared_ptr<Junction>& junction);
    void setStartJunction(const std::shared_ptr<Junction>& junction);

    const sf::Vector2f& get_start() const { return m_start; }
    const sf::Vector2f& get_end() const { return m_end; }

    sf::Vector2f get_point_at_distance(float distance) const
    {
        return m_start + distance * m_direction;
    }

    bool is_empty() const { return m_cars.empty(); }

    static constexpr float SAFE_GAP = 60.f;// Car size (50) + gap (10)

private:
    sf::Vector2f m_start, m_end, m_direction;
    float m_length;
    sf::VertexArray m_model;
    std::vector<std::unique_ptr<Car>> m_cars;
    // Use weak_ptr to prevent circular references and memory leaks
    std::pair<std::weak_ptr<Junction>, std::weak_ptr<Junction>> m_junctions;// start, end
};
