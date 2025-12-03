#include "traffic_map.hpp"
#include <memory>
#include <numeric>
#include <ranges>
#include <cmath>

// UPDATE: Taking max_speed
void TrafficMap::add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos, float max_speed)
{
    const std::shared_ptr<Junction> start_junction = get_or_create_junction(start_pos);
    const std::shared_ptr<Junction> end_junction = get_or_create_junction(end_pos);

    // CRITICAL: Pass max_speed to the Road constructor
    // (Ensure your Road.cpp constructor is updated as per previous instructions)
    const auto new_road = std::make_shared<Road>(start_pos, end_pos, max_speed);

    new_road->setStartJunction(start_junction);
    new_road->setEndJunction(end_junction);

    start_junction->add_road(new_road);
    end_junction->add_road(new_road);

    m_single_roads.push_back(new_road);
    m_all_roads.push_back(new_road);
}

// UPDATE: Taking max_speed
void TrafficMap::add_double_road(
    const sf::Vector2f& start_pos,
    const sf::Vector2f& end_pos,
    float max_speed,
    float width,
    bool need_divider
)
{
    const std::shared_ptr<Junction> start_junction = get_or_create_junction(start_pos);
    const std::shared_ptr<Junction> end_junction = get_or_create_junction(end_pos);

    // Assuming DoubleRoad constructor now accepts max_speed. 
    // If not, you must allow setting it on get_forward() / get_reverse()
    const auto new_double_road =
        std::make_shared<DoubleRoad>(start_pos, end_pos, max_speed, width, need_divider);

    // Connect Forward Lane
    new_double_road->get_forward()->setStartJunction(start_junction);
    new_double_road->get_forward()->setEndJunction(end_junction);
    
    // Connect Reverse Lane
    new_double_road->get_reverse()->setStartJunction(end_junction);
    new_double_road->get_reverse()->setEndJunction(start_junction);

    // Add to Junctions
    start_junction->add_road(new_double_road->get_forward());
    start_junction->add_road(new_double_road->get_reverse());
    end_junction->add_road(new_double_road->get_forward());
    end_junction->add_road(new_double_road->get_reverse());

    m_double_roads.push_back(new_double_road);
    m_all_roads.push_back(new_double_road->get_forward());
    m_all_roads.push_back(new_double_road->get_reverse());
}

std::shared_ptr<Junction> TrafficMap::get_or_create_junction(const sf::Vector2f& position)
{
    // Use Constants for Grid Logic
    sf::Vector2i grid = { 
        static_cast<int>(std::floor(position.x / GRID_SIZE)),
        static_cast<int>(std::floor(position.y / GRID_SIZE)) 
    };

    // Check 3x3 neighbors
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            auto it = m_junctions.find({ grid.x + i, grid.y + j });
            if (it != m_junctions.end())
            {
                for (const auto& junc : it->second)
                {
                    // Use Constant SNAP_RADIUS
                    if (point_in_circle(junc->get_location(), SNAP_RADIUS, position))
                        return junc;
                }
            }
        }
    }

    auto new_junction = std::make_shared<Junction>(position);
    m_junctions[grid].push_back(new_junction);
    return new_junction;
}

std::shared_ptr<Junction> TrafficMap::get_junction(const sf::Vector2f& position)
{
    // Reusing the same grid logic to find existing junctions quickly
    sf::Vector2i grid = { 
        static_cast<int>(std::floor(position.x / GRID_SIZE)),
        static_cast<int>(std::floor(position.y / GRID_SIZE)) 
    };

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            auto it = m_junctions.find({ grid.x + i, grid.y + j });
            if (it != m_junctions.end())
            {
                for (const auto& junc : it->second)
                {
                    if (point_in_circle(junc->get_location(), SNAP_RADIUS, position))
                        return junc;
                }
            }
        }
    }
    return nullptr;
}

void TrafficMap::update(sf::Time elapsed)
{
    // Update logic remains same
    for (const auto& road : m_double_roads)
        road->update(elapsed);
    for (const auto& road : m_single_roads)
        road->update(elapsed);

    for (const auto& junction_grid : m_junctions | std::views::values)
    {
        for (auto junction : junction_grid)
            junction->update(elapsed);
    }
}

void TrafficMap::draw(sf::RenderWindow& window) const
{
    // Draw Roads First
    for (const auto& road : m_double_roads)
        road->draw(window);
    for (const auto& road : m_single_roads)
        road->draw(window);

    // Draw Junctions (and Lights) Last so they appear on top
    for (const auto& junction_grid : m_junctions | std::views::values)
    {
        for (auto junction : junction_grid)
            junction->draw(window);
    }
}

// ... Getters (get_single_road, get_double_road, get_random_road, etc.) remain unchanged ...
// Just ensure you copy them over from your original file.

std::shared_ptr<Road> TrafficMap::get_single_road(size_t index) const {
    if (index < m_single_roads.size()) return m_single_roads[index];
    return nullptr;
}

std::shared_ptr<DoubleRoad> TrafficMap::get_double_road(size_t index) const {
    if (index < m_double_roads.size()) return m_double_roads[index];
    return nullptr;
}

std::shared_ptr<Road> TrafficMap::get_random_road() {
    if (m_all_roads.empty()) return nullptr;
    size_t index = RNG::instance().getIndex(0, m_all_roads.size() - 1);
    return m_all_roads[index];
}

size_t TrafficMap::get_car_count() const {
    return std::accumulate(m_all_roads.begin(), m_all_roads.end(), static_cast<size_t>(0),
        [](size_t sum, const auto& road) { return sum + road->get_car_count(); });
}

void TrafficMap::clear() {
    m_double_roads.clear();
    m_single_roads.clear();
    m_junctions.clear();
    m_all_roads.clear();
}