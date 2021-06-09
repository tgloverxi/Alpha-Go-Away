#ifndef CORE_UNITMAP_HPP
#define CORE_UNITMAP_HPP

#include <cstdint>
#include "utils/grid.hpp"
#include "constants.hpp"
#include "core/enum.hpp"
enum UnitType {
    human_terminator,
    human_monitor,
    human_archer,
    human_healer,
    ai_terminator,
    ai_monitor,
    ai_archer,
    ai_healer,
//    TODO: remove empty
    empty,
};
static const std::string get_unit_mesh_key[] =
        {
                "human_terminator",
                "human_monitor",
                "human_archer",
                "human_healer",
                "ai_terminator",
                "ai_monitor",
                "ai_archer",
                "ai_healer",
        };

using UnitMapState = Grid<UnitType>;
using UnitHPMapState = Grid<int>;
UnitMapState makeUnitState(Level level);
UnitType char_to_unitType(const char c);
UnitType int_to_unitType(int i);
int unitType_to_int(UnitType unit);
#endif