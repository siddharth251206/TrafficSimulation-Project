#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "traffic_map.hpp"
#include "camera.hpp"
#include "app_utility.hpp"

// UXController - extends previous UX to provide:
// - Left-side buttons to set Source/Destination for up to 3 user cars.
// - Visual, colored (red/blue/orange) pulsing rings on selected junctions.
// - Minimal, non-destructive additions: preserves existing spawn/despawn behavior.
//
// [ADDED - UI controls & multi-user-car state]
// reason: allow button-driven selection for 3 user-controlled cars; show names and colored highlights
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
    enum class SelectionMode {
        None,
        Car1_SetSource,
        Car1_SetDest,
        Car2_SetSource,
        Car2_SetDest,
        Car3_SetSource,
        Car3_SetDest
    };

    sf::RenderWindow& window;
    CameraController& camera_controller;
    TrafficMap& traffic_map;
    const sf::Texture* car_texture;
    const sf::Font* ui_font;

    std::unique_ptr<sf::Text> ui_text;

    // --- existing single-selection compatibility (kept) ---
    std::shared_ptr<Junction> source_junction;
    std::shared_ptr<Junction> dest_junction;

    // --- [ADDED - structure for each user-controlled car state] ---
    struct UserCar {
        std::shared_ptr<Junction> source_junction = nullptr;
        std::shared_ptr<Junction> dest_junction = nullptr;
        std::string source_name = "";
        std::string dest_name = "";
        bool is_spawned = false;
        sf::Color color = sf::Color::White;
        int spawned_count = 0; // per-car spawned counter (display)
    };

    // Exactly 3 user cars
    std::array<UserCar, 3> user_cars; // [ADDED - stores per-car source/dest, names, colors]

    // Selection mode state
    SelectionMode selection_mode = SelectionMode::None; // [ADDED]

    // Button regions
    struct ButtonRegion {
        sf::FloatRect bounds;
        std::string label;
        SelectionMode mode;
    };
    std::vector<ButtonRegion> left_buttons; // [ADDED - clickable UI buttons on left]

    // Named junctions mapping so button text can show human-friendly names (optional)
    struct NamedJunction {
        sf::Vector2f position;
        std::string name;
        std::shared_ptr<Junction> junction; // filled at init if found in traffic_map
    };
    std::vector<NamedJunction> named_junctions; // [ADDED - used to show names on buttons]

    // UI helper
    sf::Clock animation_clock; // for pulsing ring animation

    // minimal pre-existing members kept
    sf::Clock spawn_timer;
    int spawned_count = 0;
    int max_cars = 3;

    // Helpers (private)
    void reset_selection(); // existing behavior, now also clears selection mode
    std::shared_ptr<Junction> find_junction_at_screenpos(const sf::Vector2i& pixel) const; // convert pixel->world and return junction
    void initialize_left_buttons(); // [ADDED] create button rectangles and default labels
    void initialize_named_junctions(); // [ADDED] map positions -> names (calls traffic_map.get_junction)
    void draw_left_buttons(); // [ADDED] draw and update left-side buttons
    void draw_top_status_bar(); // [ADDED] top bar showing Car 1/2/3 status
    void draw_right_hud(); // [ADDED] help + car types box (compact)
    void draw_highlight_rings(); // [ADDED] draw pulsing rings around selected junctions
    bool is_point_in_rect(const sf::Vector2f& p, const sf::FloatRect& r) const;
};
