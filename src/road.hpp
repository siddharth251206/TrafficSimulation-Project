// #pragma once
// #include "car.hpp"
// #include <SFML/Graphics.hpp>
// #include <memory>
// #include <vector>

// class Junction;

// class Road
// {
// public:
//     Road(const sf::Vector2f& start, const sf::Vector2f& end);

//     void update(sf::Time elapsed);
//     void draw(sf::RenderWindow& window) const;

//     void add(std::unique_ptr<Car> car);   // Road owns cars
//     float getLength() const { return m_length; }
//     const sf::Vector2f& get_direction() const { return m_direction; }

//     Junction* getEndJunction() const { return m_junctions.second; }
//     void setEndJunction(Junction* junction) { m_junctions.second = junction; }

//     sf::Vector2f get_point_at_distance(float distance) const {
//         return m_start + distance * m_direction;
//     }

// private:
//     sf::Vector2f m_start, m_end, m_direction;
//     float m_length;
//     sf::VertexArray m_model;
//     std::vector<std::unique_ptr<Car>> m_cars;
//     std::pair<Junction*, Junction*> m_junctions; // start, end
// };

#pragma once
#include "car.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Junction;

class Road
{
public:
    Road(const sf::Vector2f& start, const sf::Vector2f& end);

    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window) const;

    void add(std::unique_ptr<Car> car);   // Road owns cars
    float getLength() const { return m_length; }
    const sf::Vector2f& get_direction() const { return m_direction; }

    Junction* getEndJunction() const { return m_junctions.second; }
    void setEndJunction(Junction* junction) { m_junctions.second = junction; }
    void setStartJunction(Junction* junction) { m_junctions.first = junction; }

    const sf::Vector2f& get_start() const { return m_start; }
    const sf::Vector2f& get_end() const { return m_end; }

    sf::Vector2f get_point_at_distance(float distance) const {
        return m_start + distance * m_direction;
    }

private:
    sf::Vector2f m_start, m_end, m_direction;
    float m_length;
    sf::VertexArray m_model;
    std::vector<std::unique_ptr<Car>> m_cars;
    std::pair<Junction*, Junction*> m_junctions; // start, end
};