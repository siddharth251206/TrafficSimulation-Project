// #include "camera.hpp"
// #include "traffic_light.hpp"
// #include "traffic_map.hpp"
// #include <SFML/Graphics.hpp>
// #include <iostream>
// #include <memory>
// #include <optional>
// #include <string>

// int main()
// {
//     unsigned int width = 1200;
//     unsigned int height = 800;
//     sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
//     window.setFramerateLimit(60);

//     // Initialize traffic map
//     TrafficMap traffic_map;

//     // ------ SVNIT MAP ------
//     const sf::Vector2f gajjar_junction = { 700.f, 100.f };
//     const sf::Vector2f bhabha_junction = { 200.f, 100.f };
//     const sf::Vector2f nehru_junction = { 100.f, 100.f };
//     const sf::Vector2f swami_junction = { 100.f, 200.f };
//     const sf::Vector2f swami_entrance_junction = { 200.f, 200.f };
//     const sf::Vector2f quarters_junction = { 700.f, 375.f };
//     const sf::Vector2f canteen_junction = { 200.f, 375.f };
//     const sf::Vector2f temple_junction = { 900.f, 300.f };
//     const sf::Vector2f gate1_junction = { 200.f, 700.f };
//     const sf::Vector2f gate2_junction = { 700.f, 700.f };
//     const sf::Vector2f library_junction = { 300.f, 375.f };
//     const sf::Vector2f admin_junction = { 300.f, 500.f };
//     const sf::Vector2f department_junction = { 700.f, 500.f };

//     // Add roads to the traffic map
//     traffic_map.add_double_road(gate1_junction, { 200.f, 465.f }, 15.f, false);
//     traffic_map.add_double_road(bhabha_junction, gajjar_junction, 15.f);
//     traffic_map.add_double_road(nehru_junction, bhabha_junction, 15.f);
//     traffic_map.add_double_road(nehru_junction, swami_junction, 15.f);
//     traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 15.f);
//     traffic_map.add_double_road(swami_junction, swami_entrance_junction, 15.f);
//     traffic_map.add_double_road(swami_entrance_junction, { 200.f, 300.f }, 15.f);
//     traffic_map.add_double_road({ 200.f, 300.f }, { 700.f, 300.f }, 15.f);
//     traffic_map.add_double_road(gajjar_junction, { 700.f, 300.f }, 15.f);
//     traffic_map.add_double_road({ 200.f, 300.f }, canteen_junction, 15.f);
//     traffic_map.add_double_road(canteen_junction, quarters_junction, 15.f);
//     traffic_map.add_double_road({ 700.f, 300.f }, quarters_junction, 15.f);
//     traffic_map.add_double_road({ 700.f, 300.f }, temple_junction, 15.f);
//     traffic_map.add_double_road(quarters_junction, { 900.f, 375.f }, 15.f);
//     traffic_map.add_double_road(gajjar_junction, { 900.f, 100.f }, 15.f);
//     traffic_map.add_double_road({ 900.f, 100.f }, temple_junction, 15.f);
//     traffic_map.add_double_road({ 900.f, 375.f }, temple_junction, 15.f);
//     traffic_map.add_double_road({ 100.f, 375.f }, canteen_junction, 15.f);
//     traffic_map.add_double_road(canteen_junction, { 200.f, 465.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 465.f }, { 200.f, 465.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 375.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 700.f }, 15.f);
//     traffic_map.add_double_road({ 100.f, 700.f }, gate1_junction, 15.f);
//     traffic_map.add_double_road(gate1_junction, gate2_junction, 15.f);
//     traffic_map.add_double_road(library_junction, admin_junction, 15.f);
//     traffic_map.add_double_road(admin_junction, { 300.f, 700.f }, 15.f);
//     traffic_map.add_double_road(department_junction, gate2_junction, 15.f);
//     traffic_map.add_double_road(department_junction, quarters_junction, 15.f);
//     traffic_map.add_double_road(department_junction, admin_junction, 15.f);

//     // Load car texture
//     sf::Texture car_texture;
//     bool loaded = false;
//     loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
//     if (!loaded)
//         loaded =
//             AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
//     if (!loaded)
//         loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

