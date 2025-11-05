#include "building.hpp"
#include "app_utility.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <string>

namespace {
    inline float deg2rad(float deg) { return deg * 3.14159265358979323846f / 180.f; }
    inline sf::Vector2f rotate_offset(const sf::Vector2f& v, float degrees)
    {
        const float r = deg2rad(degrees);
        const float c = std::cos(r);
        const float s = std::sin(r);
        return { v.x * c - v.y * s, v.x * s + v.y * c };
    }

    std::shared_ptr<sf::Texture> load_cached_texture(const std::string& relPath, const char* label)
    {
        static std::unordered_map<std::string, std::weak_ptr<sf::Texture>> cache;
        if (auto it = cache.find(relPath); it != cache.end())
        {
            if (auto sp = it->second.lock()) return sp;
        }
        auto tex = std::make_shared<sf::Texture>();
        if (AssetHelper::try_load_texture(*tex, relPath, label))
        {
            tex->setSmooth(true);
            cache[relPath] = tex;
            return tex;
        }
        return nullptr;
    }

    struct Style { sf::Color outline; float outlineThick; std::vector<sf::Color> fills; const char* sprite; const char* label; };
    inline Style style_for(BuildingType t)
    {
        switch (t)
        {
        case BuildingType::Residential_Small:
            return { {120,110,90},  -2.f, { {210,180,150},{200,170,120},{190,160,140},{170,190,170},{180,200,210},{220,190,190} }, "assets/buildings/residential_small.png", "residential_small" };
        case BuildingType::Residential_Medium:
            return { {120,110,100},-2.f, { {185,175,165},{170,170,190},{195,185,175},{165,180,190} }, "assets/buildings/residential_medium.png", "residential_medium" };
        case BuildingType::Commercial_Small:
            return { {160,140,120},-2.f, { {230,200,180},{220,210,160},{210,190,200},{200,180,220} }, "assets/buildings/commercial_small.png", "commercial_small" };
        case BuildingType::Commercial_Large:
            return { {90,110,130}, -3.f, { {140,160,180} }, "assets/buildings/commercial_large.png", "commercial_large" };
        case BuildingType::Public_School:
            return { {180,150,100},-2.f, { {220,200,160} }, "assets/buildings/public_school.png", "school" };
        case BuildingType::Public_Hospital:
            return { {180,50,50},   -3.f, { {220,220,230} }, "assets/buildings/public_hospital.png", "hospital" };
        case BuildingType::Public_Park:
            return { {60,120,60},   -2.f, { {100,180,100} }, nullptr, "park" };
        case BuildingType::Utility:
            return { {100,100,100}, -1.f, { {160,160,160} }, "assets/buildings/utility.png", "utility" };
        }
        return { {100,100,100}, -1.f, { {180,180,180} }, nullptr, "unknown" };
    }
}

Building::Building(
    const sf::Vector2f& position,
    const sf::Vector2f& size,
    BuildingType type,
    float rotation
)
    : m_position(position)
    , m_size(size)
    , m_type(type)
    , m_rotation(rotation)
{
    m_shape.setSize(size);
    m_shape.setPosition(position);
    m_shape.setRotation(sf::degrees(rotation));
    m_shape.setOrigin(sf::Vector2f(size.x / 2.f, size.y / 2.f));
    setup_appearance();
}

