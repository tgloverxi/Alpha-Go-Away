#ifndef ALPHAGO_AWAY_FACTORIES_HPP
#define ALPHAGO_AWAY_FACTORIES_HPP
#include "core/common.hpp"
#include <system/render.hpp>
#include "registry.hpp"
#include "components/units.hpp"
#include "components/tile.hpp"
#include "constants.hpp"
#include "unitmap.hpp"
#include "components/ui.hpp"
#include "entities/unit.hpp"
#include<string>


void init_factories();
entt::entity projectile_factory(entt::entity unit, UnitType unitType, entt::entity target);
entt::entity unit_factory(vec2 pos, UnitType unitType = UnitType::human_terminator);
entt::entity tile_factory(vec2 pos, TileType tileType);
void swap_tile_texture(entt::entity tile, TileType tileType);
GLuint get_tile_texture_id(TileType tileType);
void screenUpdate(float frame);
void loading_screen_factory();
void background_factory();
void story_factory(int story_num);
void tutorial_factory(int tutorial_num);
void tile_info_factory(TileType tileType);
void unit_info_factory(UnitType unitType);
void level_info_factory(int level_num);
int get_level_num(Level l);
entt::entity result_factory(bool res, int level_num, int lost_num);
entt::entity explosion_factory(vec2 pos);
int button_clicked(double x_pos, double y_pos, vec2 button_pos, vec2 button_size);
int info_tile(vec2 pos);
#endif //ALPHAGO_AWAY_FACTORIES_HPP
