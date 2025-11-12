#pragma once
#include "car.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Junction;

class Road : public std::enable_shared_from_this<Road>
{
public:
    Road(const sf::Vector2f& start, const sf::Vector2f& end);

    void update(sf::Time elapsed);
    // Road geometry is drawn by DoubleRoad; this draws cars only
    void draw(sf::RenderWindow& window) const;
    void draw_cars(sf::RenderWindow& window) const;

    void add(std::unique_ptr<Car> car);
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
    size_t get_car_count() const { return m_cars.size(); }
    bool operator==(const Road& other) const;

    static constexpr float SAFE_GAP = 60.f;
    float get_travel_time() const;// Calculates road cost based on traffic

private:
    sf::Vector2f m_start, m_end, m_direction;
    float m_length;
    sf::VertexArray m_model;
    std::vector<std::unique_ptr<Car>> m_cars;
    std::pair<std::weak_ptr<Junction>, std::weak_ptr<Junction>> m_junctions;
};