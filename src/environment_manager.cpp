#include "environment_manager.hpp"
#include "app_utility.hpp"
#include <cmath>
#include <numbers>
#include <limits>
#include <algorithm>

void EnvironmentManager::initialize_from_traffic_map(TrafficMap& map)
{
    m_buildings.clear();
    m_urban_elements.clear();
    m_zones.clear();
    m_road_segments.clear();

    // Collect road segments from map (using index getters until nullptr)
    for (size_t i = 0;; ++i)
    {
        if (auto r = map.get_single_road(i))
            m_road_segments.emplace_back(r->get_start(), r->get_end());
        else
            break;
    }
    for (size_t i = 0;; ++i)
    {
        if (auto dr = map.get_double_road(i))
        {
            if (auto f = dr->get_forward()) m_road_segments.emplace_back(f->get_start(), f->get_end());
            if (auto rv = dr->get_reverse()) m_road_segments.emplace_back(rv->get_start(), rv->get_end());
        }
        else
            break;
    }

    define_zones();

    // Populate zones with appropriate buildings
    for (const auto& zone : m_zones)
    {
        switch (zone.type)
        {
        case ZoneType::Residential:
            populate_residential_zone(zone);
            break;
        case ZoneType::Commercial:
            populate_commercial_zone(zone);
            break;
        case ZoneType::Public:
            populate_public_zone(zone);
            break;
        case ZoneType::Mixed:
            populate_residential_zone(zone);
            populate_commercial_zone(zone);
            break;
        }
    }

    add_street_infrastructure();
    add_parks_and_greenery();
}

void EnvironmentManager::define_zones()
{
    // Define zones based on the map layout from map_data.txt
    // Residential zones - quieter inner areas
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(250.f, 150.f), sf::Vector2f(400.f, 150.f)), ZoneType::Residential, 0.6f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(350.f, 550.f), sf::Vector2f(300.f, 200.f)), ZoneType::Residential, 0.7f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(1150.f, 150.f), sf::Vector2f(500.f, 300.f)), ZoneType::Residential, 0.65f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(1150.f, 1000.f), sf::Vector2f(500.f, 500.f)), ZoneType::Residential, 0.6f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(100.f, 1000.f), sf::Vector2f(500.f, 500.f)), ZoneType::Residential, 0.7f });

    // Commercial zones - along main roads and intersections
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(150.f, 320.f), sf::Vector2f(100.f, 100.f)), ZoneType::Commercial, 0.8f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(650.f, 320.f), sf::Vector2f(150.f, 120.f)), ZoneType::Commercial, 0.85f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(950.f, 250.f), sf::Vector2f(200.f, 150.f)), ZoneType::Commercial, 0.75f });
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(650.f, 750.f), sf::Vector2f(200.f, 100.f)), ZoneType::Commercial, 0.8f });

    // Public zones - schools, hospitals, parks
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(250.f, 440.f), sf::Vector2f(120.f, 100.f)), ZoneType::Public, 0.3f }); // School
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(750.f, 150.f), sf::Vector2f(100.f, 120.f)), ZoneType::Public, 0.3f }); // Hospital
    m_zones.push_back({ sf::FloatRect(sf::Vector2f(450.f, 320.f), sf::Vector2f(150.f, 150.f)), ZoneType::Public, 0.2f }); // Park
}

