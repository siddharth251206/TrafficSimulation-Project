#include "ux.hpp"
#include "pathfinder.hpp"
#include <iostream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <cstdint>

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
        ui_text = std::make_unique<sf::Text>(*ui_font, "", 16);
        ui_text->setFillColor(sf::Color::White);
        ui_text->setPosition({10.f, 10.f});
    }

    // Car colors
    user_cars[0].color = sf::Color(220, 20, 60);   // Red
    user_cars[1].color = sf::Color(30, 144, 255);  // Blue
    user_cars[2].color = sf::Color(255, 140, 0);   // Orange

    initialize_left_buttons();
    initialize_named_junctions();

    // Info button rectangle (top-right)
    float btnSize = 28.f;
    info_button_rect = sf::FloatRect(
        {window.getSize().x - btnSize - 10.f, 6.f},
        {btnSize, btnSize}
    );
}

void UXController::initialize_left_buttons()
{
    left_buttons.clear();
    const float bw = 200.f;
    const float bh = 34.f;
    const float left_x = 12.f;
    float y = 70.f;

    auto push = [&](const std::string& label, SelectionMode mode) {
        ButtonRegion b;
        b.bounds = sf::FloatRect({left_x, y}, {bw, bh});
        b.label = label;
        b.mode = mode;
        left_buttons.push_back(b);
        y += bh + 8.f;
    };

    push("Car 1: Set Source", SelectionMode::Car1_SetSource);
    push("Car 1: Set Destination", SelectionMode::Car1_SetDest);
    y += 6.f;
    push("Car 2: Set Source", SelectionMode::Car2_SetSource);
    push("Car 2: Set Destination", SelectionMode::Car2_SetDest);
    y += 6.f;
    push("Car 3: Set Source", SelectionMode::Car3_SetSource);
    push("Car 3: Set Destination", SelectionMode::Car3_SetDest);
}

void UXController::initialize_named_junctions()
{
    named_junctions = {
        {{700.f, 100.f}, "Staff Quarters", nullptr},
        {{200.f, 100.f}, "Bhabha", nullptr},
        {{100.f, 100.f}, "Nehru", nullptr},
        {{100.f, 200.f}, "Swami", nullptr},
        {{200.f, 200.f}, "Swami Entrance", nullptr},
        {{700.f, 375.f}, "Quarters", nullptr},
        {{200.f, 375.f}, "Canteen", nullptr},
        {{900.f, 300.f}, "Temple", nullptr},
        {{200.f, 700.f}, "Gate 1", nullptr},
        {{700.f, 700.f}, "Gate 2", nullptr},
        {{300.f, 375.f}, "Library", nullptr},
        {{300.f, 500.f}, "Admin", nullptr},
        {{700.f, 500.f}, "Department", nullptr}
    };

    for (auto &nj : named_junctions)
        nj.junction = traffic_map.get_junction(nj.position);
}

std::shared_ptr<Junction> UXController::find_junction_at_screenpos(const sf::Vector2i& pixel) const
{
    sf::Vector2f world = window.mapPixelToCoords(pixel, camera_controller.get_camera());
    return traffic_map.get_junction(world);
}

bool UXController::is_point_in_rect(const sf::Vector2f& p, const sf::FloatRect& r) const
{
    return (p.x >= r.position.x && p.x <= r.position.x + r.size.x &&
            p.y >= r.position.y && p.y <= r.position.y + r.size.y);
}

