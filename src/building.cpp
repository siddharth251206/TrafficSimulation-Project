#include "building.hpp"

Building::Building(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    BuildingType type,
    ZoneType zone,
    const sf::Texture* texture
)
    : m_position(position)
    , m_size(size)
    , m_type(type)
    , m_zone(zone)
{
    if (texture)
    {
        // Use sprite if texture provided
        m_sprite = sf::Sprite(*texture);
        m_sprite->setPosition(position);
        
        // Scale sprite to fit the desired size
        sf::Vector2u tex_size = texture->getSize();
        m_sprite->setScale(sf::Vector2f(
            size.x / static_cast<float>(tex_size.x),
            size.y / static_cast<float>(tex_size.y)
        ));
    }
    else
    {
        // Use colored rectangle as fallback base
        m_shape.setPosition(position);
        m_shape.setSize(size);
        m_shape.setFillColor(get_building_color(type));
        m_shape.setOutlineColor(sf::Color(40, 40, 40));
        m_shape.setOutlineThickness(1.f);
    }
}

void Building::draw(sf::RenderWindow& window) const
{
    if (m_sprite)
    {
        window.draw(*m_sprite);
        return;
    }

    // Fallback: richer primitive-based rendering per type
    window.draw(m_shape);
    draw_fallback(window);
}

sf::FloatRect Building::get_bounds() const
{
    return sf::FloatRect(m_position, m_size);
}

void Building::draw_fallback(sf::RenderWindow& window) const
{
    switch (m_type)
    {
    case BuildingType::SmallHouse:
        draw_roof(window);
        draw_windows(window, 2, 2, 0.14f);
        draw_ground_details(window);
        break;
    case BuildingType::Apartment:
        draw_windows(window, 3, 4, 0.10f);
        draw_ground_details(window);
        break;
    case BuildingType::OfficeBuilding:
        draw_windows(window, 4, 5, 0.08f);
        break;
    case BuildingType::Shop:
        draw_windows(window, 3, 2, 0.10f);
        draw_ground_details(window);
        break;
    case BuildingType::Hospital:
    case BuildingType::School:
    case BuildingType::PoliceStation:
        draw_windows(window, 3, 3, 0.12f);
        draw_ground_details(window);
        break;
    case BuildingType::Tree:
    {
        // simple stylized tree: trunk + crown
        sf::RectangleShape trunk({ m_size.x * 0.12f, m_size.y * 0.35f });
        trunk.setFillColor(sf::Color(90, 60, 40));
        trunk.setPosition({ m_position.x + m_size.x * 0.44f, m_position.y + m_size.y * 0.65f });
        window.draw(trunk);

        sf::CircleShape crown(m_size.x * 0.45f);
        crown.setFillColor(sf::Color(70, 160, 70));
        crown.setPosition({ m_position.x + m_size.x * 0.05f, m_position.y + m_size.y * 0.15f });
        window.draw(crown);
        break;
    }
    case BuildingType::LampPost:
    {
        sf::RectangleShape pole({ m_size.x * 0.08f, m_size.y * 0.9f });
        pole.setFillColor(sf::Color(60, 60, 60));
        pole.setPosition({ m_position.x + m_size.x * 0.46f, m_position.y + m_size.y * 0.1f });
        window.draw(pole);

        sf::CircleShape lamp(m_size.x * 0.12f);
        lamp.setFillColor(sf::Color(255, 245, 180));
        lamp.setPosition({ m_position.x + m_size.x * 0.40f, m_position.y + m_size.y * 0.02f });
        window.draw(lamp);
        break;
    }
    case BuildingType::Park:
    {
        // grass pattern
        sf::RectangleShape stripe({ m_size.x, m_size.y * 0.06f });
        stripe.setFillColor(sf::Color(70, 170, 70));
        for (int i = 0; i < 6; ++i)
        {
            stripe.setPosition({ m_position.x, m_position.y + i * stripe.getSize().y });
            window.draw(stripe);
        }
        break;
    }
    default:
        break;
    }

    // subtle side shading for depth
    sf::RectangleShape shade({ m_size.x * 0.06f, m_size.y });
    shade.setFillColor(sf::Color(0, 0, 0, 25));
    shade.setPosition({ m_position.x + m_size.x - shade.getSize().x, m_position.y });
    window.draw(shade);
}