void EnvironmentManager::populate_residential_zone(const Zone& zone)
{
    auto& rng = RNG::instance();
    const sf::FloatRect& bounds = zone.bounds;

    // Calculate number of buildings based on density
    int num_buildings = static_cast<int>(
        (bounds.size.x * bounds.size.y) / 5000.f * zone.density
    );

    // 1) Organic scatter near roads
    for (int i = 0; i < num_buildings; ++i)
    {
        float x = rng.getFloat(bounds.position.x + 30, bounds.position.x + bounds.size.x - 30);
        float y = rng.getFloat(bounds.position.y + 30, bounds.position.y + bounds.size.y - 30);
        sf::Vector2f position(x, y);
        if (!is_position_clear(position, 40.f))
            continue;

        BuildingType type = rng.getFloat(0.f, 1.f) < 0.7f ? BuildingType::Residential_Small
                                                           : BuildingType::Residential_Medium;
        sf::Vector2f size = (type == BuildingType::Residential_Small)
            ? sf::Vector2f(rng.getFloat(30.f, 50.f), rng.getFloat(26.f, 46.f))
            : sf::Vector2f(rng.getFloat(50.f, 90.f), rng.getFloat(50.f, 90.f));

        float road_angle = get_road_angle(position);
        float rotation = (rng.getFloat(0.f, 1.f) < 0.7f) ? road_angle : (rng.getFloat(0.f, 1.f) < 0.5f ? 0.f : 90.f);
        const float rad = road_angle * 3.14159265358979323846f / 180.f;
        sf::Vector2f n(-std::sin(rad), std::cos(rad)); // left normal
        position += n * rng.getFloat(18.f, 36.f);
        place_building(position, size, type, rotation);

        if (rng.getFloat(0.f, 1.f) < 0.5f)
        {
            sf::Vector2f tree_pos(position.x + rng.getFloat(-25.f, 25.f), position.y + rng.getFloat(-25.f, 25.f));
            if (is_position_clear(tree_pos, 10.f)) place_urban_element(tree_pos, UrbanElementType::Tree);
        }
        if (rng.getFloat(0.f, 1.f) < 0.15f)
        {
            sf::Vector2f bench_pos(position.x + rng.getFloat(-20.f, 20.f), position.y + rng.getFloat(-20.f, 20.f));
            if (is_position_clear(bench_pos, 8.f)) place_urban_element(bench_pos, UrbanElementType::Bench);
        }
    }

    // 2) Organized rows (block-style streets)
    const bool horizontal_rows = bounds.size.x >= bounds.size.y;
    int rows = horizontal_rows ? 3 : 4; // a few rows to look organized
    float margin = 24.f;
    for (int r = 0; r < rows; ++r)
    {
        float t = (rows == 1) ? 0.5f : static_cast<float>(r) / (rows - 1);
        float y = bounds.position.y + margin + t * (bounds.size.y - 2 * margin);
        float x = bounds.position.x + margin;
        float x_end = bounds.position.x + bounds.size.x - margin;
        float spacing = 70.f; // house spacing
        for (float cx = x; cx <= x_end; cx += spacing)
        {
            sf::Vector2f pos(horizontal_rows ? sf::Vector2f(cx, y)
                                             : sf::Vector2f(y, cx));
            if (!is_position_clear(pos, 35.f)) continue;

            // Alternate small/medium
            bool small = ((static_cast<int>(cx + y) / static_cast<int>(spacing)) % 2) == 0;
            BuildingType type = small ? BuildingType::Residential_Small : BuildingType::Residential_Medium;
            sf::Vector2f size = small ? sf::Vector2f(42.f, 36.f) : sf::Vector2f(70.f, 70.f);

            float ang = get_road_angle(pos);
            const float rad = ang * 3.14159265358979323846f / 180.f;
            sf::Vector2f n(-std::sin(rad), std::cos(rad));
            sf::Vector2f fpos = pos + n * 22.f;
            if (is_position_clear(fpos, 30.f))
                place_building(fpos, size, type, ang);

            // Street tree in front sometimes
            if (rng.getFloat(0.f, 1.f) < 0.4f)
            {
                sf::Vector2f tpos = fpos - n * 16.f;
                if (is_position_clear(tpos, 10.f)) place_urban_element(tpos, UrbanElementType::Tree);
            }
        }
    }

    // 3) Add pocket parks in residential zones
    int num_pockets = std::max(1, static_cast<int>(num_buildings * 0.06f));
    for (int i = 0; i < num_pockets; ++i)
    {
        float x = rng.getFloat(bounds.position.x + 30, bounds.position.x + bounds.size.x - 30);
        float y = rng.getFloat(bounds.position.y + 30, bounds.position.y + bounds.size.y - 30);
        sf::Vector2f center(x, y);
        if (!is_position_clear(center, 35.f)) continue;

        place_urban_element(center, UrbanElementType::GrassPatch);
        if (rng.getFloat(0.f, 1.f) < 0.4f) place_urban_element(center, UrbanElementType::Fountain);
        for (int b = 0; b < 3; ++b)
        {
            float ang = rng.getFloat(0.f, 6.28318f);
            sf::Vector2f bp(center.x + std::cos(ang) * 25.f, center.y + std::sin(ang) * 18.f);
            if (is_position_clear(bp, 6.f)) place_urban_element(bp, UrbanElementType::Bench);
        }
        for (int t2 = 0; t2 < 6; ++t2)
        {
            float ang = t2 * 6.28318f / 6.f;
            sf::Vector2f tp(center.x + std::cos(ang) * 35.f, center.y + std::sin(ang) * 28.f);
            if (is_position_clear(tp, 8.f)) place_urban_element(tp, UrbanElementType::Tree);
        }
    }
}

