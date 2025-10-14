#include "traffic_map.hpp"
#include <memory>
#include <ranges>

void TrafficMap::add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos)
{
    const std::shared_ptr<Junction> start_junction = get_or_create_junction(start_pos);
    const std::shared_ptr<Junction> end_junction = get_or_create_junction(end_pos);
    const auto new_road = std::make_shared<Road>(start_pos, end_pos);

    new_road->setStartJunction(start_junction);
    new_road->setEndJunction(end_junction);

    start_junction->add_road(new_road);
    end_junction->add_road(new_road);

    m_single_roads.push_back(new_road);
}

void TrafficMap::add_double_road(
    const sf::Vector2f& start_pos,
    const sf::Vector2f& end_pos,
    float width,
    bool need_divider
)
{
    const std::shared_ptr<Junction> start_junction = get_or_create_junction(start_pos);
    const std::shared_ptr<Junction> end_junction = get_or_create_junction(end_pos);
    const auto new_double_road = std::make_shared<DoubleRoad>(start_pos, end_pos, width, need_divider);

    new_double_road->get_forward()->setStartJunction(start_junction);
    new_double_road->get_reverse()->setStartJunction(end_junction);
    new_double_road->get_forward()->setEndJunction(end_junction);
    new_double_road->get_reverse()->setEndJunction(start_junction);

    start_junction->add_road(new_double_road->get_forward());
    start_junction->add_road(new_double_road->get_reverse());
    end_junction->add_road(new_double_road->get_forward());
    end_junction->add_road(new_double_road->get_reverse());

    m_double_roads.push_back(new_double_road);
}

std::shared_ptr<Junction> TrafficMap::get_or_create_junction(const sf::Vector2f& position)
{
    sf::Vector2i grid = { static_cast<int>(std::floor(position.x / 40.f)),
                          static_cast<int>(std::floor(position.y / 40.f)) };
    // I deserve an award for coding this
    for (short i{ -1 }; i <= 1; ++i)
    {
        for (short j{ -1 }; j <= 1; ++j)
        {
            if (const auto it = m_junctions.find({ grid.x + i, grid.y + j });
                it != m_junctions.end())
                for (auto junc_range : it->second)
                {
                    if (point_in_circle(junc_range->get_location(), 20, position))
                        return junc_range;
                }
        }
    }

    auto new_junction = std::make_shared<Junction>(position);
    m_junctions[grid].push_back(new_junction);
    return new_junction;
}

std::shared_ptr<Junction> TrafficMap::get_junction(const sf::Vector2f& position)
{
    sf::Vector2i grid = { static_cast<int>(std::floor(position.x / 40.f)),
                          static_cast<int>(std::floor(position.y / 40.f)) };
    for (short i{ -1 }; i <= 1; ++i)
    {
        for (short j{ -1 }; j <= 1; ++j)
        {
            if (const auto it = m_junctions.find({ grid.x + i, grid.y + j });
                it != m_junctions.end())
                for (auto junc_range : it->second)
                {
                    if (point_in_circle(junc_range->get_location(), 20, position))
                        return junc_range;
                }
        }
    }
    return nullptr;
}

void TrafficMap::update(sf::Time elapsed)
{
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
    for (const auto& road : m_double_roads)
        road->draw(window);
    for (const auto& road : m_single_roads)
        road->draw(window);

    for (const auto& junction_grid : m_junctions | std::views::values)
    {
        for (auto junction : junction_grid)
            junction->draw(window);
    }
}

std::shared_ptr<Road> TrafficMap::get_single_road(size_t index) const
{
    if (index < m_single_roads.size())
        return m_single_roads[index];
    return nullptr;
}

std::shared_ptr<DoubleRoad> TrafficMap::get_double_road(size_t index) const
{
    if (index < m_double_roads.size())
        return m_double_roads[index];
    return nullptr;
}

void TrafficMap::clear()
{
    m_double_roads.clear();
    m_single_roads.clear();
    m_junctions.clear();
}