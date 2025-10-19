#include "camera.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <optional>

CameraController::CameraController(float width, float height)
{
    m_camera = sf::View(sf::FloatRect(sf::Vector2f{ 0.f, 0.f }, sf::Vector2f{ width, height }));
    m_camera.setCenter(sf::Vector2f(width / 2.f, height / 2.f));
    m_max_zoom = std::max((MAP_MAX_X - MAP_MIN_X) / width, (MAP_MAX_Y - MAP_MIN_Y) / height);
}

void CameraController::handle_zoom(
    const sf::RenderWindow& window,
    const sf::Event::MouseWheelScrolled* wheel,
    unsigned int width,
    unsigned int height
)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, m_camera);

    if (wheel->delta > 0)
        m_zoomLevel = std::max(MIN_ZOOM, m_zoomLevel * (1.f - ZOOM_FACTOR));// Zoom in
    else
        m_zoomLevel = std::min(m_max_zoom, m_zoomLevel * (1.f + ZOOM_FACTOR));// Zoom out

    // Safety check for valid m_camera size
    if (m_zoomLevel > 0.f)
    {
        m_camera.setSize(
            sf::Vector2f(
                static_cast<float>(width) * m_zoomLevel, static_cast<float>(height) * m_zoomLevel
            )
        );

        // Adjust m_camera to keep the mouse's world position fixed
        sf::Vector2f newWorldPos = window.mapPixelToCoords(mousePos, m_camera);
        sf::Vector2f offset = worldPos - newWorldPos;
        m_camera.move(offset);
    }
    else// dead code, just defensive
    {
        // Reset to default zoom if invalid
        m_zoomLevel = 1.f;
    }
}

void CameraController::handle_mouse_drag(
    const sf::RenderWindow& window,
    const std::optional<sf::Event> event
)
{
    // Handle mouse m_dragging
    if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>())
    {
        if (mbp->button == sf::Mouse::Button::Middle)
        {
            m_dragging = true;
            m_lastMousePos = sf::Mouse::getPosition(window);
        }
    }
    else if (const auto* mbr = event->getIf<sf::Event::MouseButtonReleased>())
    {
        if (mbr->button == sf::Mouse::Button::Middle)
            m_dragging = false;
    }

    if (event->is<sf::Event::MouseMoved>() && m_dragging)
    {
        sf::Vector2i newPos = sf::Mouse::getPosition(window);
        sf::Vector2f delta(
            static_cast<float>(m_lastMousePos.x - newPos.x) * m_zoomLevel
                * 0.5f,// 0.5 for smooth movement
            static_cast<float>(m_lastMousePos.y - newPos.y) * m_zoomLevel * 0.5f
        );
        m_camera.move(delta);
        m_lastMousePos = newPos;
    }
}

void CameraController::handle_kb_panning(float deltaTime)
{
    // Smooth keyboard panning
    sf::Vector2f panDelta(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        panDelta.y -= PAN_SPEED * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        panDelta.x -= PAN_SPEED * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        panDelta.y += PAN_SPEED * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        panDelta.x += PAN_SPEED * deltaTime;

    m_camera.move(panDelta);
}

void CameraController::clamp_camera()
{
    // Clamp m_camera to map boundaries with safety checks
    sf::Vector2f center = m_camera.getCenter();
    sf::Vector2f halfSize = m_camera.getSize() / 2.f;

    // Ensure clamping bounds are valid
    float minX = MAP_MIN_X + halfSize.x;
    float maxX = MAP_MAX_X - halfSize.x;
    float minY = MAP_MIN_Y + halfSize.y;
    float maxY = MAP_MAX_Y - halfSize.y;

    // Prevent invalid clamp ranges
    if (minX <= maxX)
        center.x = std::clamp(center.x, minX, maxX);
    else
        center.x = (MAP_MIN_X + MAP_MAX_X) / 2.f;// Center if invalid

    if (minY <= maxY)
        center.y = std::clamp(center.y, minY, maxY);
    else
        center.y = (MAP_MIN_Y + MAP_MAX_Y) / 2.f;// Center if invalid

    m_camera.setCenter(center);
}
