//copied from entt pacman
#include "map.hpp"

namespace {
template <std::size_t Size>
MapState stateStr_to_tileType(MapState &state, const char (&str)[Size]) {
  assert(state.area() + 1 == Size);
  for (std::size_t i = 0; i != Size - 1; ++i) {
    state[i] = char_to_tileType(str[i]);
  }
  return state;
}
}

TileType int_to_tileType(const int i) {
    switch (i) {
    case 0: return TileType::basic;
    case 1: return TileType::water;
    case 2: return TileType::forest;
    default:
        assert(false);
        return {};
    }
}

int tileType_to_int(const TileType tt) {
    switch (tt) {
    case TileType::basic: return 0;
    case TileType::water: return 1;
    case TileType::forest: return 2;
    default: 
        assert(false);
        return {};
    }
}

TileType char_to_tileType(const char c) {
    switch (c) {
        case ' ': return TileType::basic;
        case '#': return TileType::forest;
        case '-': return TileType::water;
        default:
            assert(false);
            return {};
    }
}


MapState makeMapState(Level level) {
  MapState state{tile_matrix_dimension};
    const char str[] = {
            "###############"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "###############"
    };
    const char level1[] = {
            "###############"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#         ----#"
            "#             #"
            "#         -   #"
            "###############"
    };
    const char level2[] = {
            "###############"
            "#             #"
            "#########     #"
            "#             #"
            "#     ####### #"
            "#             #"
            "#             #"
            "#  ######     #"
            "#             #"
            "###############"
    };

    const char level3[] = {
            "###############"
            "#             #"
            "###         ###"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#  #       #  #"
            "#  #       #  #"
            "###############"
    };

    const char level4[] = {
            "###############"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "###############"
    };
    const char level5[] = {
            "###############"
            "#             #"
            "#             #"
            "#    ######## #"
            "#             #"
            "#--------     #"
            "#             #"
            "#             #"
            "#             #"
            "###############"
    };
    const char sandbox[] = {
            "###############"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "#             #"
            "###############"
    };
    const char path_finding[] = {
            "###############"
            "               "
            " #             "
            "########### ###"
            "            #  "
            "# ###########  "
            "            #  "
            "######## ####  "
            "#              "
            "###############"
    };
    switch (level) {
        case Level::level1: return stateStr_to_tileType(state, level1);
        case Level::level2: return stateStr_to_tileType(state, level2);
        case Level::level3: return stateStr_to_tileType(state, level3);
        case Level::level4: return stateStr_to_tileType(state, level4);
        case Level::level5: return stateStr_to_tileType(state, level5);
        case Level::sandbox: return stateStr_to_tileType(state, sandbox);
        case Level::path_finding_debug: return stateStr_to_tileType(state, path_finding);
        default:
            assert(false);
            return {};
    }

}


ivec2 get_tile_index(vec2 position){
    return ivec2(floor(position.x / tile_size.x), floor(position.y / tile_size.y));
}

vec2 get_tile_center_from_position(vec2 position){
    vec2 index = get_tile_index(position);
    return index*tile_size+(tile_size/2.f);
};
vec2 get_tile_center_from_index(ivec2 tile_index){
    vec2 index_in_float = tile_index;
    return index_in_float * tile_size + (tile_size / 2.f);
};

bool is_tile_out_of_index(ivec2 tile_index){
    return !(tile_index.x > -1 && tile_index.x < tile_matrix_dimension.x && tile_index.y > -1 && tile_index.y < tile_matrix_dimension.y);
};

char tileType_to_char(TileType tileType){
    switch (tileType) {
        case TileType::basic: return ' ';
        case TileType::forest: return '#';
        case TileType::water: return '-';
        default:
            assert(false);
            return {};
    }
}