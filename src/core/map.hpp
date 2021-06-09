#ifndef CORE_MAP_HPP
#define CORE_MAP_HPP

#include <cstdint>
#include "utils/grid.hpp"
#include "constants.hpp"
#include "enum.hpp"
//#include "factories.hpp"

//Copied from entt pacman

enum TileType{
    basic, water, forest
};

using MapState = Grid<TileType>;
//using UnitMapState = Grid<UnitType>;
TileType int_to_tileType(const int i);
int tileType_to_int(const TileType tt);
TileType char_to_tileType(const char c);
MapState makeMapState(Level level);
//UnitMapState makeUnitState();
char tileType_to_char(TileType tileType);
vec2 get_tile_center_from_position(vec2 position);
vec2 get_tile_center_from_index(ivec2 tile_index);
ivec2 get_tile_index(vec2 position);
bool is_tile_out_of_index(ivec2 index);
#endif
