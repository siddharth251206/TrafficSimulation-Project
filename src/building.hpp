#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>

enum class ZoneType
{
    Residential,
    Commercial,
    PublicService,
    Park,
    Decorative
};

enum class BuildingType
{
    // Residential
    SmallHouse,
    Apartment,
    
    // Commercial
    OfficeBuilding,
    Shop,
    
    // Public Services
    Hospital,
    School,
    PoliceStation,
    
    // Decorative
    Tree,
    LampPost,
    Park
};

class Building
{
public:
    Building(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        BuildingType type,
        ZoneType zone,
        const sf::Texture* texture = nullptr
    );

    void draw(sf::RenderWindow& window) const;
    
    const sf::Vector2f& get_position() const { return m_position; }
    const sf::Vector2f& get_size() const { return m_size; }
    BuildingType get_type() const { return m_type; }
    ZoneType get_zone() const { return m_zone; }
    sf::FloatRect get_bounds() const;

private:
    // data
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    BuildingType m_type;
    ZoneType m_zone;
    
    // Visual representation - either sprite or colored rectangle with details
    std::optional<sf::Sprite> m_sprite;
    sf::RectangleShape m_shape;

    // drawing helpers for fallback (no texture)
    void draw_fallback(sf::RenderWindow& window) const;
    void draw_windows(sf::RenderWindow& window, int cols, int rows, float margin_ratio) const;
    void draw_roof(sf::RenderWindow& window) const; // for SmallHouse
    void draw_ground_details(sf::RenderWindow& window) const; // door, shop sign, etc.

    // Helper to get appropriate color for building type
    static sf::Color get_building_color(BuildingType type);
};
