#include "camera.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <optional>

// ---- UI margins (must match the values used in ux.cpp) ----
static constexpr float UI_LEFT   = 230.f;   // left panel + padding
static constexpr float UI_RIGHT  = 390.f;   // right HUD + padding
static constexpr float UI_TOP    =  44.f;   // top bar + padding

CameraController::CameraController(float width, float height)
{
    // Initialize view at the center of the world
    float centerX = (MAP_MIN_X + MAP_MAX_X) / 2.f;
    float centerY = (MAP_MIN_Y + MAP_MAX_Y) / 2.f;
    
    m_camera = sf::View(sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(width, height)));
    m_camera.setCenter(sf::Vector2f(centerX, centerY));

    // Calculate the zoom needed to fit the ENTIRE map on screen
    float zoomX = (MAP_MAX_X - MAP_MIN_X) / width;
    float zoomY = (MAP_MAX_Y - MAP_MIN_Y) / height;
    
    // Choose the larger zoom factor to ensure no edges are cut off
    m_max_zoom = std::max(zoomX, zoomY);

    // FIX: Start at max zoom (fully zoomed out)
    // Add a tiny buffer (1.1x) so the map edges aren't touching the window bezel
    m_zoomLevel = m_max_zoom * 1.1f; 

    m_camera.setSize(sf::Vector2f(width * m_zoomLevel, height * m_zoomLevel));
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
        m_zoomLevel = std::max(MIN_ZOOM, m_zoomLevel * (1.f - ZOOM_FACTOR)); // Zoom in
    else
        m_zoomLevel = std::min(m_max_zoom, m_zoomLevel * (1.f + ZOOM_FACTOR)); // Zoom out

    if (m_zoomLevel > 0.f)
    {
        m_camera.setSize(
            sf::Vector2f(
                static_cast<float>(width) * m_zoomLevel, static_cast<float>(height) * m_zoomLevel
            )
        );

        sf::Vector2f newWorldPos = window.mapPixelToCoords(mousePos, m_camera);
        sf::Vector2f offset = worldPos - newWorldPos;
        m_camera.move(offset);
    }
    else
    {
        m_zoomLevel = 1.f;
    }
}

void CameraController::handle_mouse_drag(
    const sf::RenderWindow& window,
    const std::optional<sf::Event> event
)
{
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
        
        // FIXED: Removed the 0.5f factor. 
        // 1:1 movement means the map sticks exactly to the mouse cursor.
        sf::Vector2f delta(
            static_cast<float>(m_lastMousePos.x - newPos.x) * m_zoomLevel,
            static_cast<float>(m_lastMousePos.y - newPos.y) * m_zoomLevel
        );
        m_camera.move(delta);
        m_lastMousePos = newPos;
    }
}

void CameraController::handle_resize(unsigned int width, unsigned int height)
{
    m_camera.setSize(sf::Vector2f{
        static_cast<float>(width) * m_zoomLevel,
        static_cast<float>(height) * m_zoomLevel
    });
}

void CameraController::handle_kb_panning(float deltaTime)
{
    // FIXED: Scale speed by zoom level. 
    // Panning covers more ground when zoomed out, less when zoomed in.
    float currentSpeed = PAN_SPEED * m_zoomLevel;

    sf::Vector2f panDelta(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        panDelta.y -= currentSpeed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        panDelta.x -= currentSpeed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        panDelta.y += currentSpeed * deltaTime;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        panDelta.x += currentSpeed * deltaTime;

    m_camera.move(panDelta);
}

void CameraController::clamp_camera(const sf::RenderWindow& window)
{
    const sf::Vector2f viewSize = m_camera.getSize();
    const sf::Vector2f halfSize = viewSize * 0.5f;

    // Map boundaries
    const float mapLeft   = MAP_MIN_X;
    const float mapRight  = MAP_MAX_X;
    const float mapTop    = MAP_MIN_Y;
    const float mapBottom = MAP_MAX_Y;

    // Convert UI margins to world units
    float pixelsToWorldX = m_zoomLevel;
    float pixelsToWorldY = m_zoomLevel;

    const float leftMarginWorld  = UI_LEFT  * pixelsToWorldX;
    const float rightMarginWorld = UI_RIGHT * pixelsToWorldX;
    const float topMarginWorld   = UI_TOP   * pixelsToWorldY;

    // Calculate valid center range
    const float minX = mapLeft   + halfSize.x + leftMarginWorld;
    const float maxX = mapRight  - halfSize.x - rightMarginWorld;
    const float minY = mapTop    + halfSize.y + topMarginWorld;
    const float maxY = mapBottom - halfSize.y;

    sf::Vector2f center = m_camera.getCenter();

    // Logic: If the view is smaller than the map, clamp to edges.
    // If the view is BIGGER than the map (zoomed way out), center the map.
    if (minX <= maxX) center.x = std::clamp(center.x, minX, maxX);
    else center.x = (mapLeft + mapRight) * 0.5f + (leftMarginWorld - rightMarginWorld) * 0.5f;

    if (minY <= maxY) center.y = std::clamp(center.y, minY, maxY);
    else center.y = (mapTop + mapBottom) * 0.5f + topMarginWorld * 0.5f;

    m_camera.setCenter(center);
}