//     // Car spawner logic
//     sf::Clock spawn_timer;
//     int max_cars = 5;
//     int spawned_count = 0;

//     CameraController camera_controller(static_cast<float>(width), static_cast<float>(height));

//     sf::Clock clock;
//     while (window.isOpen())
//     {
//         sf::Time elapsed = clock.restart();
//         float deltaTime = elapsed.asSeconds();

//         // Handle events
//         while (const std::optional<sf::Event> event = window.pollEvent())
//         {
//             if (event->is<sf::Event::Closed>())
//                 window.close();

//             if (const auto* wrz = event->getIf<sf::Event::Resized>())
//             {
//                 width = wrz->size.x;
//                 height = wrz->size.y;
//             }

//             // Smooth zooming centered on mouse cursor
//             if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
//                 camera_controller.handle_zoom(window, wheel, width, height);

//             // Handle mouse dragging
//             camera_controller.handle_mouse_drag(window, event);
//         }

//         // Smooth keyboard panning
//         camera_controller.handle_kb_panning(deltaTime);

//         // Clamp camera to map boundaries with safety checks
//         camera_controller.clamp_camera();

//         // Spawn cars
//         if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 1.5f)
//         {
//             if (auto road = traffic_map.get_double_road(0))
//             {
//                 road->add_to_forward(
//                     std::make_unique<Car>(road->get_forward(), loaded ? &car_texture : nullptr)
//                 );
//                 spawned_count++;
//             }
//             spawn_timer.restart();
//         }

//         // Update and render
//         traffic_map.update(elapsed);
//         window.setView(camera_controller.get_camera());
//         window.clear(sf::Color(20, 20, 40));
//         traffic_map.draw(window);
//         window.display();
//     }
// }

// main.cpp
#include "camera.hpp"
#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include "app_utility.hpp"          // <-- point_in_circle
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <optional>

