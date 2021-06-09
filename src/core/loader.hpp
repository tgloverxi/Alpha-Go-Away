#ifndef LEVEL_LOADER_HPP
#define LEVEL_LOADER_HPP

#include <string>
#include <core/map.hpp>
#include <core/unitmap.hpp>


class Loader {

public:


	MapState load_map(Level level);

	UnitMapState load_units(Level level);

	void save_map(Level level, int gold[2]);

	void save_units(Level level);

	MapState initial_map_load(Level level);

	UnitMapState initial_units_load(Level level);

	void level_builder_map(Level level, int gold[2]);

	void level_builder_units(Level level);

    ivec2 get_gold(Level level);

    ivec2 get_gold_level_builder(Level level);

	UnitHPMapState load_units_hp(Level level);

	UnitHPMapState initial_units_hp_load(Level level);
};

#endif