void UXController::handle_event(const sf::Event& event)
{
    if (const auto* wheel = event.getIf<sf::Event::MouseWheelScrolled>())
        camera_controller.handle_zoom(window, wheel, window.getSize().x, window.getSize().y);

    if (const auto* wrz = event.getIf<sf::Event::Resized>())
        camera_controller.handle_resize(wrz->size.x, wrz->size.y);

    if (event.is<sf::Event::MouseButtonPressed>() ||
        event.is<sf::Event::MouseButtonReleased>() ||
        event.is<sf::Event::MouseMoved>())
        camera_controller.handle_mouse_drag(window, event);

    if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>())
    {
        const sf::Vector2f click_screenf(static_cast<float>(mbp->position.x), static_cast<float>(mbp->position.y));

        // Info button click
        if (is_point_in_rect(click_screenf, info_button_rect))
        {
            show_info_panel = !show_info_panel;
            return;
        }

        if (mbp->button == sf::Mouse::Button::Left)
        {
            for (auto &btn : left_buttons)
            {
                if (is_point_in_rect(click_screenf, btn.bounds))
                {
                    selection_mode = btn.mode;
                    return;
                }
            }

            if (selection_mode != SelectionMode::None)
            {
                auto clicked_junc = find_junction_at_screenpos(mbp->position);
                if (clicked_junc)
                {
                    std::string found_name;
                    for (const auto &nj : named_junctions)
                    {
                        if (nj.junction && nj.junction == clicked_junc)
                        {
                            found_name = nj.name;
                            break;
                        }
                        if (std::hypot(nj.position.x - clicked_junc->get_location().x,
                                       nj.position.y - clicked_junc->get_location().y) < 6.f)
                        {
                            found_name = nj.name;
                            break;
                        }
                    }

                    if (found_name.empty())
                    {
                        std::ostringstream ss;
                        ss << "(" << static_cast<int>(clicked_junc->get_location().x)
                           << "," << static_cast<int>(clicked_junc->get_location().y) << ")";
                        found_name = ss.str();
                    }

                    switch (selection_mode)
                    {
                        case SelectionMode::Car1_SetSource: user_cars[0].source_junction = clicked_junc; user_cars[0].source_name = found_name; user_cars[0].is_spawned = false; break;
                        case SelectionMode::Car1_SetDest:   user_cars[0].dest_junction   = clicked_junc; user_cars[0].dest_name   = found_name; break;
                        case SelectionMode::Car2_SetSource: user_cars[1].source_junction = clicked_junc; user_cars[1].source_name = found_name; user_cars[1].is_spawned = false; break;
                        case SelectionMode::Car2_SetDest:   user_cars[1].dest_junction   = clicked_junc; user_cars[1].dest_name   = found_name; break;
                        case SelectionMode::Car3_SetSource: user_cars[2].source_junction = clicked_junc; user_cars[2].source_name = found_name; user_cars[2].is_spawned = false; break;
                        case SelectionMode::Car3_SetDest:   user_cars[2].dest_junction   = clicked_junc; user_cars[2].dest_name   = found_name; break;
                        default: break;
                    }

                    spawn_cars();
                    selection_mode = SelectionMode::None;
                    return;
                }
            }
            selection_mode = SelectionMode::None;
        }
        else if (mbp->button == sf::Mouse::Button::Right)
        {
            for (auto &uc : user_cars)
            {
                uc.source_junction = nullptr;
                uc.dest_junction = nullptr;
                uc.source_name.clear();
                uc.dest_name.clear();
                uc.is_spawned = false;
                uc.spawned_count = 0;
            }
            selection_mode = SelectionMode::None;
        }
    }
}

void UXController::update(float dt)
{
    camera_controller.handle_kb_panning(dt);
    camera_controller.clamp_camera(window);

    // Fade + slide animation for info panel
    float fadeSpeed = 2.5f;
    if (show_info_panel)
    {
        info_alpha = std::min(255.f, info_alpha + fadeSpeed * dt * 255.f);
        info_slide = std::min(0.f, info_slide + dt * 250.f);
    }
    else
    {
        info_alpha = std::max(0.f, info_alpha - fadeSpeed * dt * 255.f);
        info_slide = std::max(-40.f, info_slide - dt * 250.f);
    }
}

void UXController::spawn_cars()
{
    for (int i = 0; i < 3; ++i)
    {
        auto &uc = user_cars[i];
        if (!uc.is_spawned && uc.source_junction && uc.dest_junction)
        {
            const auto& outgoing = uc.source_junction->get_outgoing_roads();
            if (outgoing.empty()) continue;

            size_t idx = RNG::instance().getIndex(0, outgoing.size() - 1);
            if (auto start_road = outgoing[idx].lock())
            {
                PathFinder pf;
                auto raw_path = pf.find_path(uc.source_junction, uc.dest_junction);
                if (raw_path.empty())
                    continue;

                std::weak_ptr<Road> final_road = raw_path.back();
                auto final_ptr = final_road.lock();
                if (!final_ptr) continue;

                float dest_distance = final_ptr->getLength() * 0.9f;
                auto car = std::make_unique<Car>(start_road, nullptr, RNG::instance().getFloat(0.f, 10.f), uc.color);

                std::deque<std::weak_ptr<Road>> dq(raw_path.begin(), raw_path.end());
                car->set_destination(std::move(dq), final_road, dest_distance);
                start_road->add(std::move(car));

                uc.is_spawned = true;
                uc.spawned_count++;
            }
        }
    }
}