void Building::draw_windows(sf::RenderWindow& window, int cols, int rows, float margin_ratio) const
{
    const float margin = m_size.x * margin_ratio;
    const float usable_w = m_size.x - 2.f * margin;
    const float usable_h = m_size.y - 2.f * margin;

    if (cols <= 0 || rows <= 0 || usable_w <= 0.f || usable_h <= 0.f)
        return;

    const float cell_w = usable_w / static_cast<float>(cols);
    const float cell_h = usable_h / static_cast<float>(rows);

    sf::RectangleShape win({ cell_w * 0.6f, cell_h * 0.5f });
    win.setFillColor(sf::Color(230, 235, 255));
    win.setOutlineColor(sf::Color(50, 50, 70));
    win.setOutlineThickness(1.f);

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            const float x = m_position.x + margin + c * cell_w + (cell_w - win.getSize().x) * 0.5f;
            const float y = m_position.y + margin + r * cell_h + (cell_h - win.getSize().y) * 0.5f;
            win.setPosition({ x, y });
            window.draw(win);
        }
    }
}

void Building::draw_roof(sf::RenderWindow& window) const
{
    sf::ConvexShape roof;
    roof.setPointCount(3);
    roof.setPoint(0, { m_position.x, m_position.y + m_size.y * 0.25f });
    roof.setPoint(1, { m_position.x + m_size.x * 0.5f, m_position.y });
    roof.setPoint(2, { m_position.x + m_size.x, m_position.y + m_size.y * 0.25f });
    roof.setFillColor(sf::Color(150, 60, 50));
    roof.setOutlineColor(sf::Color(40, 20, 20));
    roof.setOutlineThickness(1.f);
    window.draw(roof);
}

void Building::draw_ground_details(sf::RenderWindow& window) const
{
    // Door centered at bottom
    sf::RectangleShape door({ m_size.x * 0.18f, m_size.y * 0.28f });
    door.setFillColor(sf::Color(90, 70, 50));
    door.setOutlineColor(sf::Color(40, 30, 25));
    door.setOutlineThickness(1.f);
    door.setPosition({ m_position.x + (m_size.x - door.getSize().x) * 0.5f,
                       m_position.y + m_size.y - door.getSize().y - 1.f });
    window.draw(door);
}

sf::Color Building::get_building_color(BuildingType type)
{
    switch (type)
    {
    // Residential - warm colors
    case BuildingType::SmallHouse:
        return sf::Color(200, 150, 100);  // Beige
    case BuildingType::Apartment:
        return sf::Color(180, 140, 120);  // Light brown
    
    // Commercial - cool/modern colors
    case BuildingType::OfficeBuilding:
        return sf::Color(120, 140, 160);  // Steel blue
    case BuildingType::Shop:
        return sf::Color(220, 180, 100);  // Golden
    
    // Public Services - distinct colors
    case BuildingType::Hospital:
        return sf::Color(255, 100, 100);  // Red
    case BuildingType::School:
        return sf::Color(255, 220, 100);  // Yellow
    case BuildingType::PoliceStation:
        return sf::Color(100, 150, 255);  // Blue
    
    // Decorative - natural colors
    case BuildingType::Tree:
        return sf::Color(50, 150, 50);    // Green
    case BuildingType::LampPost:
        return sf::Color(180, 180, 180);  // Gray
    case BuildingType::Park:
        return sf::Color(80, 180, 80);    // Grass green
    
    default:
        return sf::Color(150, 150, 150);  // Gray fallback
    }
}
