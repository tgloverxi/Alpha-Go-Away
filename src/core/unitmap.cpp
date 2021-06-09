
#include "unitmap.hpp"


namespace {
template <std::size_t Size>
UnitMapState stateStr_to_unitType(UnitMapState &state, const char (&str)[Size]) {
    assert(state.area() + 1 == Size);
    for (std::size_t i = 0; i != Size - 1; ++i) {
        state[i] = char_to_unitType(str[i]);
    }
    return state;
}

}
UnitType int_to_unitType(int i) {
    switch (i) {
    case 0: return UnitType::empty;
    case 1: return UnitType::human_terminator;
    case 2: return UnitType::human_monitor;
    case 3: return UnitType::human_archer;
    case 4: return UnitType::human_healer;
    case 5: return UnitType::ai_terminator;
    case 6: return UnitType::ai_monitor;
    case 7: return UnitType::ai_archer;
    case 8: return UnitType::ai_healer;
    default:
        assert(false);
        return {};
    }
}

int unitType_to_int(UnitType unit) {
    switch (unit) {
    case UnitType::empty: return 0;
    case UnitType::human_terminator: return 1;
    case UnitType::human_monitor: return 2;
    case UnitType::human_archer: return 3;
    case UnitType::human_healer: return 4;
    case UnitType::ai_terminator: return 5;
    case UnitType::ai_monitor: return 6;
    case UnitType::ai_archer: return 7;
    case UnitType::ai_healer: return 8;
    default:
        assert(false);
        return {};
    }
}

UnitType char_to_unitType(const char c) {
    switch (c) {
        case ' ': return UnitType::empty;
        case '1': return UnitType::human_terminator;
        case '2': return UnitType::human_monitor;
        case '3': return UnitType::human_archer;
        case '4': return UnitType::human_healer;
        case '5': return UnitType:: ai_terminator;
        case '6': return UnitType:: ai_monitor;
        case '7': return UnitType:: ai_archer;
        case '8': return UnitType:: ai_healer;
        default:
            assert(false);
            return {};
    }
}


UnitMapState makeUnitState(Level level) {
    UnitMapState state{tile_matrix_dimension};
    const char str[] = {
            "               "
            "               "
            "               "
            "               "
            "               "
            "             5 "
            "             6 "
            "             7 "
            "             8 "
            "               "
    };
    const char level1[] = {
            "               "
            "               "
            "               "
            "               "
            "               "
            "               "
            "               "
            "            88 "
            "            88 "
            "               "
    };
    const char level2[] = {
            "               "
            "               "
            "               "
            "               "
            "               "
            "             5 "
            "             5 "
            "             5 "
            "             5 "
            "               "
    };
    const char level3[] = {
            "               "
            " 7           7 "
            "               "
            "               "
            "               "
            "               "
            "               "
            "               "
            " 7           7 "
            "               "
    };
    const char level4[] = {
            "               "
            " 5             "
            " 5             "
            " 5             "
            " 5             "
            "             5 "
            "             6 "
            "             7 "
            "             8 "
            "               "
    };
    const char level5[] = {
            "               "
            " 5             "
            " 6             "
            " 7             "
            " 8             "
            "             5 "
            "             6 "
            "             7 "
            "             8 "
            "               "
    };
    const char sandbox[] = {
            "               "
            " 1             "
            " 2             "
            " 3             "
            " 4             "
            "             5 "
            "             6 "
            "             7 "
            "             8 "
            "               "
    };
    const char path_finding[] = {
            "               "
            "234            "
            "1              "
            "               "
            "               "
            "             5 "
            "             6 "
            "             7 "
            "             8 "
            "               "
    };
    switch (level) {
        case Level::level1: return stateStr_to_unitType(state, level1);
        case Level::level2: return stateStr_to_unitType(state, level2);
        case Level::level3: return stateStr_to_unitType(state, level3);
        case Level::level4: return stateStr_to_unitType(state, level4);
        case Level::level5: return stateStr_to_unitType(state, level5);
        case Level::sandbox: return stateStr_to_unitType(state, sandbox);
        case Level::path_finding_debug: return stateStr_to_unitType(state, path_finding);
        default:
            assert(false);
            return {};
    }
    return stateStr_to_unitType(state, str);
}