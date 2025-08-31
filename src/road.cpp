#include "road.hpp"
#include "app_utility.hpp"
#include <random>

// Road methods

Road::Road(const sf::Vector2f& start, const sf::Vector2f& end)
    : m_start(start), m_end(end), m_direction((end - start).normalized()),
      m_model(sf::PrimitiveType::Lines, 2), m_length((end - start).length())
{
    m_model[0] = sf::Vertex{ m_start };
    m_model[1] = sf::Vertex{ m_end };
}

const sf::Vector2f& Road::get_direction() const noexcept { return m_direction; }

sf::Vector2f Road::get_point_at_distance(const float position) const noexcept
{
    return m_start + position * m_direction;
}

void Road::update(const sf::Time elapsed)
{
    for (auto& car : m_cars)
        car.update(elapsed);
}

void Road::draw(sf::RenderWindow& window) const
{
    window.draw(m_model);
    for (const auto& car : m_cars)
        car.draw(window);
}
void Road::add(const Car& car) { m_cars.push_back(car); }

float Road::getLength() const { return m_length; }

Junction* Road::getEndJunction() const { return m_junctions.second; }

// Junction Methods

Junction::Junction(const sf::Vector2f& location) : j_position(location) {}

void Junction::accept_car(Car* entering_car) { j_car_entered = entering_car; }

bool Junction::handle_car_redirection()
{
    if (!j_car_entered || j_road_list.size() <= 1)
        return false;
    else
    {
        Road* new_road = nullptr;
        do
        {
            new_road = &j_road_list[RNG::instance().getIndex(0, j_road_list.size())];
        } while (new_road != j_car_entered->m_road);
        j_car_entered->m_road = new_road;
    }
    j_car_entered = nullptr;
    return true;
}

void Junction::add_road(const Road& new_road) { j_road_list.push_back(new_road); }

const sf::Vector2f& Junction::get_location() const { return j_position; }

bool Junction::operator==(const Junction& other) const noexcept
{
    return (j_position == other.j_position);
}