void UXController::draw_left_buttons()
{
    sf::RectangleShape side_panel({220.f, static_cast<float>(window.getSize().y)});
    side_panel.setPosition({0.f, 0.f});
    side_panel.setFillColor(sf::Color(10, 15, 30, 220));
    window.draw(side_panel);

    for (size_t i = 0; i < left_buttons.size(); ++i)
    {
        const ButtonRegion &btn = left_buttons[i];
        sf::RectangleShape bg(btn.bounds.size);
        bg.setPosition(btn.bounds.position);

        bool active = (selection_mode == btn.mode);
        int car_index = static_cast<int>(i / 2);
        bool is_source_button = (i % 2 == 0);

        sf::Color fill = active ? sf::Color(60, 120, 60, 200) : sf::Color(40, 40, 60, 180);
        sf::Color outline = active ? sf::Color(120, 220, 120) : sf::Color(120, 120, 120);
        std::string text = btn.label;

        if (car_index >= 0 && car_index < 3) {
            if (is_source_button && !user_cars[car_index].source_name.empty())
                text = user_cars[car_index].source_name;
            if (!is_source_button && !user_cars[car_index].dest_name.empty())
                text = user_cars[car_index].dest_name;
        }

        bg.setFillColor(fill);
        bg.setOutlineThickness(2.f);
        bg.setOutlineColor(outline);
        window.draw(bg);

        if (ui_font)
        {
            sf::Text t(*ui_font, text, 12);
            t.setFillColor(sf::Color::White);
            t.setPosition({btn.bounds.position.x + 8.f, btn.bounds.position.y + 6.f});
            window.draw(t);
        }

        if (car_index >= 0 && car_index < 3)
        {
            sf::CircleShape dot(6.f);
            dot.setOrigin({6.f, 6.f});
            dot.setPosition({
                btn.bounds.position.x + btn.bounds.size.x - 14.f,
                btn.bounds.position.y + btn.bounds.size.y / 2.f
            });
            dot.setFillColor(user_cars[car_index].color);
            window.draw(dot);
        }
    }
}

void UXController::draw_top_status_bar()
{
    float w = static_cast<float>(window.getSize().x);
    sf::RectangleShape bar({w, 34.f});
    bar.setPosition({0.f, 0.f});
    bar.setFillColor(sf::Color(25, 35, 45, 220));
    window.draw(bar);

    if (!ui_font) return;

    float x = 12.f;
    float y = 8.f;

    sf::Text title(*ui_font, "User Cars:", 12);
    title.setFillColor(sf::Color(180, 200, 220));
    title.setPosition({x, y});
    window.draw(title);
    x += 80.f;

    for (size_t i = 0; i < user_cars.size(); ++i)
    {
        std::string label = "Car " + std::to_string(i+1) + " Idle";
        sf::Color dot_color = sf::Color(120, 120, 120);
        if (user_cars[i].is_spawned) { label = "Car " + std::to_string(i+1) + " Active"; dot_color = user_cars[i].color; }
        else if (user_cars[i].source_junction || user_cars[i].dest_junction) { label = "Car " + std::to_string(i+1) + " Ready"; dot_color = user_cars[i].color; }

        sf::CircleShape dot(6.f);
        dot.setOrigin({6.f, 6.f});
        dot.setPosition({x + 6.f, y + 8.f});
        dot.setFillColor(dot_color);
        window.draw(dot);

        sf::Text t(*ui_font, label, 11);
        t.setFillColor(sf::Color::White);
        t.setPosition({x + 18.f, y});
        window.draw(t);

        x += 200.f;
    }
}