void EnvironmentManager::populate_commercial_zone(const Zone& zone)
{
    auto& rng = RNG::instance();
    const sf::FloatRect& bounds = zone.bounds;

    // Fewer but larger buildings
    int num_buildings = static_cast<int>(
        (bounds.size.x * bounds.size.y) / 8000.f * zone.density
    );

    // 1) Organic frontage
    for (int i = 0; i < num_buildings; ++i)
    {
        float x = rng.getFloat(bounds.position.x + 40, bounds.position.x + bounds.size.x - 40);
        float y = rng.getFloat(bounds.position.y + 40, bounds.position.y + bounds.size.y - 40);
        sf::Vector2f position(x, y);

        if (!is_position_clear(position, 50.f)) continue;

        BuildingType type = rng.getFloat(0.f, 1.f) < 0.6f ? BuildingType::Commercial_Small
                                                           : BuildingType::Commercial_Large;
        sf::Vector2f size = (type == BuildingType::Commercial_Small)
            ? sf::Vector2f(rng.getFloat(40.f, 70.f), rng.getFloat(35.f, 60.f))
            : sf::Vector2f(rng.getFloat(90.f, 140.f), rng.getFloat(90.f, 140.f));

        float angle = get_road_angle(position);
        const float rad = angle * 3.14159265358979323846f / 180.f;
        sf::Vector2f n(-std::sin(rad), std::cos(rad));
        position += n * rng.getFloat(24.f, 40.f);
        place_building(position, size, type, angle);
    }

    // 2) Organized retail rows along the long side
    bool along_x = bounds.size.x >= bounds.size.y;
    int bands = along_x ? 2 : 3;
    float margin = 28.f;
    for (int b = 0; b < bands; ++b)
    {
        float t = (bands == 1) ? 0.5f : static_cast<float>(b) / (bands - 1);
        float y = bounds.position.y + margin + t * (bounds.size.y - 2 * margin);
        float x0 = bounds.position.x + margin;
        float x1 = bounds.position.x + bounds.size.x - margin;
        float spacing = 110.f; // shop/mid-size spacing
        for (float cx = x0; cx <= x1; cx += spacing)
        {
            sf::Vector2f pos(along_x ? sf::Vector2f(cx, y)
                                     : sf::Vector2f(y, cx));
            if (!is_position_clear(pos, 45.f)) continue;

            bool small = (static_cast<int>(cx + y) / static_cast<int>(spacing)) % 3 != 0;
            BuildingType type = small ? BuildingType::Commercial_Small : BuildingType::Commercial_Large;
            sf::Vector2f size = small ? sf::Vector2f(60.f, 48.f) : sf::Vector2f(120.f, 110.f);

            float ang = get_road_angle(pos);
            const float rad = ang * 3.14159265358979323846f / 180.f;
            sf::Vector2f n(-std::sin(rad), std::cos(rad));
            sf::Vector2f fpos = pos + n * 28.f;
            if (is_position_clear(fpos, 40.f))
                place_building(fpos, size, type, ang);

            // Street furniture
            if (rng.getFloat(0.f, 1.f) < 0.3f)
            {
                sf::Vector2f bs = fpos - n * 16.f;
                if (is_position_clear(bs, 8.f)) place_urban_element(bs, UrbanElementType::Bench);
            }
        }
    }

    // Add parking lots near commercial areas
    if (bounds.size.x > 100 && bounds.size.y > 100)
    {
        sf::Vector2f parking_pos(
            bounds.position.x + bounds.size.x / 2,
            bounds.position.y + bounds.size.y / 2
        );
        if (is_position_clear(parking_pos, 40.f))
            place_urban_element(parking_pos, UrbanElementType::ParkingLot);
    }

    // Add a couple of bus stops along edges
    for (int k = 0; k < 2; ++k)
    {
        sf::Vector2f bus_stop_pos(
            bounds.position.x + (k == 0 ? rng.getFloat(20.f, 60.f) : bounds.size.x - rng.getFloat(20.f, 60.f)),
            bounds.position.y + rng.getFloat(20.f, 60.f)
        );
        if (is_position_clear(bus_stop_pos, 15.f))
            place_urban_element(bus_stop_pos, UrbanElementType::BusStop);
        if (rng.getFloat(0.f, 1.f) < 0.4f)
        {
            sf::Vector2f bench_pos(bus_stop_pos.x + rng.getFloat(-12.f, 12.f), bus_stop_pos.y + rng.getFloat(-8.f, 8.f));
            if (is_position_clear(bench_pos, 8.f)) place_urban_element(bench_pos, UrbanElementType::Bench);
        }
    }
}

