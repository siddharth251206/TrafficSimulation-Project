#include "car.hpp"
#include "app_utility.hpp"
#include "road.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <variant>

Car::Car(const std::weak_ptr<Road>& road, const sf::Texture* texture, float start_distance, const sf::Color& color)
    : m_road(road), m_relative_distance(start_distance), m_state(CarState::Spawning),
      m_fade_timer(FADE_IN_DURATION)
{
    if (auto road_ptr = m_road.lock())
        m_position = road_ptr->get_point_at_distance(m_relative_distance);

    // If a texture is provided, construct and configure the sprite (SFML 3)
    if (texture)
    {
        m_visual.emplace<sf::Sprite>(*texture);
        sf::Sprite& m_sprite = std::get<sf::Sprite>(m_visual);
        const sf::FloatRect bounds = m_sprite.getLocalBounds();
        const sf::Vector2f originPoint(bounds.size.x / 2.f, bounds.size.y / 2.f);
        m_sprite.setOrigin(originPoint);
        m_sprite.setScale({ 0.5f, 0.5f });
        m_sprite.setColor(color); // ✅ tint sprite with passed color
    }
    else
    {
        // Fallback rectangle styling (when no texture is provided)
        m_visual.emplace<sf::RectangleShape>(sf::RectangleShape({ CAR_LENGTH, CAR_LENGTH }));
        sf::RectangleShape& m_model = std::get<sf::RectangleShape>(m_visual);
        m_model.setOrigin({ CAR_LENGTH / 2.f, CAR_LENGTH / 2.f });
        m_model.setFillColor(color); // ✅ use passed color directly
    }

    // --- IDM property randomization ---
    m_max_speed = RNG::instance().getFloat(110.f, 160.f);
    m_max_acceleration = RNG::instance().getFloat(40.f, 60.f);
    m_brake_deceleration = RNG::instance().getFloat(80.f, 100.f);
    m_time_headway = RNG::instance().getFloat(1.1f, 2.0f);

    set_alpha(0);
}

void Car::update(sf::Time elapsed)
{
    switch (m_state)
    {
    case CarState::Spawning:
        m_fade_timer -= elapsed;
        if (m_fade_timer <= sf::Time::Zero)
        {
            m_state = CarState::Driving;
            set_alpha(255);
        }
        else
        {
            const float alpha_ratio = 1.f - (m_fade_timer / FADE_IN_DURATION);
            set_alpha(static_cast<std::uint8_t>(alpha_ratio * 255));
        }
        break;
    case CarState::Driving:
        if (!m_is_finished && m_final_road.lock() && m_road.lock() == m_final_road.lock()
            && m_relative_distance >= m_destination_distance)
        {
            m_is_finished = true;
            m_state = CarState::Despawning;
            m_fade_timer = FADE_OUT_DURATION;
        }
        break;
    case CarState::Despawning:
        m_speed = 0.f;
        m_acceleration = 0.f;
        m_fade_timer -= elapsed;
        if (m_fade_timer <= sf::Time::Zero)
        {
            set_alpha(0);
        }
        else
        {
            const float alpha_ratio = m_fade_timer / FADE_OUT_DURATION;
            set_alpha(static_cast<std::uint8_t>(alpha_ratio * 255));
        }
        break;
    }

    if (m_state != CarState::Despawning)
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
                m_sprite->setRotation(sf::degrees(angle_rad * 180.f / 3.14159265f));
            }
            else
            {
                sf::RectangleShape& m_model = std::get<sf::RectangleShape>(m_visual);
                m_model.setPosition(m_position);
            }
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

void Car::set_destination(
    std::deque<std::weak_ptr<Road>> path,
    const std::weak_ptr<Road>& final_road,
    const float destination_distance
)
{
    m_path = std::move(path);
    m_final_road = final_road;
    m_destination_distance = destination_distance;
}

std::weak_ptr<Road> Car::get_next_road_in_path()
{
    if (m_path.empty())
        return {};
    return m_path.front();
}

void Car::advance_path()
{
    if (!m_path.empty())
        m_path.pop_front();
}

bool Car::is_finished() const { return m_is_finished; }

bool Car::is_ready_for_removal() const
{
    return m_state == CarState::Despawning && m_fade_timer <= sf::Time::Zero;
}

void Car::set_alpha(const std::uint8_t alpha)
{
    std::visit(
        [=]<typename ShapeType>(ShapeType& shape)
        {
            if constexpr (std::is_same_v<ShapeType, sf::Sprite>)
            {
                sf::Color color = shape.getColor();
                color.a = alpha;
                shape.setColor(color);
            }
            else if constexpr (std::is_same_v<ShapeType, sf::RectangleShape>)
            {
                sf::Color color = shape.getFillColor();
                color.a = alpha;
                shape.setFillColor(color);
            }
        },
        m_visual
    );
}
