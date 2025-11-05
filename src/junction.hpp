#pragma once
#include "car.hpp"
#include "traffic_light.hpp"
#include <SFML/System/Time.hpp>
#include <map>
#include <memory>
#include <queue>
#include <vector>

class Road;// Forward declaration

class Junction
{
public:
    explicit Junction(const sf::Vector2f& location);
    ~Junction() = default;

    void add_road(const std::shared_ptr<Road>& road);
    void accept_car(std::unique_ptr<Car> car);
    void update(sf::Time elapsed);
    void draw(sf::RenderWindow& window);

    const sf::Vector2f& get_location() const { return j_position; }
    bool is_blocked() const { return j_is_occupied || !j_car_queue.empty(); }

    // --- MODIFIED ---
    void install_light(sf::Time base_green_duration, sf::Time base_yellow_duration);
    // -----------------

    TrafficLight::State get_light_state_for_road(std::weak_ptr<const Road> road) const;
    const std::vector<std::weak_ptr<Road>>& get_outgoing_roads() const;
    size_t get_incoming_road_count() const { return j_roads_incoming.size(); }

private:
    void handle_car_redirection();

    // --- NEW: Helper function for the new logic ---
    sf::Time calculate_dynamic_green_time(const std::weak_ptr<Road>& road);
    // ----------------------------------------------

    sf::Vector2f j_position;
    std::vector<std::weak_ptr<Road>> j_roads_incoming;
    std::vector<std::weak_ptr<Road>> j_roads_outgoing;
    std::queue<std::unique_ptr<Car>> j_car_queue;
    bool j_is_occupied = false;
    static constexpr float CROSSING_DELAY = 0.5f;// Seconds for a car to cross junction
    float j_crossing_timer = CROSSING_DELAY;
    float j_radius{ 12.f };
    std::vector<TrafficLight> j_lights;

    // --- NEW: State machine logic for the junction ---
    bool j_has_lights = false;
    size_t j_current_green_index = 0;
    TrafficLight::State j_current_cycle_state = TrafficLight::State::Red;
    sf::Time j_cycle_timer;
    sf::Time j_current_green_duration; // The *dynamic* duration for the current green
    sf::Time j_yellow_duration;
    sf::Time j_base_green_duration; // The default green time
    // --------------------------------------------------

    // --- NEW: Tuning constants for dynamic logic ---
    static constexpr int MAX_DENSITY_CAR_COUNT = 15; // "Full" road
    static constexpr float MIN_GREEN_TIME_FACTOR =
        0.5f;// At 0 density, green time is 50% of base
    static constexpr float MAX_GREEN_TIME_FACTOR =
        1.5f;// At max density, green time is 150% of base
    // -----------------------------------------------
};