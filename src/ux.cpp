// ux.cpp
#include "ux.hpp"
#include <iostream>

UXController::UXController(sf::RenderWindow& window,
                           CameraController& camera_controller,
                           TrafficMap& traffic_map,
                           const sf::Texture* car_texture,
                           const sf::Font* font)
    : window(window),
      camera_controller(camera_controller),
      traffic_map(traffic_map),
      car_texture(car_texture),
      ui_font(font)
{
    if (ui_font)
    {
        ui_text = std::make_unique<sf::Text>(*ui_font, "", 18);
        ui_text->setFillColor(sf::Color::White);
        ui_text->setPosition(sf::Vector2f(10.f, 10.f));
    }
}

void UXController::handle_event(const sf::Event& event)
{
    // Zoom
    if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>())
        camera_controller.handle_zoom(window, wheel, window.getSize().x, window.getSize().y);

    // Resize
    if (const auto* wrz = event.getIf<sf::Event::Resized>())
        camera_controller.handle_resize(wrz->size.x, wrz->size.y);

    // Mouse drag
    if (event.is<sf::Event::MouseButtonPressed>() ||
        event.is<sf::Event::MouseButtonReleased>() ||
        event.is<sf::Event::MouseMoved>())
        camera_controller.handle_mouse_drag(window, event);

    // Left/Right click for junction selection
    if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>())
    {
        const sf::Vector2i pixel = mbp->position;
        const sf::Vector2f world = window.mapPixelToCoords(pixel, camera_controller.get_camera());

        if (mbp->button == sf::Mouse::Button::Left)
        {
            bool hit = false;
            for (auto& j : traffic_map.get_all_junctions())
            {
                if (point_in_circle(j->get_location(), 25.f, world))
                {
                    j->is_selected = true;
                    hit = true;

                    if (mode == SelectionMode::None || mode == SelectionMode::ChoosingSource)
                    {
                        if (source_junction) source_junction->is_source = false;
                        source_junction = j;
                        j->is_source = true;
                        mode = SelectionMode::ChoosingDestination;
                    }
                    else if (mode == SelectionMode::ChoosingDestination)
                    {
                        if (dest_junction) dest_junction->is_destination = false;
                        dest_junction = j;
                        j->is_destination = true;
                        mode = SelectionMode::None;
                    }
                    break;
                }
            }

            if (!hit)
                reset_selection();
        }
        else if (mbp->button == sf::Mouse::Button::Right)
        {
            reset_selection();
        }
    }
}

void UXController::reset_selection()
{
    for (auto& j : traffic_map.get_all_junctions())
        j->is_source = j->is_destination = j->is_selected = false;

    source_junction = dest_junction = nullptr;
    mode = SelectionMode::None;
}

void UXController::update(float dt)
{
    camera_controller.handle_kb_panning(dt);
    camera_controller.clamp_camera();

    for (auto& j : traffic_map.get_all_junctions())
        j->is_selected = false;
}

void UXController::spawn_cars()
{
    if (!source_junction) return;

    if (spawn_timer.getElapsedTime().asSeconds() > 1.8f && spawned_count < max_cars)
    {
        const auto& out = source_junction->get_outgoing_roads();
        if (!out.empty())
        {
            size_t idx = RNG::instance().getIndex(0, out.size() - 1);
            if (auto road = out[idx].lock())
                road->add(std::make_unique<Car>(road, car_texture));
            spawned_count++;
        }
        spawn_timer.restart();
    }
}

void UXController::render_ui()
{
    if (!ui_text) return;

    std::string txt = "L-Click: Select Source → Dest | R-Click: Clear";

    if (source_junction && dest_junction)
    {
        txt = "Route: (" +
            std::to_string(static_cast<int>(source_junction->get_location().x)) + "," +
            std::to_string(static_cast<int>(source_junction->get_location().y)) + ") → (" +
            std::to_string(static_cast<int>(dest_junction->get_location().x)) + "," +
            std::to_string(static_cast<int>(dest_junction->get_location().y)) + ")";
    }
    else if (source_junction)
    {
        txt = "Source selected – click destination.";
    }

    txt += " | Cars: " + std::to_string(spawned_count) + "/" + std::to_string(max_cars);

    if (spawned_count >= max_cars && source_junction && dest_junction)
        txt += " | All cars spawned!";

    ui_text->setString(txt);
    ui_text->setCharacterSize(18);
    ui_text->setFillColor(sf::Color::White);
    ui_text->setPosition(sf::Vector2f(10.f, 10.f));

    window.setView(window.getDefaultView());
    window.draw(*ui_text);
    window.setView(camera_controller.get_camera());
}
