#pragma once
#include "junction.hpp"
#include<deque>
#include <memory>
#include <vector>

class Road;
class Junction;

class PathFinder{
public:
PathFinder()=default;

std::deque<std::weak_ptr<Road>> find_path(std::shared_ptr<Junction>start,std::shared_ptr<Junction>end );
};