void UXController::draw_right_hud()
{
    float w = static_cast<float>(window.getSize().x);
    float h = static_cast<float>(window.getSize().y);

    sf::RectangleShape help_box({380.f, 170.f});
    help_box.setPosition({w - 390.f, 10.f});
    help_box.setFillColor(sf::Color(0, 0, 0, 180));
    help_box.setOutlineThickness(2.f);
    help_box.setOutlineColor(sf::Color(255, 215, 0));
    window.draw(help_box);

    if (ui_font) {
        float x = w - 380.f + 10.f;
        float y = 18.f;
        sf::Text title(*ui_font, "HOW TO CONTROL CARS:", 13);
        title.setFillColor(sf::Color(255, 215, 0));
        title.setPosition({x, y});
        window.draw(title);
        y += 24.f;
        std::vector<std::string> lines = {
            "1. Click 'Set Car X Source' / 'Set Destination' button",
            "2. Click any junction on the map",
            "3. Car spawns and follows optimal path",
            " ",
            "Car Colors: Red / Blue / Orange = User controlled"
        };
        for (auto &l : lines) {
            sf::Text txt(*ui_font, l, 12);
            txt.setFillColor(sf::Color::White);
            txt.setPosition({x, y});
            window.draw(txt);
            y += 16.f;
        }
    }

    sf::RectangleShape legend({250.f, 110.f});
    legend.setPosition({w - 260.f, h/2.f});
    legend.setFillColor(sf::Color(0, 0, 0, 160));
    legend.setOutlineThickness(2.f);
    legend.setOutlineColor(sf::Color(100, 200, 255));
    window.draw(legend);

    if (ui_font) {
        float lx = w - 250.f + 10.f;
        float ly = h/2.f + 8.f;
        sf::Text title(*ui_font, "CAR TYPES:", 13);
        title.setFillColor(sf::Color(100,200,255));
        title.setPosition({lx, ly});
        window.draw(title);
        ly += 20.f;

        for (size_t i=0;i<user_cars.size();i++) {
            sf::CircleShape dot(5.f);
            dot.setOrigin({5.f,5.f});
            dot.setPosition({lx + 8.f, ly + 7.f});
            dot.setFillColor(user_cars[i].color);
            window.draw(dot);

            std::string label = "Car " + std::to_string(i+1);
            if (user_cars[i].source_name.empty() && user_cars[i].dest_name.empty()) label += " (Not configured)";
            else if (user_cars[i].is_spawned) label += " (Active)";

            sf::Text lt(*ui_font, label, 11);
            lt.setFillColor(sf::Color(200,200,200));
            lt.setPosition({lx + 22.f, ly});
            window.draw(lt);
            ly += 16.f;
        }
    }
}

void UXController::draw_highlight_rings()
{
    float t = animation_clock.getElapsedTime().asSeconds();
    float pulse = 1.0f + 0.15f * std::sin(t * 3.5f);

    // draw in world coordinates (map view)
    window.setView(camera_controller.get_camera());

    for (size_t i = 0; i < user_cars.size(); ++i)
    {
        const UserCar &uc = user_cars[i];
        if (uc.source_junction)
        {
            sf::CircleShape ring(22.f * pulse);
            ring.setOrigin({ring.getRadius(), ring.getRadius()});
            ring.setPosition(uc.source_junction->get_location());
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineThickness(4.f);
            ring.setOutlineColor(uc.color);
            window.draw(ring);
        }
        if (uc.dest_junction)
        {
            sf::CircleShape ring2(26.f * pulse);
            ring2.setOrigin({ring2.getRadius(), ring2.getRadius()});
            ring2.setPosition(uc.dest_junction->get_location());
            ring2.setFillColor(sf::Color::Transparent);
            ring2.setOutlineThickness(4.f);
            sf::Color dest_col = uc.color;
            dest_col.a = 230;
            ring2.setOutlineColor(dest_col);
            window.draw(ring2);
        }
    }

    // restore UI view
    window.setView(window.getDefaultView());
}

void UXController::render_ui()
{
    if (!ui_text) return;

    draw_top_status_bar();
    draw_left_buttons();
    draw_right_hud();

    draw_highlight_rings();

    // bottom-left instruction text
    std::string txt = "Left Click: Select junctions via buttons | Right Click: Reset selections | ESC: Cancel";
    ui_text->setString(txt);
    ui_text->setCharacterSize(13);
    ui_text->setFillColor(sf::Color(220, 220, 220));
    ui_text->setPosition({12.f, 40.f});
    window.draw(*ui_text);
}