void EnvironmentManager::populate_public_zone(const Zone& zone)
{
    const sf::FloatRect& bounds = zone.bounds;
    sf::Vector2f center(
        bounds.position.x + bounds.size.x / 2,
        bounds.position.y + bounds.size.y / 2
    );

    // Determine building type based on zone index
    static int public_building_counter = 0;
    BuildingType type = BuildingType::Public_Park;
    sf::Vector2f size;

    switch (public_building_counter % 3)
    {
    case 0: // School
        type = BuildingType::Public_School;
        size = sf::Vector2f(100.f, 80.f);
        break;
    case 1: // Hospital
        type = BuildingType::Public_Hospital;
        size = sf::Vector2f(90.f, 90.f);
        break;
    case 2: // Park
        type = BuildingType::Public_Park;
        size = sf::Vector2f(120.f, 100.f);

        // Add fountain in park
        place_urban_element(center, UrbanElementType::Fountain);

        // Add benches
        for (int i = 0; i < 4; ++i)
        {
            float angle = i * std::numbers::pi_v<float> / 2.f;
            sf::Vector2f bench_pos(
                center.x + std::cos(angle) * 40.f,
                center.y + std::sin(angle) * 40.f
            );
            place_urban_element(bench_pos, UrbanElementType::Bench);
        }

        // Add trees around park
        for (int i = 0; i < 8; ++i)
        {
            float angle = i * std::numbers::pi_v<float> / 4.f;
            sf::Vector2f tree_pos(
                center.x + std::cos(angle) * 55.f,
                center.y + std::sin(angle) * 45.f
            );
            place_urban_element(tree_pos, UrbanElementType::Tree);
        }
        break;
    }

    public_building_counter++;

    if (type != BuildingType::Public_Park)
    {
        place_building(center, size, type, 0.f);

        // Add some trees around public buildings
        auto& rng = RNG::instance();
        for (int i = 0; i < 6; ++i)
        {
            float angle = rng.getFloat(0.f, 2.f * std::numbers::pi_v<float>);
            sf::Vector2f tree_pos(
                center.x + std::cos(angle) * 70.f,
                center.y + std::sin(angle) * 65.f
            );
            if (is_position_clear(tree_pos, 10.f))
                place_urban_element(tree_pos, UrbanElementType::Tree);
        }
    }
}

void EnvironmentManager::add_street_infrastructure()
{
    auto& rng = RNG::instance();

    // Add street lamps along major roads at regular intervals
    // Main horizontal roads
    for (float x = 100.f; x <= 1700.f; x += 80.f)
    {
        for (float y : { 100.f, 300.f, 375.f, 500.f, 700.f, 800.f })
        {
            sf::Vector2f lamp_pos(x + rng.getFloat(-10.f, 10.f), y + 25.f);
            if (is_position_clear(lamp_pos, 20.f))
                place_urban_element(lamp_pos, UrbanElementType::StreetLamp);
        }
    }

    // Main vertical roads
    for (float y = 50.f; y <= 1550.f; y += 80.f)
    {
        for (float x : { 100.f, 200.f, 300.f, 700.f, 900.f, 1000.f })
        {
            sf::Vector2f lamp_pos(x + 25.f, y + rng.getFloat(-10.f, 10.f));
            if (is_position_clear(lamp_pos, 20.f))
                place_urban_element(lamp_pos, UrbanElementType::StreetLamp);
        }
    }
}

