#include "car.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <cmath>
#include <cstdint>

Car::Car(const std::weak_ptr<Road>& road, const sf::Texture* texture) : m_road(road)
{
    if (auto road_ptr = m_road.lock())
        m_position = road_ptr->get_point_at_distance(0.f);

    // If a texture is provided, construct and configure the sprite (SFML 3)
    if (texture)
    {
        m_visual.emplace<sf::Sprite>(*texture);
        sf::Sprite& m_sprite = std::get<sf::Sprite>(m_visual);
        const sf::FloatRect bounds = m_sprite.getLocalBounds();
        const sf::Vector2f originPoint(bounds.size.x / 2.f, bounds.size.y / 2.f);
        m_sprite.setOrigin(originPoint);
        m_sprite.setScale({ 0.5f, 0.5f });
    }
    else
    {
        // Fallback rectangle styling (when no texture is provided)
        m_visual.emplace<sf::RectangleShape>(sf::RectangleShape({ CAR_LENGTH, CAR_LENGTH }));
        sf::RectangleShape& m_model = std::get<sf::RectangleShape>(m_visual);
        m_model.setOrigin({ CAR_LENGTH / 2, CAR_LENGTH / 2 });
        m_model.setFillColor(
            sf::Color(
                static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255)),
                static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255)),
                static_cast<std::uint8_t>(RNG::instance().getFloat(100, 255))
            )
        );
    }

    // --- IDM property randomization ---
    m_max_speed = RNG::instance().getFloat(110.f, 160.f);
    m_max_acceleration = RNG::instance().getFloat(40.f, 60.f);
    m_brake_deceleration = RNG::instance().getFloat(80.f, 100.f);
    m_time_headway = RNG::instance().getFloat(1.1f, 2.0f);
}

void Car::update(sf::Time elapsed)
{
    const float dt = elapsed.asSeconds();

    // Kinematics update
    m_relative_distance += (m_speed + 0.5f * m_acceleration * dt) * dt;
    m_speed += m_acceleration * dt;
    if (m_speed < 0.f)
        m_speed = 0.f;

    if (auto road_ptr = m_road.lock())
    {
        // Clamp to road length and notify junction if we reached the end
        const float road_len = road_ptr->getLength();
        if (m_relative_distance >= road_len)
        {
            m_relative_distance = road_len;
        }

        // Update position and orientation
        m_position = road_ptr->get_point_at_distance(m_relative_distance);

        if (sf::Sprite* m_sprite = std::get_if<sf::Sprite>(&m_visual))
        {
            m_sprite->setPosition(m_position);
            const sf::Vector2f direction = road_ptr->get_direction();
            const float angle_rad = std::atan2(direction.y, direction.x);
            m_sprite->setRotation(sf::radians(angle_rad));
        }
        else
        {
            sf::RectangleShape& m_model = std::get<sf::RectangleShape>(m_visual);
            m_model.setPosition(m_position);
        }
    }
}

void Car::draw(sf::RenderWindow& window)
{
    if (sf::Sprite* m_sprite = std::get_if<sf::Sprite>(&m_visual))
        window.draw(*m_sprite);
    else if (sf::RectangleShape* m_model = std::get_if<sf::RectangleShape>(&m_visual))
        window.draw(*m_model);
}
void Car::set_path(std::deque<std::weak_ptr<Road>> new_path){
    m_path=std::move(new_path);
}
std::weak_ptr<Road> Car::get_next_road_in_path(){
    // Logic to get the first road from m_path will go here.
    if(m_path.empty()){   
        return{};
}
return m_path.front();
 
}
void Car::advance_path(){
    // Logic to remove the first road from m_path will go here.
    if(!m_path.empty()){
        m_path.pop_front();
    }
}