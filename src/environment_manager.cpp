#include "environment_manager.hpp"
#include "app_utility.hpp"
#include <cmath>

using sf::Vector2f;

static Vector2f normalize(const Vector2f& v)
{
    const float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len <= 1e-6f)
        return {0.f, 0.f};
    return { v.x / len, v.y / len };
}

void EnvironmentManager::initialize_from_traffic_map(TrafficMap& map)
{
    m_buildings.clear();

    // Try to decorate along available double roads until nullptr
    for (size_t i = 0; ; ++i)
    {
        auto dr = map.get_double_road(i);
        if (!dr)
            break;

        // Use forward lane geometry
        auto road = dr->get_forward();
        const Vector2f a = road->get_start();
        const Vector2f b = road->get_end();
        const float width = 30.f; // visual width baseline

        place_decorations_along_road(a, b, width);

        // also sprinkle some near the midpoint, but fewer and spread wider
        const Vector2f mid{ (a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f };
        place_buildings_near_point(mid, 140.f);
    }
}

void EnvironmentManager::update(sf::Time dt)
{
    if (m_day_night)
        m_time += dt.asSeconds();
}

void EnvironmentManager::draw_environment(sf::RenderWindow& window) const
{
    for (const auto& b : m_buildings)
        b.draw(window);
}

sf::Color EnvironmentManager::get_ambient_color() const
{
    if (!m_day_night)
        return sf::Color(135, 206, 235); // sky blue

    // simple 20s day/night sine cycle
    const float t = std::fmod(m_time, 20.f) / 20.f;
    const float k = 0.5f * (1.f + std::sin((t * 2.f * 3.1415926f) - 3.1415926f/2.f)); // 0..1
    const unsigned char r = static_cast<unsigned char>(30 + 105 * k);
    const unsigned char g = static_cast<unsigned char>(30 + 150 * k);
    const unsigned char b = static_cast<unsigned char>(50 + 150 * k);
    return sf::Color(r, g, b);
}

void EnvironmentManager::place_decorations_along_road(const Vector2f& start,
                                                      const Vector2f& end,
                                                      float road_width)
{
    const Vector2f dir = normalize({ end.x - start.x, end.y - start.y });
    if (dir.x == 0.f && dir.y == 0.f)
        return;
    const Vector2f perp{ -dir.y, dir.x };

    const float length = std::sqrt((end.x - start.x)*(end.x - start.x) + (end.y - start.y)*(end.y - start.y));
    const float step = 240.f; // increase spacing to reduce crowding
    const int count = static_cast<int>(length / step);

    for (int i = 1; i < count; ++i)
    {
        const float d = static_cast<float>(i) * step;
        const Vector2f base{ start.x + dir.x * d, start.y + dir.y * d };

        // place fewer roadside items: lamps every segment, trees every other segment; push farther away
        if ((i % 2) == 0)
        {
            Vector2f lp = { base.x + perp.x * (road_width * 1.2f), base.y + perp.y * (road_width * 1.2f) };
            m_buildings.emplace_back(lp, Vector2f{ 8.f, 40.f }, BuildingType::LampPost, ZoneType::Decorative);
        }
        if ((i % 4) == 0)
        {
            Vector2f tr = { base.x - perp.x * (road_width * 1.6f), base.y - perp.y * (road_width * 1.6f) };
            m_buildings.emplace_back(tr, Vector2f{ 36.f, 48.f }, BuildingType::Tree, ZoneType::Decorative);
        }
    }
}

void EnvironmentManager::place_buildings_near_point(const Vector2f& p, float spacing)
{
    // small, sparse block around a point (4 buildings on the corners)
    const float s = spacing * 2.0f;
    const Vector2f offsets[] = {
        { -s, -s }, {  s, -s }, { -s,  s }, {  s,  s }
    };

    const BuildingType types[] = {
        BuildingType::SmallHouse, BuildingType::Apartment,
        BuildingType::Shop, BuildingType::OfficeBuilding
    };

    for (int i = 0; i < 4; ++i)
    {
        const Vector2f pos{ p.x + offsets[i].x, p.y + offsets[i].y };
        const Vector2f size{ 48.f, 48.f };
        m_buildings.emplace_back(pos, size, types[i], ZoneType::Residential);
    }
}