void Building::add_window_grid(float margin_x, float margin_y, float cell_w, float cell_h,
                               float gap_x, float gap_y, int rows_limit, int cols_limit,
                               float bottom_reserved)
{
    const float avail_w = std::max(0.f, m_size.x - 2.f * margin_x);
    const float avail_h = std::max(0.f, m_size.y - (margin_y + bottom_reserved) - margin_y);

    int cols = std::max(1, static_cast<int>(std::floor((avail_w + gap_x) / (cell_w + gap_x))));
    int rows = std::max(1, static_cast<int>(std::floor((avail_h + gap_y) / (cell_h + gap_y))));
    if (cols_limit > 0) cols = std::min(cols, cols_limit);
    if (rows_limit > 0) rows = std::min(rows, rows_limit);

    const sf::Color win_fill(205, 220, 240);
    const sf::Color win_outline(80, 90, 100);

    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
        {
            sf::RectangleShape rect;
            rect.setSize({ cell_w, cell_h });
            rect.setOrigin({ cell_w / 2.f, cell_h / 2.f });
            rect.setFillColor(win_fill);
            rect.setOutlineColor(win_outline);
            rect.setOutlineThickness(-1.f);
            rect.setRotation(sf::degrees(m_rotation));
            const float lx = -m_size.x / 2.f + margin_x + cell_w / 2.f + c * (cell_w + gap_x);
            const float ly = -m_size.y / 2.f + margin_y + cell_h / 2.f + r * (cell_h + gap_y);
            rect.setPosition(m_position + rotate_offset({ lx, ly }, m_rotation));
            m_windows.push_back(rect);
        }
}

void Building::add_storefront(float height_ratio)
{
    const float h = std::max(10.f, m_size.y * height_ratio);
    sf::RectangleShape glass;
    glass.setSize({ m_size.x - 8.f, h - 4.f });
    glass.setOrigin({ glass.getSize().x / 2.f, glass.getSize().y / 2.f });
    glass.setFillColor(sf::Color(170, 200, 220));
    glass.setOutlineColor(sf::Color(70, 90, 110));
    glass.setOutlineThickness(-2.f);
    glass.setRotation(sf::degrees(m_rotation));
    glass.setPosition(m_position + rotate_offset({ 0.f, m_size.y / 2.f - h / 2.f - 2.f }, m_rotation));
    m_facade_bands.push_back(glass);
}

void Building::add_roof_triangle(float overhang, float height, sf::Color color)
{
    m_roof = sf::ConvexShape(3);
    m_roof.setPoint(0, { -m_size.x / 2.f - overhang, -m_size.y / 2.f });
    m_roof.setPoint(1, {  m_size.x / 2.f + overhang, -m_size.y / 2.f });
    m_roof.setPoint(2, {  0.f, -m_size.y / 2.f - height });
    m_roof.setFillColor(color);
    m_roof.setOutlineColor(sf::Color(100, 40, 40));
    m_roof.setOutlineThickness(-2.f);
    m_roof.setPosition(m_position);
    m_roof.setRotation(sf::degrees(m_rotation));
    m_has_roof = true;
}

void Building::add_parapet(float thickness, sf::Color color)
{
    sf::RectangleShape band;
    band.setSize({ m_size.x, thickness });
    band.setOrigin({ band.getSize().x / 2.f, band.getSize().y / 2.f });
    band.setFillColor(color);
    band.setOutlineColor(sf::Color(30, 30, 30));
    band.setOutlineThickness(-1.f);
    band.setRotation(sf::degrees(m_rotation));
    band.setPosition(m_position + rotate_offset({ 0.f, -m_size.y / 2.f + thickness / 2.f }, m_rotation));
    m_facade_bands.push_back(band);
}

void Building::add_hospital_cross(float size_ratio, sf::Color color)
{
    const float size = std::min(m_size.x, m_size.y) * size_ratio;
    const float bar = std::max(6.f, size / 3.f);
    for (int i = 0; i < 2; ++i)
    {
        sf::RectangleShape rect;
        rect.setSize(i == 0 ? sf::Vector2f(size, bar) : sf::Vector2f(bar, size));
        rect.setOrigin({ rect.getSize().x / 2.f, rect.getSize().y / 2.f });
        rect.setFillColor(color);
        rect.setOutlineColor(sf::Color(120, 20, 20));
        rect.setOutlineThickness(-1.f);
        rect.setRotation(sf::degrees(m_rotation));
        rect.setPosition(m_position + rotate_offset({ 0.f, -m_size.y * 0.1f }, m_rotation));
        m_signage.push_back(rect);
    }
}

