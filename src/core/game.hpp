#ifndef ALPHAGO_AWAY_GAME_HPP
#define ALPHAGO_AWAY_GAME_HPP
// internal
#include "core/common.hpp"
#include "core/registry.hpp"
#include "core/map.hpp"
#include "core/constants.hpp"
#include "factories.hpp"
#include "gui/gui.hpp"
#include "system/physics.hpp"
#include "logger/debug.hpp"
#include "system/particleSystem.hpp"
#include "system/render_components.hpp"
#include "system/ai.hpp"
#include "entities/unit.hpp"
#include "enum.hpp"
#include "loader.hpp"
#include "system/utils/a_star.hpp";

// stlib
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <sstream>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <utility>
#include <entt/entt.hpp>
#include "components/motion.hpp"
#include <nlohmann/json.hpp>

struct ImguiState{
//    PlaceTileType place_tile_type = PlaceTileType::disabled;
};
// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods]
class Game
{
public:
	// Creates a window
	Game(ivec2 window_size_px);

	// Releases all associated resources
	~Game();


	// restart level
    void restart(Level level= Level::start_screen);

    void restart_without_loading_level(Level level = Level::start_screen);

	// Steps the game ahead by ms milliseconds
	void update(float elapsed_ms, vec2 window_size_in_game_units);

	// Check for collisions
    void handle_collisions();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	// parallax offset
	float parallax_offset;

	static bool shake;

	static float timeleft;

	// OpenGL window handle
	GLFWwindow* window;

	void init_level();
    
    ParticleSystem *particles;

    bool should_place = false;


private:
	// Input callback functions
    vec2 get_cursor_position();
    ivec2 get_window_size();
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_pos);
    void on_mouse_click(int button, int action, int mods);
    void sandbox_on_click(int button, int action, int mods);
    void init_map_grid();
    GameMode game_mode = GameMode::free_mode;
    int gold[2] = {0, 0};
    int number_of_entity_flash_light = 20;
    int number_of_shader_flash_light = 1;

    bool show_not_enough_gold_message;
    int player_index = 0;
    // Loads the audio
    void init_audio();

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// Game state
	float current_speed;
	
	// music references
	Mix_Music* background_music;
    Mix_Chunk* win_sound;
    Mix_Chunk* lose_sound;
    Mix_Chunk* battle_sound;
   Mix_Chunk* dead_sound;

    bool has_battle_started;
    float battle_start_in;
    bool battle_over = false;
    entt::entity battle_result;
    
    float frame = 1.f;
    int story_page = 0;
    int tutorial_num = 0;
    int lost_num = 0;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	MapState mapState;
    UnitMapState unitMapState;
    UnitHPMapState unitHPState;
    Level level = Level::sandbox;
    int level_res = 0;
    Loader loader = Loader();
    bool is_paused = true;
    bool show_imgui = true;

    void imgui();
    void imgui_level_selection_menu();

    int imgui_entity_selection = 0;
    void imgui_sandbox_menu();

    TileType imgui_entity_selection_to_tileType();

    void imgui_save_sandbox_level();

    void imgui_load_sandbox_level();

    void load_grid(std::string);

    void imgui_tile_menu();
    void imgui_particle_menu();

    void imgui_ally_menu();

    void imgui_enemy_menu();
    
    void imgui_tutorial_menu();

    UnitType imgui_entity_selection_to_unitType();

    void init_unit_grid();

    void imgui_battle_control_menu();

    void place_an_ally(ivec2 tile_index);

    void place_an_enemy(ivec2 tile_index);

    void level_on_click(int button, int action, int mods);
    
    void story_on_click(int button, int action, int mods);
    
    void tutorial_on_click(int button, int action, int mods);
    
    void info_on_click(int button, int action, int mods);
    
    void result_on_click(int button, int action, int mods);

	void update_camera_pos(float time);

    void imgui_game_mode();

    void init_gold(ivec2);

    std::map<UnitType, int> unit_cost = {
            {UnitType::human_terminator,  100},
            {UnitType::human_archer, 200},
            {UnitType::human_monitor,  150},
            {UnitType::human_healer,  120},
    };

    void imgui_story();
    void imgui_save_menu();

    void map_on_click(int button, int action, int mods);

    void init_dark_mode();

    void imgui_flash_light_menu();

    void imgui_camera_control_menu();

    void path_finding_menu();

    void imgui_projectile_menu();
};

#endif //ALPHAGO_AWAY_GAME_HPP
