#include "camera.hpp"
#include "traffic_light.hpp"
#include "traffic_map.hpp"
#include "pathfinder.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

int main()
{
    unsigned int width = 1200;
    unsigned int height = 800;
    sf::RenderWindow window{ sf::VideoMode{ { width, height } }, "Traffic Simulator" };
    window.setFramerateLimit(60);

    // Initialize traffic map
    TrafficMap traffic_map;

    // ----------- MAIN CAMPUS GRID EXPANDED ------------
    // Base area (existing)
    const sf::Vector2f gajjar_junction = { 700.f, 100.f };
    const sf::Vector2f bhabha_junction = { 200.f, 100.f };
    const sf::Vector2f nehru_junction = { 100.f, 100.f };
    const sf::Vector2f swami_junction = { 100.f, 200.f };
    const sf::Vector2f swami_entrance_junction = { 200.f, 200.f };
    const sf::Vector2f quarters_junction = { 700.f, 375.f };
    const sf::Vector2f canteen_junction = { 200.f, 375.f };
    const sf::Vector2f temple_junction = { 900.f, 300.f };
    const sf::Vector2f gate1_junction = { 200.f, 700.f };
    const sf::Vector2f gate2_junction = { 700.f, 700.f };
    const sf::Vector2f library_junction = { 300.f, 375.f };
    const sf::Vector2f admin_junction = { 300.f, 500.f };
    const sf::Vector2f department_junction = { 700.f, 500.f };

    // ----------- NEW EXTENDED AREA ------------
    // Northern extension
    const sf::Vector2f north_road1 = { 100.f, 50.f };
    const sf::Vector2f north_road2 = { 200.f, 50.f };
    const sf::Vector2f north_road3 = { 400.f, 50.f };
    const sf::Vector2f north_road4 = { 700.f, 50.f };
    const sf::Vector2f north_road5 = { 900.f, 50.f };

    // Eastern side (existing/previous)
    const sf::Vector2f east_road1 = { 1000.f, 100.f };
    const sf::Vector2f east_road2 = { 1000.f, 300.f };
    const sf::Vector2f east_road3 = { 1000.f, 500.f };
    const sf::Vector2f east_road4 = { 1000.f, 700.f };

    // Western expansion (small)
    const sf::Vector2f west_road1 = { 50.f, 100.f };
    const sf::Vector2f west_road2 = { 50.f, 300.f };
    const sf::Vector2f west_road3 = { 50.f, 500.f };
    const sf::Vector2f west_road4 = { 50.f, 700.f };

    // Southern extension (existing)
    const sf::Vector2f south_road1 = { 100.f, 800.f };
    const sf::Vector2f south_road2 = { 300.f, 800.f };
    const sf::Vector2f south_road3 = { 500.f, 800.f };
    const sf::Vector2f south_road4 = { 700.f, 800.f };
    const sf::Vector2f south_road5 = { 900.f, 800.f };
    const sf::Vector2f south_road6 = { 1000.f, 800.f };

    // Central new area
    const sf::Vector2f sports_junction = { 500.f, 500.f };
    const sf::Vector2f hostel_junction = { 500.f, 300.f };
    const sf::Vector2f lab_junction = { 400.f, 500.f };
    const sf::Vector2f parking_junction = { 900.f, 500.f };
    const sf::Vector2f cafeteria_junction = { 500.f, 375.f };

    // ----------- ADDITIONAL EASTERN JUNCTIONS (existing expansion) ------------
    const sf::Vector2f east1 = { 1100.f, 100.f };
    const sf::Vector2f east2 = { 1100.f, 300.f };
    const sf::Vector2f east3 = { 1100.f, 500.f };
    const sf::Vector2f east4 = { 1100.f, 700.f };

    const sf::Vector2f east5 = { 1300.f, 100.f };
    const sf::Vector2f east6 = { 1300.f, 300.f };
    const sf::Vector2f east7 = { 1300.f, 500.f };
    const sf::Vector2f east8 = { 1300.f, 700.f };

    const sf::Vector2f east9 = { 1500.f, 100.f };
    const sf::Vector2f east10 = { 1500.f, 300.f };
    const sf::Vector2f east11 = { 1500.f, 500.f };
    const sf::Vector2f east12 = { 1500.f, 700.f };

    const sf::Vector2f east13 = { 1700.f, 100.f };
    const sf::Vector2f east14 = { 1700.f, 300.f };
    const sf::Vector2f east15 = { 1700.f, 500.f };
    const sf::Vector2f east16 = { 1700.f, 700.f };

    // ----------- ADDITIONAL SOUTH-EASTERN EXPANSION (existing) ------------
    const float y_offset = 850.f;// below current map (which ended around y=800)

    // New southern junctions aligned with eastern grid
    const sf::Vector2f se1 = { 1100.f, 100.f + y_offset };
    const sf::Vector2f se2 = { 1100.f, 300.f + y_offset };
    const sf::Vector2f se3 = { 1100.f, 500.f + y_offset };
    const sf::Vector2f se4 = { 1100.f, 700.f + y_offset };

    const sf::Vector2f se5 = { 1300.f, 100.f + y_offset };
    const sf::Vector2f se6 = { 1300.f, 300.f + y_offset };
    const sf::Vector2f se7 = { 1300.f, 500.f + y_offset };
    const sf::Vector2f se8 = { 1300.f, 700.f + y_offset };

    const sf::Vector2f se9 = { 1500.f, 100.f + y_offset };
    const sf::Vector2f se10 = { 1500.f, 300.f + y_offset };
    const sf::Vector2f se11 = { 1500.f, 500.f + y_offset };
    const sf::Vector2f se12 = { 1500.f, 700.f + y_offset };

    const sf::Vector2f se13 = { 1700.f, 100.f + y_offset };
    const sf::Vector2f se14 = { 1700.f, 300.f + y_offset };
    const sf::Vector2f se15 = { 1700.f, 500.f + y_offset };
    const sf::Vector2f se16 = { 1700.f, 700.f + y_offset };

    // ----------- NEW WEST-SOUTH EXPANSION (bottom-left block) ------------
    // We'll add a block to the left of the main map and below it.
    // Coordinates use negative x so they sit to the left of existing minimum (~50f).
    // All roads are vertical/horizontal only.

    const float west_x1 = -700.f;// far left column
    const float west_x2 = -500.f;
    const float west_x3 = -300.f;
    const float west_x4 = -100.f;// closer to main

    const float south_start = 900.f;// below existing south ~800
    const float s_row1 = south_start + 100.f;// 1000
    const float s_row2 = south_start + 300.f;// 1200
    const float s_row3 = south_start + 500.f;// 1400
    const float s_row4 = south_start + 700.f;// 1600

    // define left-bottom junctions (4x4 grid)
    // const sf::Vector2f wl1 = { west_x1, s_row1 };
    // const sf::Vector2f wl2 = { west_x1, s_row2 };
    // const sf::Vector2f wl3 = { west_x1, s_row3 };
    // const sf::Vector2f wl4 = { west_x1, s_row4 };

    // const sf::Vector2f wm1 = { west_x2, s_row1 };
    // const sf::Vector2f wm2 = { west_x2, s_row2 };
    // const sf::Vector2f wm3 = { west_x2, s_row3 };
    // const sf::Vector2f wm4 = { west_x2, s_row4 };

    // const sf::Vector2f wr1 = { west_x3, s_row1 };
    // const sf::Vector2f wr2 = { west_x3, s_row2 };
    // const sf::Vector2f wr3 = { west_x3, s_row3 };
    // const sf::Vector2f wr4 = { west_x3, s_row4 };

    // const sf::Vector2f wx1 = { west_x4, s_row1 };
    // const sf::Vector2f wx2 = { west_x4, s_row2 };
    // const sf::Vector2f wx3 = { west_x4, s_row3 };
    // const sf::Vector2f wx4 = { west_x4, s_row4 };

    // ---------------- ADD ROADS ----------------
    // Original campus roads (unchanged)
    traffic_map.add_double_road(gate1_junction, { 200.f, 465.f }, 15.f, false);
    traffic_map.add_double_road(bhabha_junction, gajjar_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, bhabha_junction, 15.f);
    traffic_map.add_double_road(nehru_junction, swami_junction, 15.f);
    traffic_map.add_double_road(bhabha_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_junction, swami_entrance_junction, 15.f);
    traffic_map.add_double_road(swami_entrance_junction, { 200.f, 300.f }, 15.f);
    traffic_map.add_double_road({ 200.f, 300.f }, { 700.f, 300.f }, 15.f);
    traffic_map.add_double_road(gajjar_junction, { 700.f, 300.f }, 15.f);
    traffic_map.add_double_road({ 200.f, 300.f }, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, quarters_junction, 15.f);
    traffic_map.add_double_road({ 700.f, 300.f }, quarters_junction, 15.f);
    traffic_map.add_double_road({ 700.f, 300.f }, temple_junction, 15.f);
    traffic_map.add_double_road(quarters_junction, { 900.f, 375.f }, 15.f);
    traffic_map.add_double_road(gajjar_junction, { 900.f, 100.f }, 15.f);
    traffic_map.add_double_road({ 900.f, 100.f }, temple_junction, 15.f);
    traffic_map.add_double_road({ 900.f, 375.f }, temple_junction, 15.f);
    traffic_map.add_double_road({ 100.f, 375.f }, canteen_junction, 15.f);
    traffic_map.add_double_road(canteen_junction, { 200.f, 465.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 200.f, 465.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 375.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 465.f }, { 100.f, 700.f }, 15.f);
    traffic_map.add_double_road({ 100.f, 700.f }, gate1_junction, 15.f);
    traffic_map.add_double_road(gate1_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(library_junction, admin_junction, 15.f);
    traffic_map.add_double_road(admin_junction, { 300.f, 700.f }, 15.f);
    traffic_map.add_double_road(department_junction, gate2_junction, 15.f);
    traffic_map.add_double_road(department_junction, quarters_junction, 15.f);
    traffic_map.add_double_road(department_junction, admin_junction, 15.f);

    PathFinder pathfinder;
    // ---------------- NEW ROADS ----------------
    // Northern
    traffic_map.add_double_road(north_road1, north_road5, 15.f);
    traffic_map.add_double_road(north_road1, nehru_junction, 15.f);
    traffic_map.add_double_road(north_road5, { 900.f, 100.f }, 15.f);

    // East
    traffic_map.add_double_road(temple_junction, east_road2, 15.f);
    traffic_map.add_double_road(east_road1, east_road2, 15.f);
    traffic_map.add_double_road(east_road2, east_road3, 15.f);
    traffic_map.add_double_road(east_road3, east_road4, 15.f);
    traffic_map.add_double_road(east_road4, south_road6, 15.f);

    // West (small existing)
    traffic_map.add_double_road(west_road1, west_road2, 15.f);
    traffic_map.add_double_road(west_road2, west_road3, 15.f);
    traffic_map.add_double_road(west_road3, west_road4, 15.f);
    traffic_map.add_double_road(west_road4, south_road1, 15.f);
    traffic_map.add_double_road(west_road1, nehru_junction, 15.f);

    // Central
    traffic_map.add_double_road(hostel_junction, cafeteria_junction, 15.f);
    traffic_map.add_double_road(cafeteria_junction, library_junction, 15.f);
    traffic_map.add_double_road(lab_junction, admin_junction, 15.f);
    traffic_map.add_double_road(sports_junction, lab_junction, 15.f);
    traffic_map.add_double_road(sports_junction, department_junction, 15.f);
    traffic_map.add_double_road(parking_junction, temple_junction, 15.f);
    traffic_map.add_double_road(parking_junction, department_junction, 15.f);
    traffic_map.add_double_road(hostel_junction, gajjar_junction, 15.f);

    // ----------- CONNECT EASTERN JUNCTIONS HORIZONTALLY ------------
    traffic_map.add_double_road(east1, east5, 15.f);
    traffic_map.add_double_road(east2, east6, 15.f);
    traffic_map.add_double_road(east3, east7, 15.f);
    traffic_map.add_double_road(east4, east8, 15.f);

    traffic_map.add_double_road(east5, east9, 15.f);
    traffic_map.add_double_road(east6, east10, 15.f);
    traffic_map.add_double_road(east7, east11, 15.f);
    traffic_map.add_double_road(east8, east12, 15.f);

    traffic_map.add_double_road(east9, east13, 15.f);
    traffic_map.add_double_road(east10, east14, 15.f);
    traffic_map.add_double_road(east11, east15, 15.f);
    traffic_map.add_double_road(east12, east16, 15.f);

    // ----------- CONNECT EASTERN JUNCTIONS VERTICALLY ------------
    traffic_map.add_double_road(east1, east2, 15.f);
    traffic_map.add_double_road(east2, east3, 15.f);
    traffic_map.add_double_road(east3, east4, 15.f);

    traffic_map.add_double_road(east5, east6, 15.f);
    traffic_map.add_double_road(east6, east7, 15.f);
    traffic_map.add_double_road(east7, east8, 15.f);

    traffic_map.add_double_road(east9, east10, 15.f);
    traffic_map.add_double_road(east10, east11, 15.f);
    traffic_map.add_double_road(east11, east12, 15.f);

    traffic_map.add_double_road(east13, east14, 15.f);
    traffic_map.add_double_road(east14, east15, 15.f);
    traffic_map.add_double_road(east15, east16, 15.f);

    // ----------- CONNECT OLD MAP TO NEW EASTERN AREA ------------
    traffic_map.add_double_road(temple_junction, east2, 15.f);
    traffic_map.add_double_road(parking_junction, east7, 15.f);
    traffic_map.add_double_road(east4, south_road6, 15.f);

    // ----------- CONNECT SOUTH-EAST JUNCTIONS HORIZONTALLY ------------
    traffic_map.add_double_road(se1, se5, 15.f);
    traffic_map.add_double_road(se2, se6, 15.f);
    traffic_map.add_double_road(se3, se7, 15.f);
    traffic_map.add_double_road(se4, se8, 15.f);

    traffic_map.add_double_road(se5, se9, 15.f);
    traffic_map.add_double_road(se6, se10, 15.f);
    traffic_map.add_double_road(se7, se11, 15.f);
    traffic_map.add_double_road(se8, se12, 15.f);

    traffic_map.add_double_road(se9, se13, 15.f);
    traffic_map.add_double_road(se10, se14, 15.f);
    traffic_map.add_double_road(se11, se15, 15.f);
    traffic_map.add_double_road(se12, se16, 15.f);

    // ----------- CONNECT SOUTH-EAST JUNCTIONS VERTICALLY ------------
    traffic_map.add_double_road(se1, se2, 15.f);
    traffic_map.add_double_road(se2, se3, 15.f);
    traffic_map.add_double_road(se3, se4, 15.f);

    traffic_map.add_double_road(se5, se6, 15.f);
    traffic_map.add_double_road(se6, se7, 15.f);
    traffic_map.add_double_road(se7, se8, 15.f);

    traffic_map.add_double_road(se9, se10, 15.f);
    traffic_map.add_double_road(se10, se11, 15.f);
    traffic_map.add_double_road(se11, se12, 15.f);

    traffic_map.add_double_road(se13, se14, 15.f);
    traffic_map.add_double_road(se14, se15, 15.f);
    traffic_map.add_double_road(se15, se16, 15.f);

    // ----------- CONNECT UPPER EAST BLOCK TO LOWER SOUTH-EAST BLOCK ------------
    traffic_map.add_double_road(east4, se1, 15.f);
    traffic_map.add_double_road(east8, se5, 15.f);
    traffic_map.add_double_road(east12, se9, 15.f);
    traffic_map.add_double_road(east16, se13, 15.f);

    // ----------- EXTRA LINKS FOR FLOW AND BALANCE ------------
    traffic_map.add_double_road(se1, se6, 15.f);
    traffic_map.add_double_road(se5, se10, 15.f);
    traffic_map.add_double_road(se9, se14, 15.f);
    traffic_map.add_double_road(se13, se16, 15.f);
    traffic_map.add_double_road(se4, se8, 15.f);// bottom alignment

    // ----------- NEW WEST-SOUTH EXPANSION (bottom-left block, repositioned) ------------
    // This grid sits just below the main left campus and connects to the right-bottom area.
    // It forms a continuous rectangular layout.

    const float left_x1 = 50.f;// aligns with west_road1
    const float left_x2 = 250.f;
    const float left_x3 = 450.f;
    const float left_x4 = 650.f;

    const float top_y = 850.f;// just below main map
    const float row1 = top_y + 100.f;// 950
    const float row2 = top_y + 300.f;// 1150
    const float row3 = top_y + 500.f;// 1350
    const float row4 = top_y + 700.f;// 1550

    // define left-south grid (4x4 block)
    const sf::Vector2f ls1 = { left_x1, row1 };
    const sf::Vector2f ls2 = { left_x1, row2 };
    const sf::Vector2f ls3 = { left_x1, row3 };
    const sf::Vector2f ls4 = { left_x1, row4 };

    const sf::Vector2f lm1 = { left_x2, row1 };
    const sf::Vector2f lm2 = { left_x2, row2 };
    const sf::Vector2f lm3 = { left_x2, row3 };
    const sf::Vector2f lm4 = { left_x2, row4 };

    const sf::Vector2f lr1 = { left_x3, row1 };
    const sf::Vector2f lr2 = { left_x3, row2 };
    const sf::Vector2f lr3 = { left_x3, row3 };
    const sf::Vector2f lr4 = { left_x3, row4 };

    const sf::Vector2f lx1 = { left_x4, row1 };
    const sf::Vector2f lx2 = { left_x4, row2 };
    const sf::Vector2f lx3 = { left_x4, row3 };
    const sf::Vector2f lx4 = { left_x4, row4 };

    // ----------- GRID CONNECTIONS ------------
    auto add_block_roads = [&](auto a1, auto a2, auto a3, auto a4)
    {
        traffic_map.add_double_road(a1, a2, 15.f);
        traffic_map.add_double_road(a2, a3, 15.f);
        traffic_map.add_double_road(a3, a4, 15.f);
    };
    auto add_row_roads = [&](auto r1, auto r2, auto r3, auto r4)
    {
        traffic_map.add_double_road(r1, r2, 15.f);
        traffic_map.add_double_road(r2, r3, 15.f);
        traffic_map.add_double_road(r3, r4, 15.f);
    };

    // vertical links
    add_block_roads(ls1, ls2, ls3, ls4);
    add_block_roads(lm1, lm2, lm3, lm4);
    add_block_roads(lr1, lr2, lr3, lr4);
    add_block_roads(lx1, lx2, lx3, lx4);

    // horizontal links
    add_row_roads(ls1, lm1, lr1, lx1);
    add_row_roads(ls2, lm2, lr2, lx2);
    add_row_roads(ls3, lm3, lr3, lx3);
    add_row_roads(ls4, lm4, lr4, lx4);

    // ----------- CONNECTIONS TO EXISTING MAP ------------
    traffic_map.add_double_road(west_road4, ls1, 15.f);// connect main west bottom to new grid
    traffic_map.add_double_road(gate1_junction, lm1, 15.f);// connect from main bottom gate area

    // Connect bottom-left grid horizontally into south-east expansion
    traffic_map.add_double_road(lx4, se4, 15.f);
    traffic_map.add_double_road(lx3, se3, 15.f);
    traffic_map.add_double_road(lx2, se2, 15.f);
    traffic_map.add_double_road(lx1, se1, 15.f);

    // extra horizontal connectors for smooth flow
    traffic_map.add_double_road(lx2, se6, 15.f);
    // traffic_map.add_double_road(lx3, se10, 15.f);


    // -----------------------------------------------------------------------

    // ----------- VEHICLES & CAMERA -------------
    sf::Texture car_texture;
    bool loaded = false;
    loaded = AssetHelper::try_load_texture(car_texture, "assets/premium_car.png", "premium car");
    if (!loaded)
        loaded =
            AssetHelper::try_load_texture(car_texture, "assets/advanced_car.png", "advanced car");
    if (!loaded)
        loaded = AssetHelper::try_load_texture(car_texture, "assets/car.png", "simple car");

    sf::Clock spawn_timer;
    int max_cars = 25;// larger map → more cars
    int spawned_count = 0;

    CameraController camera_controller(static_cast<float>(width), static_cast<float>(height));

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.restart();
        float deltaTime = elapsed.asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* wrz = event->getIf<sf::Event::Resized>())
            {
                width = wrz->size.x;
                height = wrz->size.y;
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
                camera_controller.handle_zoom(window, wheel, width, height);

            camera_controller.handle_mouse_drag(window, event);
        }

        camera_controller.handle_kb_panning(deltaTime);
        camera_controller.clamp_camera();

        // Spawn cars
        if (spawned_count < max_cars && spawn_timer.getElapsedTime().asSeconds() > 0.5f)
        {
            auto start_junction = traffic_map.get_junction(gate1_junction);
            auto end_junction = traffic_map.get_junction(gajjar_junction); // e.g., Department junction

            if (start_junction && end_junction)
            {
                // 4. Calculate the optimal path using the Pathfinder
                std::deque<std::weak_ptr<Road>> path = pathfinder.find_path(start_junction, end_junction);

                // 5. Only spawn the car if a valid path was found
                if (!path.empty())
                {
                    // Get the very first road segment from the calculated path
                    if (auto first_road = path.front().lock())
                    {
                        // Create the car and assign it the path
                        auto car = std::make_unique<Car>(first_road, loaded ? &car_texture : nullptr);
                        car->set_path(path);

                        // Add the car to the first road
                        first_road->add(std::move(car));

                        spawned_count++;
                    }
                }
            }
            spawn_timer.restart();
        }

        traffic_map.update(elapsed);
        window.setView(camera_controller.get_camera());
        window.clear(sf::Color(20, 20, 40));
        traffic_map.draw(window);
        window.display();
    }
}