void Building::add_school_banner()
{
    add_banner(0.6f, std::max(8.f, m_size.y * 0.08f), sf::Color(210, 180, 60), sf::Color(120, 100, 40), m_size.y * 0.5f - std::max(8.f, m_size.y * 0.08f) * 1.5f);
}

void Building::try_set_sprite(const char* relPath, const char* label)
{
    if (!relPath) return;
    if (auto tex = load_cached_texture(relPath, label))
    {
        m_texture = tex;
        m_sprite = std::make_unique<sf::Sprite>(*m_texture);
        const auto tsize = m_texture->getSize();
        if (tsize.x > 0 && tsize.y > 0)
        {
            const float sx = m_size.x / static_cast<float>(tsize.x);
            const float sy = m_size.y / static_cast<float>(tsize.y);
            m_sprite->setScale({ sx, sy });
        }
        m_sprite->setOrigin({ m_texture->getSize().x / 2.f, m_texture->getSize().y / 2.f });
        m_sprite->setPosition(m_position);
        m_sprite->setRotation(sf::degrees(m_rotation));
        m_use_sprite = true;
    }
}

void Building::make_door(float w_ratio, float h_ratio, sf::Color fill, sf::Color outline, float front_margin)
{
    m_has_door = true;
    const float w = std::max(12.f, m_size.x * w_ratio);
    const float h = std::max(16.f, m_size.y * h_ratio);
    m_door.setSize({ w, h });
    m_door.setOrigin({ w / 2.f, h / 2.f });
    m_door.setFillColor(fill);
    m_door.setOutlineColor(outline);
    m_door.setOutlineThickness(-2.f);
    m_door.setRotation(sf::degrees(m_rotation));
    m_door.setPosition(m_position + rotate_offset({ 0.f, m_size.y / 2.f - h / 2.f - front_margin }, m_rotation));
}

void Building::add_banner(float width_ratio, float height_pixels, sf::Color fill, sf::Color outline, float y_from_front)
{
    sf::RectangleShape banner;
    banner.setSize({ m_size.x * width_ratio, height_pixels });
    banner.setOrigin({ banner.getSize().x / 2.f, banner.getSize().y / 2.f });
    banner.setFillColor(fill);
    banner.setOutlineColor(outline);
    banner.setOutlineThickness(-2.f);
    banner.setRotation(sf::degrees(m_rotation));
    // y_from_front is distance down from the top/front edge
    const float local_y = m_size.y / 2.f - y_from_front - height_pixels / 2.f;
    banner.setPosition(m_position + rotate_offset({ 0.f, local_y }, m_rotation));
    m_signage.push_back(banner);
}

void Building::add_horizontal_bands(int count, float thickness, sf::Color color)
{
    for (int i = 1; i <= count; ++i)
    {
        sf::RectangleShape band;
        band.setSize({ m_size.x, thickness });
        band.setOrigin({ band.getSize().x / 2.f, band.getSize().y / 2.f });
        band.setFillColor(color);
        band.setRotation(sf::degrees(m_rotation));
        const float y = -m_size.y / 2.f + i * (m_size.y / (count + 1.f));
        band.setPosition(m_position + rotate_offset({ 0.f, y }, m_rotation));
        m_facade_bands.push_back(band);
    }
}

void Building::add_utility_vents(float offset_y_ratio)
{
    for (int i = -1; i <= 1; i += 2)
    {
        sf::RectangleShape vent;
        vent.setSize({ 14.f, 6.f });
        vent.setOrigin({ 7.f, 3.f });
        vent.setFillColor(sf::Color(90, 90, 95));
        vent.setRotation(sf::degrees(m_rotation));
        vent.setPosition(m_position + rotate_offset({ i * m_size.x * 0.25f, m_size.y * offset_y_ratio }, m_rotation));
        m_signage.push_back(vent);
    }
}

