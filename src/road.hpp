#pragma once

#include "app_utility.hpp"
#include "car.hpp"

#include <SFML/Graphics.hpp>
#include <optional>
#include <utility>
#include <vector>

class Car;

class Junction;

class Road
{
public:
    // Construct a road based on given start and end points.
    Road(const sf::Vector2f& start, const sf::Vector2f& end);
    // Returns the unit vector direction of the road.
    [[nodiscard]] const sf::Vector2f& get_direction() const noexcept;
    // Gets the coordinates of the point in road at 'position' distance from the start.
    [[nodiscard]] sf::Vector2f get_point_at_distance(float position) const noexcept;
    // Updates the position of all cars in the road.
    void update(sf::Time elapsed);
    // Draws the road shape onto the world.
    void draw(sf::RenderWindow& window) const;
    // Add a car to the road.
    void add(const Car& car);
    // Gets the length of the road.
    [[nodiscard]] float getLength() const;
    // To obtain the junction at the end of the road.
    [[nodiscard]] Junction* getEndJunction() const;

private:
    // Junctions associated with the end of the road.
    std::pair<Junction*, Junction*> m_junctions;
    // Coordinates in the world where the road 'starts'.
    sf::Vector2f m_start;
    // Coordinates in the world where the road 'ends'.
    sf::Vector2f m_end;
    // Unit vector in the direction the road points.
    sf::Vector2f m_direction;
    // Vector of all cars currently on the given road.
    std::vector<Car> m_cars;
    // The physical road shape to be drawn on screen.
    sf::VertexArray m_model;
    // Length of the road (end - start).
    float m_length;
};

class Junction
{
public:
    // Construct a junction with the given coordinates.
    Junction(const sf::Vector2f& location);
    // Receive a car into the junction.
    void accept_car(Car* entering_car);
    // Choose a road to travel to from the given junction.
    bool handle_car_redirection();
    // Adds a road to the given junction.
    void add_road(const Road& new_road);
    // Get the location of the junction.
    [[nodiscard]] const sf::Vector2f& get_location() const;
    // Compares two junctions.
    bool operator==(const Junction &other) const noexcept;

private:
    // Car which currently exists in the junction.
    Car* j_car_entered;
    // Coordinates of the junction.
    sf::Vector2f j_position;
    // List of roads connected to the junction.
    std::vector<Road> j_road_list;
};
