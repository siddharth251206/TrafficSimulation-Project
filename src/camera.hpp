#pragma once  // ✅ prevents multiple inclusions

#include <SFML/Graphics.hpp>
#include <optional>

class CameraController
{
public:
    CameraController(float width, float height);

    void handle_zoom(
        const sf::RenderWindow& window,
        const sf::Event::MouseWheelScrolled* wheel,
        unsigned int width,
        unsigned int height
    );
    void handle_resize(unsigned int width, unsigned int height);
    void handle_mouse_drag(const sf::RenderWindow& window, const std::optional<sf::Event> event);
    void handle_kb_panning(float deltaTime);
    void clamp_camera();
    const sf::View& get_camera() const { return m_camera; }

private:
    // Constants for zoom limits
    static constexpr float MIN_ZOOM = 0.5f;     // Prevent zooming in too far
    static constexpr float ZOOM_FACTOR = 0.1f;  // Smooth zoom increment
    static constexpr float PAN_SPEED = 720.f;   // Smooth panning speed
    static constexpr float MAP_MIN_X = 0.f;
    static constexpr float MAP_MAX_X = 2000.f;
    static constexpr float MAP_MIN_Y = 0.f;
    static constexpr float MAP_MAX_Y = 2000.f;

    sf::View m_camera;
    float m_zoomLevel{ 1.f };
    bool m_dragging{ false };
    sf::Vector2i m_lastMousePos;
    float m_max_zoom;
};