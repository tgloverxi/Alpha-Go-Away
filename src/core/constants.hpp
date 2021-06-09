//
// Created by grumpy on 2021-03-04.
//

#ifndef ALPHAGO_AWAY_CONSTANTS_HPP
#define ALPHAGO_AWAY_CONSTANTS_HPP

constexpr ivec2 window_size_in_px = {1600, 800};
constexpr vec2 window_size_in_game_units = { 1600, 800 };
constexpr ivec2 tile_matrix_dimension = {15 , 10};
constexpr vec2 tile_size = {80, 80};
constexpr vec2 next_pos = {tile_matrix_dimension.x*tile_size.x-100, tile_matrix_dimension.y*tile_size.y-70};
constexpr vec2 skip_pos = {tile_matrix_dimension.x*tile_size.x-300, tile_matrix_dimension.y*tile_size.y-70};
constexpr vec2 info_window_pos = {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2};
constexpr vec2 button_size = {150, 100};
constexpr vec2 tutorial_size = {900, 200};
constexpr vec2 info_window_size = {900, 600};
constexpr float map_x_min = tile_size.x/2;
constexpr float map_x_max = tile_matrix_dimension.x*tile_size.x - tile_size.x/2;
constexpr float map_y_min = tile_size.y/2;
constexpr float map_y_max = tile_matrix_dimension.y*tile_size.y- tile_size.y/2;
constexpr vec2 tutorial_pos = {tile_matrix_dimension.x*tile_size.x/2, map_y_min + 8};
constexpr vec2 skip_t_pos = {tile_size.x*2, map_y_max-10};
constexpr vec2 done_pos = {tile_matrix_dimension.x*tile_size.x-160, map_y_max-10};
constexpr vec2 result_button_pos = {window_size_in_game_units.x/2 - 200, map_y_max-10};
constexpr float unit_speed = 100;
constexpr float cool_down_unit = tile_size.x / unit_speed * 1000;
constexpr float projectile_speed = 350;
#endif //ALPHAGO_AWAY_CONSTANTS_HPP
