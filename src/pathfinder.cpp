#include "pathfinder.hpp"
#include "junction.hpp"
#include "road.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>

struct PathfinderNode{
    std::shared_ptr<Junction> junction;
    std::weak_ptr<Junction> parent; // to reconstruct the path later;
    std::weak_ptr<Road> edge_from_parent;   //the road taken to get here
    float g_cost = std::numeric_limits<float>::infinity(); //Actual cost from the start
    float h_cost = 0.f;  //estimated cost to the end;

    float f_cost() const {return g_cost + h_cost;}   //the total score for this node. A* always picks the node with the lowest f_cost.

    //for the priority_queue to compare 2 nodes
    bool operator>(const PathfinderNode& other) const{
        return f_cost() > other.f_cost();
    }
};

//this is good guess for the remaining distance. we calc a simple straight line here.
static float heuristic(const sf::Vector2f& a, const sf::Vector2f& b){
    return std::hypot(a.x-b.x, a.y-b.y);
}
std::deque<std::weak_ptr<Road>> PathFinder::find_path(std::shared_ptr<Junction>start,std::shared_ptr<Junction>end){
    std::priority_queue<PathfinderNode, std::vector<PathfinderNode>, std::greater<PathfinderNode>> open_set;
    std::unordered_map<std::shared_ptr<Junction>, PathfinderNode> all_nodes;

    PathfinderNode start_node{
        start, {}, {}, 0.f, heuristic(start->get_location(), end->get_location())
    };
    open_set.push(start_node);
    all_nodes[start] = start_node;

    while(!open_set.empty()){
        PathfinderNode current = open_set.top();
        open_set.pop();

        if(current.junction == end){
            std::deque<std::weak_ptr<Road>> path;
            while(current.junction!=start){
                path.push_front(current.edge_from_parent);
                current = all_nodes[current.parent.lock()];
            }
            return path;
        }
        for(const auto& road_weak :  current.junction->get_outgoing_roads()){
            if(auto road = road_weak.lock()){
                auto neighbor_junction = road->getEndJunction().lock();
                if(!neighbor_junction) continue;

                float tentative_g_cost = current.g_cost + road->get_travel_time();
                if(all_nodes.find(neighbor_junction) == all_nodes.end() || tentative_g_cost < all_nodes[neighbor_junction].g_cost){
                    PathfinderNode& neighbor_node = all_nodes[neighbor_junction];
                    neighbor_node.junction = neighbor_junction;
                    neighbor_node.parent = current.junction;
                    neighbor_node.edge_from_parent = road;
                    neighbor_node.g_cost = tentative_g_cost;
                    neighbor_node.h_cost = heuristic(neighbor_junction->get_location(), end->get_location());
                    open_set.push(neighbor_node);
                }
            }
        }
    }
    return {};
}