#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

enum class BuildingType
{
    Residential_Small,   // 1-2 storey houses
    Residential_Medium,  // Apartment blocks
    Commercial_Small,    // Shops
    Commercial_Large,    // Malls, office towers
    Public_School,
    Public_Hospital,
    Public_Park,
    Utility
};

enum class ZoneType
{
    Residential,
    Commercial,
    Public,
    Mixed
};

class Building
{
public:
    Building(
        const sf::Vector2f& position,
        const sf::Vector2f& size,
        BuildingType type,
        float rotation = 0.f
    );

    void draw(sf::RenderWindow& window) const;
    const sf::Vector2f& get_position() const { return m_position; }
    const sf::Vector2f& get_size() const { return m_size; }
    BuildingType get_type() const { return m_type; }

private:
    sf::Vector2f m_position;
    sf::Vector2f m_size;
    BuildingType m_type;
    float m_rotation;

    // Base body
    sf::RectangleShape m_shape;
    sf::Color m_color;

    // Decorative/components
    std::vector<sf::RectangleShape> m_windows;
    std::vector<sf::RectangleShape> m_facade_bands;
    sf::RectangleShape m_door;
    bool m_has_door{false};

    sf::ConvexShape m_roof; // For small houses
    bool m_has_roof{false};

    // Special signage (e.g., hospital cross, shop sign)
    std::vector<sf::RectangleShape> m_signage;

    // Optional sprite-based rendering
    bool m_use_sprite{false};
    std::unique_ptr<sf::Sprite> m_sprite;
    std::shared_ptr<sf::Texture> m_texture; // cached texture pointer

    void setup_appearance();
    void add_window_grid(float margin_x, float margin_y, float cell_w, float cell_h,
                         float gap_x, float gap_y, int rows_limit = -1, int cols_limit = -1,
                         float bottom_reserved = 0.f);
    void add_storefront(float height_ratio = 0.3f);
    void add_roof_triangle(float overhang = 8.f, float height = 16.f, sf::Color color = sf::Color(150, 60, 60));
    void add_parapet(float thickness = 4.f, sf::Color color = sf::Color(50, 50, 50));
    void add_hospital_cross(float size_ratio = 0.4f, sf::Color color = sf::Color(200, 40, 40));
    void add_school_banner();

    // New compact helpers to reduce duplication
    void make_door(float w_ratio, float h_ratio, sf::Color fill, sf::Color outline, float front_margin = 2.f);
    void add_banner(float width_ratio, float height_pixels, sf::Color fill, sf::Color outline, float y_from_front);
    void try_set_sprite(const char* relPath, const char* label);
    void add_horizontal_bands(int count, float thickness, sf::Color color);
    void add_utility_vents(float offset_y_ratio = -0.35f);
};