void EnvironmentManager::add_parks_and_greenery()
{
    auto& rng = RNG::instance();

    // Add decorative trees along roads
    for (float x = 120.f; x <= 1650.f; x += 50.f)
    {
        for (float y : { 120.f, 280.f, 355.f, 480.f, 680.f })
        {
            sf::Vector2f tree_pos(x + rng.getFloat(-6.f, 6.f), y + rng.getFloat(-6.f, 6.f));
            if (is_position_clear(tree_pos, 15.f) && rng.getFloat(0.f, 1.f) < 0.6f)
                place_urban_element(tree_pos, UrbanElementType::Tree);
        }
    }

    // Create 3-5 feature parks with fountain, benches, trees
    int parks = 3 + static_cast<int>(rng.getFloat(0.f, 2.f));
    for (int p = 0; p < parks; ++p)
    {
        float cx = rng.getFloat(200.f, 1600.f);
        float cy = rng.getFloat(200.f, 1400.f);
        sf::Vector2f center(cx, cy);
        if (!is_position_clear(center, 60.f)) continue;

        place_urban_element(center, UrbanElementType::GrassPatch);
        place_urban_element(center, UrbanElementType::Fountain);
        for (int i = 0; i < 4; ++i)
        {
            float ang = i * 3.14159265f / 2.f;
            sf::Vector2f bp(center.x + std::cos(ang) * 48.f, center.y + std::sin(ang) * 36.f);
            if (is_position_clear(bp, 8.f)) place_urban_element(bp, UrbanElementType::Bench);
        }
        for (int i = 0; i < 12; ++i)
        {
            float ang = i * 6.28318f / 12.f;
            sf::Vector2f tp(center.x + std::cos(ang) * 75.f, center.y + std::sin(ang) * 55.f);
            if (is_position_clear(tp, 10.f)) place_urban_element(tp, UrbanElementType::Tree);
        }
    }
}

void EnvironmentManager::place_building(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    BuildingType type,
    float rotation
)
{
    m_buildings.emplace_back(position, size, type, rotation);
}

void EnvironmentManager::place_urban_element(const sf::Vector2f& position, UrbanElementType type)
{
    m_urban_elements.emplace_back(position, type);
}

bool EnvironmentManager::is_position_clear(const sf::Vector2f& position, float radius) const
{
    // Check against existing buildings
    for (const auto& building : m_buildings)
    {
        sf::Vector2f diff = position - building.get_position();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        sf::Vector2f size = building.get_size();
        float building_radius = std::max(size.x, size.y) / 2.f;

        if (distance < radius + building_radius + 15.f) // Add spacing
            return false;
    }

    // Check against large urban elements
    for (const auto& element : m_urban_elements)
    {
        if (element.get_type() == UrbanElementType::ParkingLot
            || element.get_type() == UrbanElementType::Fountain
            || element.get_type() == UrbanElementType::GrassPatch)
        {
            sf::Vector2f diff = position - element.get_position();
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (distance < radius + 30.f)
                return false;
        }
    }

    return true;
}

float EnvironmentManager::get_road_angle(const sf::Vector2f& position) const
{
    // Find nearest road segment and return its angle in degrees
    float best_dist2 = std::numeric_limits<float>::max();
    float best_angle = 0.f;

    for (const auto& seg : m_road_segments)
    {
        const sf::Vector2f a = seg.first;
        const sf::Vector2f b = seg.second;
        const sf::Vector2f ab = b - a;
        const float ab2 = ab.x * ab.x + ab.y * ab.y + 1e-5f;
        const float t = std::clamp(((position - a).x * ab.x + (position - a).y * ab.y) / ab2, 0.f, 1.f);
        const sf::Vector2f proj = a + t * ab;
        const sf::Vector2f diff = position - proj;
        const float d2 = diff.x * diff.x + diff.y * diff.y;
        if (d2 < best_dist2)
        {
            best_dist2 = d2;
            best_angle = std::atan2(ab.y, ab.x) * 180.f / 3.14159265358979323846f;
        }
    }
    return best_angle;
}


void EnvironmentManager::draw_environment(sf::RenderWindow& window) const
{
    // Draw in order: grass patches, buildings, trees, other elements
    for (const auto& element : m_urban_elements)
    {
        if (element.get_type() == UrbanElementType::GrassPatch)
            element.draw(window);
    }

    for (const auto& building : m_buildings)
    {
        building.draw(window);
    }

    for (const auto& element : m_urban_elements)
    {
        if (element.get_type() != UrbanElementType::GrassPatch)
            element.draw(window);
    }
}
