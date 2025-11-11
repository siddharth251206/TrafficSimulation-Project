#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include "camera.hpp"
#include "traffic_map.hpp"
#include "car.hpp"

enum class SelectionMode
{
    None,
    Car1_SetSource, Car1_SetDest,
    Car2_SetSource, Car2_SetDest,
    Car3_SetSource, Car3_SetDest
};

struct ButtonRegion
{
    sf::FloatRect bounds;
    std::string label;
    SelectionMode mode;
};

struct NamedJunction
{
    sf::Vector2f position;
    std::string name;
    std::shared_ptr<Junction> junction;
};

struct UserCar
{
    std::shared_ptr<Junction> source_junction;
    std::shared_ptr<Junction> dest_junction;
    std::string source_name;
    std::string dest_name;
    sf::Color color;
    bool is_spawned = false;
    int spawned_count = 0;
};

class UXController
{
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
    void reset_selection();

private:
    sf::RenderWindow& window;
    CameraController& camera_controller;
    TrafficMap& traffic_map;
    const sf::Texture* car_texture;
    const sf::Font* ui_font;

    std::unique_ptr<sf::Text> ui_text;

    std::vector<ButtonRegion> left_buttons;
    std::vector<NamedJunction> named_junctions;
    std::array<UserCar, 3> user_cars;

    SelectionMode selection_mode = SelectionMode::None;

    // Info panel animation state
    bool show_info_panel = false;
    float info_alpha = 0.f;
    float info_slide = 0.f;
    sf::FloatRect info_button_rect;

    sf::Clock animation_clock;

    void initialize_left_buttons();
    void initialize_named_junctions();
    std::shared_ptr<Junction> find_junction_at_screenpos(const sf::Vector2i& pixel) const;
    bool is_point_in_rect(const sf::Vector2f& p, const sf::FloatRect& r) const;

    void draw_left_buttons();
    void draw_top_status_bar();
    void draw_right_hud();
    void draw_highlight_rings();
};