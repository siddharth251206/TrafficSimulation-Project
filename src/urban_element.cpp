#include "urban_element.hpp"
#include "app_utility.hpp"
#include <unordered_map>
#include <memory>
#include <string>

namespace {
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
}

UrbanElement::UrbanElement(const sf::Vector2f& position, UrbanElementType type)
    : m_position(position)
    , m_type(type)
    , m_use_circle(true)
{
    setup_appearance();
}

void UrbanElement::setup_appearance()
{
    m_use_sprite = false;
    m_texture.reset();

    switch (m_type)
    {
    case UrbanElementType::Tree:
        m_use_circle = true;
        m_shape.setRadius(8.f);
        m_shape.setPosition(m_position - sf::Vector2f(8.f, 8.f));
        m_color = sf::Color(60, 140, 60); // Dark green
        m_shape.setFillColor(m_color);
        m_shape.setOutlineColor(sf::Color(40, 100, 40));
        m_shape.setOutlineThickness(-1.f);
        break;

    case UrbanElementType::StreetLamp:
        m_use_circle = true;
        m_shape.setRadius(4.f);
        m_shape.setPosition(m_position - sf::Vector2f(4.f, 4.f));
        m_color = sf::Color(255, 240, 180); // Warm light
        m_shape.setFillColor(m_color);
        m_shape.setOutlineColor(sf::Color(80, 80, 80));
        m_shape.setOutlineThickness(-1.f);
        break;

    case UrbanElementType::Bench:
        m_use_circle = false;
        m_rect_shape.setSize(sf::Vector2f(12.f, 6.f));
        m_rect_shape.setPosition(m_position - sf::Vector2f(6.f, 3.f));
        m_color = sf::Color(139, 90, 43); // Brown
        m_rect_shape.setFillColor(m_color);
        m_rect_shape.setOutlineColor(sf::Color(100, 60, 30));
        m_rect_shape.setOutlineThickness(-1.f);
        break;

    case UrbanElementType::BusStop:
        m_use_circle = false;
        m_rect_shape.setSize(sf::Vector2f(20.f, 12.f));
        m_rect_shape.setPosition(m_position - sf::Vector2f(10.f, 6.f));
        m_color = sf::Color(180, 180, 200); // Light blue-gray
        m_rect_shape.setFillColor(m_color);
        m_rect_shape.setOutlineColor(sf::Color(100, 100, 120));
        m_rect_shape.setOutlineThickness(-2.f);
        break;

    case UrbanElementType::ParkingLot:
        m_use_circle = false;
        m_rect_shape.setSize(sf::Vector2f(60.f, 40.f));
        m_rect_shape.setPosition(m_position - sf::Vector2f(30.f, 20.f));
        m_color = sf::Color(80, 80, 80); // Dark gray
        m_rect_shape.setFillColor(m_color);
        m_rect_shape.setOutlineColor(sf::Color(200, 200, 0)); // Yellow lines
        m_rect_shape.setOutlineThickness(-2.f);
        break;

    case UrbanElementType::Fountain:
        m_use_circle = true;
        m_shape.setRadius(15.f);
        m_shape.setPosition(m_position - sf::Vector2f(15.f, 15.f));
        m_color = sf::Color(100, 180, 220); // Light blue
        m_shape.setFillColor(m_color);
        m_shape.setOutlineColor(sf::Color(200, 200, 200)); // White stone
        m_shape.setOutlineThickness(-3.f);
        break;

    case UrbanElementType::GrassPatch:
        m_use_circle = false;
        m_rect_shape.setSize(sf::Vector2f(40.f, 30.f));
        m_rect_shape.setPosition(m_position - sf::Vector2f(20.f, 15.f));
        m_color = sf::Color(80, 160, 80); // Green
        m_rect_shape.setFillColor(m_color);
        m_rect_shape.setOutlineColor(sf::Color(60, 120, 60));
        m_rect_shape.setOutlineThickness(-1.f);
        break;
    }

    // Try to apply sprites
    std::string rel;
    const char* label = "urban";
    switch (m_type)
    {
    case UrbanElementType::Tree:       rel = "assets/urban/tree.png"; label = "tree"; break;
    case UrbanElementType::StreetLamp: rel = "assets/urban/streetlamp.png"; label = "streetlamp"; break;
    case UrbanElementType::Bench:      rel = "assets/urban/bench.png"; label = "bench"; break;
    case UrbanElementType::BusStop:    rel = "assets/urban/bus_stop.png"; label = "bus_stop"; break;
    case UrbanElementType::ParkingLot: rel = "assets/urban/parking_lot.png"; label = "parking_lot"; break;
    case UrbanElementType::Fountain:   rel = "assets/urban/fountain.png"; label = "fountain"; break;
    case UrbanElementType::GrassPatch: rel = "assets/urban/grass.png"; label = "grass"; break;
    }

    if (!rel.empty())
    {
        if (auto tex = load_cached_texture(rel, label))
        {
            m_texture = tex;
            m_sprite = std::make_unique<sf::Sprite>(*m_texture);
            auto tsize = m_texture->getSize();
            if (tsize.x > 0 && tsize.y > 0)
            {
                float target_w = 0.f, target_h = 0.f;
                if (m_use_circle)
                {
                    target_w = target_h = m_shape.getRadius() * 2.f;
                }
                else
                {
                    target_w = m_rect_shape.getSize().x;
                    target_h = m_rect_shape.getSize().y;
                }
                const float sx = target_w / static_cast<float>(tsize.x);
                const float sy = target_h / static_cast<float>(tsize.y);
                m_sprite->setScale({ sx, sy });
            }
            m_sprite->setOrigin({ m_texture->getSize().x / 2.f, m_texture->getSize().y / 2.f });
            m_sprite->setPosition(m_position);
            m_use_sprite = true;
        }
    }
}

void UrbanElement::draw(sf::RenderWindow& window) const
{
    if (m_use_sprite && m_sprite)
    {
        window.draw(*m_sprite);
        return;
    }

    if (m_use_circle)
        window.draw(m_shape);
    else
        window.draw(m_rect_shape);
}