void Building::setup_appearance()
{
    // reset components
    m_windows.clear();
    m_facade_bands.clear();
    m_signage.clear();
    m_has_door = false;
    m_has_roof = false;

    // Set colors and outline based on building type (table-driven)
    auto& rng = RNG::instance();
    const auto style0 = style_for(m_type);
    m_color = style0.fills.empty() ? sf::Color(180,180,180)
                                   : style0.fills[rng.getIndex(0, style0.fills.size() - 1)];
    m_shape.setOutlineColor(style0.outline);
    m_shape.setOutlineThickness(style0.outlineThick);
    m_shape.setFillColor(m_color);

    // Try sprite from style table
    if (style0.sprite) try_set_sprite(style0.sprite, style0.label);

    // Details per type

    // Details per type
    if (m_type == BuildingType::Residential_Small)
    {
        // Small house: door + a couple of windows + gable roof
        make_door(0.18f, 0.35f, sf::Color(120,80,50), sf::Color(80,60,40));
        add_window_grid(8.f, 10.f, 12.f, 12.f, 10.f, 10.f, 2, 2, m_door.getSize().y + 8.f);
        add_roof_triangle(6.f, std::max(12.f, m_size.y * 0.25f));
    }
    else if (m_type == BuildingType::Residential_Medium)
    {
        // Apartments: grid windows + parapet + door
        add_window_grid(8.f, 8.f, 12.f, 16.f, 8.f, 8.f);
        add_parapet(4.f);
        make_door(0.2f, 0.22f, sf::Color(110,110,120), sf::Color(70,70,80));
    }
    else if (m_type == BuildingType::Commercial_Small)
    {
        // Shop: storefront glass + sign + fewer larger windows
        const float storefront_h = std::max(10.f, m_size.y * 0.35f);
        add_storefront(0.35f);
        add_banner(0.6f, std::max(10.f, m_size.y * 0.12f), sf::Color(240, 90, 70), sf::Color(160, 60, 50), m_size.y / 2.f - storefront_h - std::max(10.f, m_size.y * 0.12f)/2.f - 4.f);
        add_window_grid(10.f, 10.f, 14.f, 14.f, 12.f, 12.f, 2, -1, storefront_h + 10.f);
    }
    else if (m_type == BuildingType::Commercial_Large)
    {
        // Office/mall: dense glass grid + facade bands
        add_window_grid(8.f, 8.f, 12.f, 12.f, 6.f, 6.f);
        add_horizontal_bands(3, 3.f, sf::Color(100, 120, 140));
        add_parapet(5.f, sf::Color(80, 100, 120));
    }
    else if (m_type == BuildingType::Public_School)
    {
        add_window_grid(10.f, 12.f, 14.f, 16.f, 12.f, 12.f, 2, 3, 24.f);
        add_parapet(4.f, sf::Color(160, 140, 100));
        add_school_banner();
        make_door(0.22f, 0.25f, sf::Color(160,120,60), sf::Color(110,90,50));
    }
    else if (m_type == BuildingType::Public_Hospital)
    {
        add_window_grid(8.f, 10.f, 12.f, 16.f, 10.f, 10.f, -1, -1, 26.f);
        add_parapet(4.f, sf::Color(170, 170, 180));
        add_hospital_cross(0.45f);
        make_door(0.22f, 0.25f, sf::Color(200,200,205), sf::Color(120,120,130));
    }
    else if (m_type == BuildingType::Utility)
    {
        add_utility_vents();
        add_parapet(3.f, sf::Color(110, 110, 115));
    }
}

void Building::draw(sf::RenderWindow& window) const
{
    if (m_use_sprite && m_sprite) { window.draw(*m_sprite); return; }
    window.draw(m_shape);
    for (const auto& band : m_facade_bands) window.draw(band);
    for (const auto& win : m_windows) window.draw(win);
    if (m_has_door) window.draw(m_door);
    for (const auto& s : m_signage) window.draw(s);
    if (m_has_roof) window.draw(m_roof);
}
