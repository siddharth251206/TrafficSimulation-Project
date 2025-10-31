// ux.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include "traffic_map.hpp"
#include "camera.hpp"
#include "app_utility.hpp"

class UXController {
public:
    UXController(sf::RenderWindow& window,
                 CameraController& camera_controller,
                 TrafficMap& traffic_map,
                 const sf::Texture* car_texture,
                 const sf::Font* font);

    void handle_event(const sf::Event& event);
    void update(float dt);
    void render_ui();
    void spawn_cars();

private:
    enum class SelectionMode { None, ChoosingSource, ChoosingDestination };
    SelectionMode mode = SelectionMode::None;

    sf::RenderWindow& window;
    CameraController& camera_controller;
    TrafficMap& traffic_map;
    const sf::Texture* car_texture;
    const sf::Font* ui_font;

    std::shared_ptr<Junction> source_junction;
    std::shared_ptr<Junction> dest_junction;

    std::unique_ptr<sf::Text> ui_text;

    sf::Clock spawn_timer;
    int spawned_count = 0;
    int max_cars = 3;

    void reset_selection();
};
