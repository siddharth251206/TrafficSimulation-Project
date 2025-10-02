#include "traffic_map.hpp"
#include <ranges>

void TrafficMap::add_road(const sf::Vector2f& start_pos, const sf::Vector2f& end_pos)
{
    const std::shared_ptr<Junction> start_junction = get_or_create_junction(start_pos);
    const std::shared_ptr<Junction> end_junction = get_or_create_junction(end_pos);
    const auto new_road = std::make_shared<Road>(start_pos, end_pos);

    // Link road to junctions
    new_road->setStartJunction(start_junction);
    new_road->setEndJunction(end_junction);

    // Link junctions to road
    start_junction->add_road(new_road);
    end_junction->add_road(new_road);

    m_roads.push_back(new_road);
}

std::shared_ptr<Junction> TrafficMap::get_or_create_junction(const sf::Vector2f& position)
{
    if (const auto it = m_junctions.find(position); it != m_junctions.end())
        return it->second; // Found an entry (coords, junction) -> return junction

    auto new_junction = std::make_shared<Junction>(position);
    m_junctions[position] = new_junction;
    return new_junction;
}

void TrafficMap::update(sf::Time elapsed)
{
    // Update junctions first to handle car transfers
    for (const auto& junction : m_junctions | std::views::values)   // Returns the value for each pair
        junction->update(elapsed);

    for (const auto& road : m_roads)
        road->update(elapsed);      // which also updates cars on them
}

void TrafficMap::draw(sf::RenderWindow& window) const
{
    for (const auto& road : m_roads)
        road->draw(window);         // which also draws cars on them

    // To draw junctions if they have visuals later
    // for (auto const& [pos, junction] : m_junctions) {
    //     junction->draw(window);
    // }
}

std::weak_ptr<Road> TrafficMap::get_road(size_t index) const
{
    if (index < m_roads.size())
        return m_roads[index];
    return {};
}

void TrafficMap::clear()
{
    m_roads.clear();
    m_junctions.clear();
}