int main()
{
    // --------------------------------------------------------------
    // WINDOW
    // --------------------------------------------------------------
    unsigned int width  = 1200;
    unsigned int height = 800;
    sf::RenderWindow window(sf::VideoMode({width, height}),
                            "Traffic Simulator – Source/Dest UI");
    window.setFramerateLimit(60);

    // --------------------------------------------------------------
    // MAP (unchanged – copy from your original)
    // --------------------------------------------------------------
    TrafficMap traffic_map;

    const sf::Vector2f gajjar_junction   = {700.f, 100.f};
    const sf::Vector2f bhabha_junction   = {200.f, 100.f};
    const sf::Vector2f nehru_junction    = {100.f, 100.f};
    const sf::Vector2f swami_junction    = {100.f, 200.f};
    const sf::Vector2f swami_entrance_junction = {200.f, 200.f};
    const sf::Vector2f quarters_junction = {700.f, 375.f};
    const sf::Vector2f canteen_junction  = {200.f, 375.f};
    const sf::Vector2f temple_junction   = {900.f, 300.f};
    const sf::Vector2f gate1_junction    = {200.f, 700.f};
    const sf::Vector2f gate2_junction    = {700.f, 700.f};
    const sf::Vector2f library_junction  = {300.f, 375.f};
    const sf::Vector2f admin_junction    = {300.f, 500.f};
    const sf::Vector2f department_junction = {700.f, 500.f};

    // (all add_double_road calls – unchanged)
    traffic_map.add_double_road(gate1_junction, {200.f, 465.f}, 15.f, false);
    traffic_map.add_double_road(bhabha_junction, gajjar_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, bhabha_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, swami_junction, 15.f);
    traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_entrance_junction, {200.f, 300.f}, 15.f);
    traffic_map.add_double_road({200.f, 300.f}, {700.f, 300.f}, 15.f);
    traffic_map.add_double_road(gajjar_junction, {700.f, 300.f}, 15.f);
    traffic_map.add_double_road({200.f, 300.f}, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, quarters_junction, 15.f);
    traffic_map.add_double_road({700.f, 300.f}, quarters_junction, 15.f);
    traffic_map.add_double_road({700.f, 300.f}, temple_junction, 15.f);
    traffic_map.add_double_road(quarters_junction, {900.f, 375.f}, 15.f);
    traffic_map.add_double_road(gajjar_junction, {900.f, 100.f}, 15.f);
    traffic_map.add_double_road({900.f, 100.f}, temple_junction, 15.f);
    traffic_map.add_double_road({900.f, 375.f}, temple_junction, 15.f);
    traffic_map.add_double_road({100.f, 375.f}, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, {200.f, 465.f}, 15.f);
    traffic_map.add_double_road({100.f, 465.f}, {200.f, 465.f}, 15.f);
    traffic_map.add_double_road({100.f, 465.f}, {100.f, 375.f}, 15.f);
    traffic_map.add_double_road({100.f, 465.f}, {100.f, 700.f}, 15.f);
    traffic_map.add_double_road({100.f, 700.f}, gate1_junction, 15.f);
    traffic_map.add_double_road(gate1_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(library_junction, admin_junction, 15.f);
    traffic_map.add_double_road(admin_junction, {300.f, 700.f}, 15.f);
    traffic_map.add_double_road(department_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(department_junction, quarters_junction, 15.f);
    traffic_map.add_double_road(department_junction, admin_junction, 15.f);

    // --------------------------------------------------------------
    // CAR TEXTURE
    // --------------------------------------------------------------
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded) loaded = AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded) loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

    // --------------------------------------------------------------
    // CAMERA & CLOCKS
    // --------------------------------------------------------------
    CameraController camera_controller(static_cast<float>(width), static_cast<float>(height));
    sf::Clock clock;
    sf::Clock spawn_timer;

    // --------------------------------------------------------------
    // UI STATE
    // --------------------------------------------------------------
    enum class SelectionMode { None, ChoosingSource, ChoosingDestination };
    SelectionMode mode = SelectionMode::None;
    std::shared_ptr<Junction> source_junction = nullptr;
    std::shared_ptr<Junction> dest_junction   = nullptr;

    // --------------------------------------------------------------
    // FONT / TEXT (safe fallback)
    // --------------------------------------------------------------
    sf::Font font;
    const bool font_ok = font.openFromFile("assets/fonts/Roboto-Regular.ttf");

    sf::Text ui_text(font, "", 18);
    if (font_ok) {
        ui_text.setFillColor(sf::Color::White);
        ui_text.setPosition(sf::Vector2f(10.f, 10.f));
    }

    // --------------------------------------------------------------
    // OPTIONAL TRAFFIC LIGHTS
    // --------------------------------------------------------------
    // auto install_lights = [&](const sf::Vector2f& pos) {
    //     if (auto j = traffic_map.get_junction(pos))
    //         j->install_light(sf::seconds(8));
    // };
    // install_lights(gajjar_junction);
    // install_lights(bhabha_junction);
    // install_lights(canteen_junction);
    // install_lights(quarters_junction);

    // --------------------------------------------------------------
    // MAIN LOOP
    // --------------------------------------------------------------
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        float dt = elapsed.asSeconds();

        // ---------- EVENT POLLING ----------
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            // ---- CLOSE ----
            if (event->is<sf::Event::Closed>())
                window.close();

            // ---- RESIZE ----
            if (const auto* wrz = event->getIf<sf::Event::Resized>()) {
                width  = wrz->size.x;
                height = wrz->size.y;
                camera_controller.handle_resize(width, height);
            }

            // ---- ZOOM ----
            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                camera_controller.handle_zoom(window, wheel, width, height);
            }

            // ---- MIDDLE-DRAG PAN ----
            if (event->is<sf::Event::MouseButtonPressed>() ||
                event->is<sf::Event::MouseButtonReleased>() ||
                event->is<sf::Event::MouseMoved>())
            {
                camera_controller.handle_mouse_drag(window, event);
            }

            // ---- UI: LEFT / RIGHT CLICK ----
            if (const auto* mbp = event->getIf<sf::Event::MouseButtonPressed>()) {
                const sf::Vector2i pixel = mbp->position;
                const sf::Vector2f world = window.mapPixelToCoords(pixel, camera_controller.get_camera());

                if (mbp->button == sf::Mouse::Button::Left) {
                    bool hit = false;
                    for (auto& j : traffic_map.get_all_junctions()) {
                        if (point_in_circle(j->get_location(), 25.f, world)) {
                            j->is_selected = true;
                            hit = true;

                            if (mode == SelectionMode::None || mode == SelectionMode::ChoosingSource) {
                                if (source_junction) source_junction->is_source = false;
                                source_junction = j;
                                j->is_source = true;
                                mode = SelectionMode::ChoosingDestination;
                            }
                            else if (mode == SelectionMode::ChoosingDestination) {
                                if (dest_junction) dest_junction->is_destination = false;
                                dest_junction = j;
                                j->is_destination = true;
                                mode = SelectionMode::None;
                            }
                            break;
                        }
                    }
                    if (!hit) {
                        if (source_junction) source_junction->is_source = false;
                        if (dest_junction)   dest_junction->is_destination = false;
                        source_junction = dest_junction = nullptr;
                        mode = SelectionMode::None;
                    }
                }
                else if (mbp->button == sf::Mouse::Button::Right) {
                    for (auto& j : traffic_map.get_all_junctions())
                        j->is_source = j->is_destination = j->is_selected = false;
                    source_junction = dest_junction = nullptr;
                    mode = SelectionMode::None;
                }
            }
        }

        // ---- clear highlight ----
        for (auto& j : traffic_map.get_all_junctions()) j->is_selected = false;

        // ---- keyboard panning & clamp ----
        camera_controller.handle_kb_panning(dt);
        camera_controller.clamp_camera();

        // ---- SPAWN FROM SOURCE ----

        int max_cars = 3;        // <--- CHANGED: Only 3 cars TOTAL
        int spawned_count = 0;

        if (source_junction && spawn_timer.getElapsedTime().asSeconds() > 1.8f && spawned_count < max_cars) {  // <--- ADDED LIMIT CHECK
        const auto& out = source_junction->get_outgoing_roads();
        if (!out.empty()) {
            size_t idx = RNG::instance().getIndex(0, out.size() - 1);
            if (auto road = out[idx].lock())
                road->add(std::make_unique<Car>(road,loaded ? &car_texture : nullptr));
            spawned_count++;  // <--- INCREMENT COUNTER
        }
        spawn_timer.restart();
    }

        // ---- DESPAWN AT DESTINATION ----
        // if (dest_junction) {
        //     auto& q = dest_junction->get_car_queue();
        //     while (!q.empty()) q.pop();
        // }

        // ---- UPDATE ----
        traffic_map.update(elapsed);

        // ---- RENDER ----
        window.setView(camera_controller.get_camera());
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);

        // ---- UI TEXT ----
        // ---- UI TEXT (IMPROVED) ----
    if (font_ok) {
        std::string txt;
        
        // Base instruction
        txt = "L-Click: Select Source → Dest | R-Click: Clear";

        // Show selection progress
        if (source_junction && dest_junction) {
            txt = "Route: (" +
                std::to_string(static_cast<int>(source_junction->get_location().x)) + "," +
                std::to_string(static_cast<int>(source_junction->get_location().y)) + ") → (" +
                std::to_string(static_cast<int>(dest_junction->get_location().x)) + "," +
                std::to_string(static_cast<int>(dest_junction->get_location().y)) + ")";
        }
        else if (source_junction) {
            txt = "Source selected – click destination.";
        }

        // Add spawn counter (always show)
        txt += " | Cars: " + std::to_string(spawned_count) + "/3";

        // Final state
        if (spawned_count >= 3 && source_junction && dest_junction) {
            txt += " | All cars spawned!";
        }

        ui_text.setString(txt);
        ui_text.setCharacterSize(18);
        ui_text.setFillColor(sf::Color::White);
        // ui_text.setPosition(10.f, 10.f); // Top-left corner

        window.setView(window.getDefaultView());
        window.draw(ui_text);
        window.setView(camera_controller.get_camera());
    }

        window.display();
    }

    return 0;
}