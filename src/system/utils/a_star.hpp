#include "core/common.hpp"
#include "core/registry.hpp"
#include "components/motion.hpp"
#include "components/units.hpp"
#include <iostream>
#include <utility>
#include <vector>
#include <iterator>
#include <queue>
#include <map>

#ifndef A_STAR_H
#define A_STAR_H

class A_Star {
public:
    static int unit_cost;
    static bool path_finding_projectile;
    static bool spline;
    std::vector<std::vector<int >> unit_grid = {};
    std::vector<std::vector<int >> grid = {};
    // update the map as needed this is just filler
    // TODO: update this to use a centralized enum along with battle_screen_world.cpp
    std::map<TileType, int> costs = {
            {TileType::basic,  1},
            {TileType::forest, 99999},
            {TileType::water,  99999}
    };


    // comparator class for priority queue
    class compare {
    public:
        int operator()(const std::pair<int, std::pair<int, int>> &p1, const std::pair<int, std::pair<int, int>> &p2) {
            return p1.first > p2.first;
        }
    };

    // manhattan distance, we can use a different heuristic if there is a preference
    float heuristic(std::pair<int, int> a, std::pair<int, int> b) {
        return abs(a.first - b.first) + abs(a.second - b.second);
    }

    // initialize a 2D vector to represent the grid, where grid[i][j] is the cost of moving to grid[i][j] from an adjacent square
    A_Star(std::pair<int, int> grid_size) {

        this->grid.resize(grid_size.first);
        this->unit_grid.resize(grid_size.first);
        for (unsigned int i = 0; i < grid.size(); i++) {
            this->grid[i].resize(grid_size.second);
            this->unit_grid[i].resize(grid_size.second);
        }
//        TODO pass in grid state from game instead. Not important
        for (auto&&[entity, tile_comp, tile_pos] : m_registry.view<Tile, Position>().each()) {
            ivec2 tile_index = get_tile_index(tile_pos.position);
            this->grid[tile_index.x][tile_index.y] = costs[tile_comp.type];
        }
        for (auto&&[entity, unit_prop, unit_pos] : m_registry.view<UnitProperty, Position>().each()) {
            ivec2 tile_index = get_tile_index(unit_pos.position);
            this->unit_grid[tile_index.x][tile_index.y] = unit_cost;
        }
    }

    // a* search algorithm, start is a the starting grid position, end is the target grid position
    std::vector<std::pair<int, int>> getPath(std::pair<int, int> start, std::pair<int, int> end, bool is_complete_path = false) {
        std::priority_queue<std::pair<int, std::pair<int, int>>, std::vector<std::pair<int, std::pair<int, int>>>, compare> fronteir = {};
        std::map<std::pair<int, int>, int> costSoFar = {};
        std::map<std::pair<int, int>, std::pair<int, int>> cameFrom = {};
        std::vector<std::pair<int, int>> complete_path = {};

        fronteir.push(std::make_pair(0, start));
        costSoFar[start] = 0;
        while (!fronteir.empty()) {
            std::vector<std::pair<int, int>> neighbours;
            std::pair<int, int> current = fronteir.top().second;
            fronteir.pop();
            if (current == end) {
                break;
            }
            // check neighbours, can be modified to add diagonal neighbours in the future
            neighbours.push_back(std::make_pair(current.first - 1, current.second));
            neighbours.push_back(std::make_pair(current.first + 1, current.second));
            neighbours.push_back(std::make_pair(current.first, current.second - 1));
            neighbours.push_back(std::make_pair(current.first, current.second + 1));

            for (auto &neighbour : neighbours) {
                if (neighbour.first >= 0 && neighbour.first < grid.size() && neighbour.second >= 0 &&
                    neighbour.second < grid[0].size()) {
                    int updatedCost = costSoFar[current] + this->grid[neighbour.first][neighbour.second];
                    if(current==start && !is_complete_path){updatedCost+=this->unit_grid[neighbour.first][neighbour.second];}
                    if (!costSoFar.count(neighbour) || updatedCost < costSoFar[neighbour]) {
                        costSoFar[neighbour] = updatedCost;
                        int prioirity = updatedCost + heuristic(neighbour, end);
                        fronteir.push(std::make_pair(prioirity, neighbour));
                        cameFrom[neighbour] = current;
                    }
                }
            }
        }

        std::pair<int, int> current = end;
        while (current != start) {
            complete_path.insert(complete_path.begin(), current);
            current = cameFrom[current];
        }
        if(is_complete_path) return complete_path;

        std::vector<std::pair<int, int>> res_path = {};
        for(auto tile_index: complete_path){
            if(costSoFar[tile_index]>99){
                break;
            }
            res_path.push_back(tile_index);
            break;
        }

        if(!res_path.empty()){
            unit_grid[res_path[0].first][res_path[0].second] = 100;
            unit_grid[start.first][start.second] = 0;
        }else{
            unit_grid[start.first][start.second] = 100;
        }
        return res_path;
    }


};



#endif