// Header
#include "game.hpp"
#include <tuple>

// Game configuration
const size_t TURTLE_DELAY_MS = 2000;

bool Game::shake = false;
float Game::timeleft = 500.f;

// Create the fish world
// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
Game::Game(ivec2 window_size_px) :
        points(0) {

    // Seeding rng with random device
    rng = std::default_random_engine(std::random_device()());

    ///////////////////////////////////////
    // Initialize GLFW
    auto glfw_err_callback = [](int error, const char *desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
    glfwSetErrorCallback(glfw_err_callback);
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    //-------------------------------------------------------------------------
    // GLFW / OGL Initialization, needs to be set before glfwCreateWindow
    // Core Opengl 3.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, 0);
    // Create the main window (for rendering, keyboard, and mouse input)
    window = glfwCreateWindow(window_size_px.x, window_size_px.y, "AlphaGo-Away", nullptr, nullptr);
    if (window == nullptr)
        throw std::runtime_error("Failed to glfwCreateWindow");

    // Setting callbacks to member functions (that's why the redirect is needed)
    // Input is handled using GLFW, for more info see
    // http://www.glfw.org/docs/latest/input_guide.html
    glfwSetWindowUserPointer(window, this);

    auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) {
        ((Game *) glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3);
    };
    auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) {
        ((Game *) glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1});
    };
    auto cursor_click_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2) {
        ((Game *) glfwGetWindowUserPointer(wnd))->on_mouse_click(_0, _1, _2);
    };
    glfwSetKeyCallback(window, key_redirect);
    glfwSetCursorPosCallback(window, cursor_pos_redirect);
    glfwSetMouseButtonCallback(window, cursor_click_redirect);

    // Playing background music indefinitely
    init_audio();
    Mix_PlayMusic(background_music, -1);
    Mix_VolumeMusic(20);
    std::cout << "Loaded music\n";
}


Game::~Game() {
    // Destroy music components
    if (background_music != nullptr)
        Mix_FreeMusic(background_music);
    if (dead_sound != nullptr)
        Mix_FreeChunk(dead_sound);
    if (win_sound != nullptr)
        Mix_FreeChunk(win_sound);
    if (lose_sound != nullptr)
        Mix_FreeChunk(lose_sound);
    if (battle_sound != nullptr)
        Mix_FreeChunk(battle_sound);
    Mix_CloseAudio();

    // Close the window
    glfwDestroyWindow(window);
    glfwTerminate();
    SDL_Quit();
}

void Game::init_audio() {

    //////////////////////////////////////
    // Loading music and sounds with SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        throw std::runtime_error("Failed to initialize SDL Audio");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
        throw std::runtime_error("Failed to open audio device");

    background_music = Mix_LoadMUS(audio_path("background1.wav").c_str());
    dead_sound = Mix_LoadWAV(audio_path("death.wav").c_str());
    win_sound = Mix_LoadWAV(audio_path("win.wav").c_str());
    lose_sound = Mix_LoadWAV(audio_path("lose.wav").c_str());
    battle_sound = Mix_LoadWAV(audio_path("battle.wav").c_str());
    Mix_VolumeMusic(20);


    if (background_music == nullptr || dead_sound == nullptr || win_sound == nullptr || lose_sound == nullptr || battle_sound == nullptr)
        throw std::runtime_error("Failed to load sounds make sure the data directory is present: " +
                                 audio_path("background1.wav")+
                                 audio_path("death.wav")+
                                 audio_path("win.wav")+
                                 audio_path("lose.wav")+
                                 audio_path("battle.wav"));

}

// Update our game world
void Game::update(float elapsed_ms, vec2 window_size_in_game_units) {
    if (this->level == Level::start_screen) {
        frame += 0.1;
        screenUpdate(frame);
    }
    if (has_battle_started) {
        ai_update(elapsed_ms);
        Mix_HaltMusic();
        Mix_PlayChannel(-1, battle_sound, -1);
        Mix_Volume(-1,2);

        if (battle_start_in > 0) {
            battle_start_in -= elapsed_ms;
        } else {
            physics_update(elapsed_ms);
        }
        if (!battle_over) {
            if (m_registry.view<Ally>().size() == 0) {
                Mix_HaltChannel(-1);
                Mix_PlayChannel(-1, lose_sound, 0);
                lost_num++;
                battle_result = result_factory(false, get_level_num(level), lost_num);
                battle_over = true;
                std::cout << "human fails!!!" << std::endl;
            } else if (m_registry.view<Enemy>().size() == 0) {
                Mix_HaltChannel(-1);
                Mix_PlayChannel(-1, win_sound, 0);
                battle_result = result_factory(true, get_level_num(level), lost_num);
                battle_over = true;
                level_res = 1;
                std::cout << "ai fails!!!" << std::endl;
            }
        }
    }
    if (!has_battle_started && this->level != Level::start_screen) {
        // update keyframe animation
        physics_update_keyframe(elapsed_ms);
    }
    update_camera_pos(elapsed_ms);
    imgui();

}

// Reset the world state to its initial state
void Game::restart(Level level) {
    Mix_HaltChannel(-1);
    this->level = level;
    battle_over = false;
    level_res = 0;

    for (auto entity : m_registry.view<ShadedMeshRef>()) {
        m_registry.destroy(entity);
    }
    std::cout << "Restarting\n";
    current_speed = 1.f;
    has_battle_started = false;

    if(level == Level::story){
        Mix_PlayMusic(background_music, -1);
        Mix_VolumeMusic(20);
        story_page = 0;
        story_factory(story_page);
        background_factory();
    } else if (level == Level::start_screen){
        Mix_PlayMusic(background_music, -1);
        Mix_VolumeMusic(20);
        frame = 1.f;

        loading_screen_factory();
        background_factory();
        this->level = level;
    }else{
        if (level == Level::tutorial) {
            tutorial_num = 0;
            tutorial_factory(tutorial_num);
        }
        background_factory();
        init_level();
        init_map_grid();
        init_unit_grid();
        init_dark_mode();
    }
}

void Game::restart_without_loading_level(Level level) {
    this->level = level;
    battle_over = false;

    for (auto entity : m_registry.view<ShadedMeshRef>()) {
        m_registry.destroy(entity);
    }
    std::cout << "Restarting\n";
    current_speed = 1.f;
    has_battle_started = false;

    if (level == Level::start_screen) {
        Mix_PlayMusic(background_music, -1);
        Mix_VolumeMusic(20);
        frame = 1.f;
        loading_screen_factory();
        this->level = level;
    }
    else {
        background_factory();
    }
}

void Game::init_dark_mode() {
//    RenderSystem::dark_mode = level == Level::level2 ? 1 : 0;
    RenderSystem::dark_mode = 0;
}

// Compute collisions between entities
void Game::handle_collisions() {
    points;

}

// Should the game be over ?
bool Game::is_over() const {
    return glfwWindowShouldClose(window) > 0;
}

void Game::update_camera_pos(float elapsed_ms) {
	if (timeleft > 0 && shake) timeleft -= elapsed_ms;
	if (timeleft < 0) {
		shake = false;
		timeleft = 500.f;
	}

    if(level!=Level::start_screen){
        ivec2 window_size = get_window_size();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (abs(xpos - window_size.x) <= 20 || xpos <= 20) {
            parallax_offset += 1 * xpos <= 20 ? 1 : -1;
            parallax_offset = ((int) parallax_offset % (int) (window_size.x));
            if (current_speed == 1) {
                for (auto&&[entity, position, screenComponent]: m_registry.view<Position,ScreenComponent>().each()){
                    position.position.x = window_size_in_game_units.x / 2 + screenComponent.parallax_speed * parallax_offset;
                }
            }
        }
    }
}


void Game::on_key(int key, int, int action, int mod) {

    // Resetting game
    if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
//		restart();
    }

//    game menu toggle
    if (action == GLFW_RELEASE && key == GLFW_KEY_D) {
        show_imgui = !show_imgui;
    }

    //place units
    if (action == GLFW_PRESS && key == GLFW_KEY_P)
    {
        should_place = !should_place;
        //physicsSystem->should_pause = should_place;
    }
    // Debugging
    if (key == GLFW_KEY_B)
        DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

    // Control the current speed with `<` `>`
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA) {
        current_speed -= 0.1f;
        std::cout << "Current speed = " << current_speed << std::endl;
    }
    if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD) {
        current_speed += 0.1f;
        std::cout << "Current speed = " << current_speed << std::endl;
    }
    current_speed = std::max(0.f, current_speed);
}


void sandbox_on_key(int key, int, int action, int mod) {

}

vec2 Game::get_cursor_position() {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return vec2(xpos, ypos);
}

ivec2 Game::get_window_size() {
    int winWidth, winHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    return ivec2(winWidth, winHeight);
}

void Game::on_mouse_click(int button, int action, int mods) {
    result_on_click(button, action, mods);
    if (!should_place) {
        info_on_click(button, action, mods);

        if (!has_battle_started &&
            (level == Level::level1 || level == Level::level2 || level == Level::level3 || level == Level::level4 ||
             level == Level::level5)) {
            map_on_click(button, action, mods);
        }
        if(level == Level::sandbox || game_mode == GameMode::free_mode) return sandbox_on_click(button, action, mods);
        if (level == Level::level1 || level == Level::level2 || level == Level::level3 || level == Level::level4 ||
            level == Level::level5) {
            return level_on_click(button, action, mods);
        }
    }
    if (level == Level::tutorial) {
        tutorial_on_click(button, action, mods);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int winWidth, winHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);
        //auto& selected_unit = ECS::registry<Unit>.entities[0];
        if (should_place && !has_battle_started) {
            if (action == GLFW_PRESS) {
                for (auto &entity: m_registry.view<UnitProperty>()) {
                    auto &motion = m_registry.get<Motion>(entity);
                    auto &position = m_registry.get<Position>(entity);
                    auto dis_x = abs(position.position.x - xpos);
                    auto dis_y = abs(position.position.y - ypos);
                    auto &property = m_registry.get<UnitProperty>(entity);
                    if(property.is_human){
                        if (dis_x < tile_size.x/2 && dis_y < tile_size.y/2) {
                            //Propertyed_unit = entity;

                            DebugSystem::in_debug_mode = true;
                            property.selected = true;
                            property.init_pos = position.position;
                            glfwGetCursorPos(window, &xpos, &ypos);
                            position.position.x = xpos;
                            position.position.y = ypos;


                        }

                        if (property.selected) {
                            // Mark the selected unit

                            auto& bb = m_registry.get<BoundingBox>(entity);
                            std::cout << bb.vertices.size() << std::endl;
                            for (auto& vertices : bb.vertices) {
                                auto dotSize = vec2(5.f, 5.f);
                                std::cout << "draw" << std::endl;
                                DebugSystem::createLine(vertices, dotSize);
                            }
                            break;
                        }

                    }

                }
            } else if (action == GLFW_RELEASE) {
                for (auto &entity: m_registry.view<UnitProperty>()) {
                    auto &property = m_registry.get<UnitProperty>(entity);
                    if(property.is_human){
                        if (property.selected) {
                            auto &motion = m_registry.get<Motion>(entity);
                            auto &position = m_registry.get<Position>(entity);
                            int grid_pos_x = 1200;
                            int grid_pos_y = 800;
                            glfwGetCursorPos(window, &xpos, &ypos);
                            auto dis_x = xpos - grid_pos_x;
                            auto dis_y = ypos - grid_pos_y;
                            if (dis_x > tile_size.x/2 || dis_y > tile_size.x/2) {

                                property.selected = false;
                                property.selected_release = true;
                            } else {
                                for (int i = 0; i < tile_matrix_dimension.x; i++) {
                                    float grid_pos_x = tile_size.x/2 + tile_size.x * i;
                                    for (int j = 0; j < tile_matrix_dimension.y; j++) {
                                        float  grid_pos_y = tile_size.y/2 + tile_size.y * j;
                                        glfwGetCursorPos(window, &xpos, &ypos);
                                        dis_x = abs(xpos - grid_pos_x);
                                        dis_y = abs(ypos - grid_pos_y);
                                        if (dis_x < tile_size.x/2 && dis_y < tile_size.x/2) {
                                            position.position.x = grid_pos_x;
                                            position.position.y = grid_pos_y;

                                            DebugSystem::in_debug_mode = false;
                                            property.selected = false;
                                            property.selected_release = true;
                                            break;
                                        }
                                    }
                                }
                            }


                        }
                    }

                }
            } else if (action == GLFW_REPEAT) {
                for (auto &entity: m_registry.view<UnitProperty>()) {
                    auto &property = m_registry.get<UnitProperty>(entity);
                    if(property.is_human){
                        if (property.selected) {
                            auto &motion = m_registry.get<Motion>(entity);
                            auto &position = m_registry.get<Position>(entity);
                            int grid_pos_x = 1200;
                            int grid_pos_y = 800;
                            glfwGetCursorPos(window, &xpos, &ypos);
                            auto dis_x = xpos - grid_pos_x;
                            auto dis_y = ypos - grid_pos_y;
                            if (dis_x > tile_size.x/2 || dis_y > tile_size.x/2) {

                                property.selected = false;
                                property.selected_release = true;
                            } else {
                                for (int i = 0; i < tile_matrix_dimension.x; i++) {
                                    float grid_pos_x = tile_size.x/2 + tile_size.x * i;
                                    for (int j = 0; j < tile_matrix_dimension.y; j++) {
                                        float  grid_pos_y = tile_size.y/2 + tile_size.y * j;
                                        glfwGetCursorPos(window, &xpos, &ypos);
                                        dis_x = abs(xpos - grid_pos_x);
                                        dis_y = abs(ypos - grid_pos_y);
                                        if (dis_x <  tile_size.x/2 && dis_y <  tile_size.y/2) {
                                            position.position.x = grid_pos_x;
                                            position.position.y = grid_pos_y;

                                            DebugSystem::in_debug_mode = false;
                                            //property.selected = false;
                                            property.selected_release = true;
                                            break;
                                        }
                                    }
                                }
                            }


                        }
                    }

                }
            }
            //draw moving trajetory
            for (auto &entity: m_registry.view<UnitProperty>()) {
                auto &property = m_registry.get<UnitProperty>(entity);
                // std::cout << "test" << std::endl;
//                if (property.selected) {
//                    // Mark the selected unit
//                    auto& bb = m_registry.get<BoundingBox>(entity);
//                    for (auto& vertices : bb.transformed_vertices) {
//                        auto dotSize = vec2(5.f, 5.f);
//                        std::cout << "draw" << std::endl;
//                        DebugSystem::createLine(vertices, dotSize);
//                    }
//                }else

                if(property.is_human){
                    if(property.selected){
                        //draw the moving  trajectory
                        auto &motion= m_registry.get<Motion>(entity);
                        auto &position = m_registry.get<Position>(entity);
                        vec2 tri_pos = {(position.position.x-property.init_pos.x)/2+property.init_pos.x, (position.position.y-property.init_pos.y)/2+property.init_pos.y};
                        float x1 = position.position.x-property.init_pos.x;
                        float y1 = position.position.y-property.init_pos.y;
                        // use dot product to calculate the angle

                        vec2 v1 = normalize(vec2({x1, y1}));

                        float angle = acos(dot(v1, {1.f, 0.f}));
                        if (y1 < 0.f) {
                            //clock wise
                            angle *= -1.f;
                        }
                        if (y1==0){

                            DebugSystem::createDirectTri(tri_pos,{x1/2,30},0.f);
                            std::cout << "tri" << std::endl;
                        } else if (x1==0){
                            DebugSystem::createDirectTri(tri_pos,{30,y1/2},M_PI/2*y1/abs(y1));
                            std::cout << "tri" << std::endl;
                        }else {
                            DebugSystem::createDirectTri(tri_pos, {abs((position.position.x-property.init_pos.x)/2),abs((position.position.y-property.init_pos.y)/2)},angle);
                            std::cout << "tri" << std::endl;
                        }
                        property.selected_release = false;
                    }
                }

            }
        }
    }
    if (level == Level::story) return story_on_click(button, action, mods);

}

TileType Game::imgui_entity_selection_to_tileType() {
    switch (imgui_entity_selection) {
        case 1:
            return TileType::basic;
        case 2:
            return TileType::water;
        case 3:
            return TileType::forest;
        default:
            return TileType::basic;
    }
    return TileType::forest;
}

UnitType Game::imgui_entity_selection_to_unitType() {
    switch (imgui_entity_selection) {
        case 4:
            return UnitType::human_terminator;
        case 5:
            return UnitType::human_monitor;
        case 6:
            return UnitType::human_archer;
        case 7:
            return UnitType::human_healer;
        case 8:
            return UnitType::ai_terminator;
        case 9:
            return UnitType::ai_monitor;
        case 10:
            return UnitType::ai_archer;
        case 11:
            return UnitType::ai_healer;
        default:
            return UnitType::human_terminator;
    }
    return UnitType::ai_healer;
}

void Game::place_an_ally(ivec2 tile_index) {
    int cost = unit_cost[imgui_entity_selection_to_unitType()];
    if (imgui_entity_selection < 8 && imgui_entity_selection > 3 && mapState[tile_index] == TileType::basic &&
        unitMapState[tile_index] == UnitType::empty && gold[player_index] >= cost) {
        unitMapState[tile_index] = imgui_entity_selection_to_unitType();
        unit_factory(get_tile_center_from_index(tile_index), imgui_entity_selection_to_unitType());
        gold[player_index] -= cost;
        show_not_enough_gold_message = false;
        particles->emitParticle(get_tile_center_from_index(tile_index), 5);
    } else {
        show_not_enough_gold_message = true;
    }
}

void Game::place_an_enemy(ivec2 tile_index) {
    if (imgui_entity_selection < 12 && imgui_entity_selection > 7 && mapState[tile_index] == TileType::basic &&
        unitMapState[tile_index] == UnitType::empty) {
        unitMapState[tile_index] = imgui_entity_selection_to_unitType();
        unit_factory(get_tile_center_from_index(tile_index), imgui_entity_selection_to_unitType());
    }
}

void Game::sandbox_on_click(int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!has_battle_started) {
            if (imgui_entity_selection > 0) {
                auto cursor_position = get_cursor_position();
                ivec2 tile_index = get_tile_index(cursor_position);
                if (!is_tile_out_of_index(tile_index)) {
//                TODO refactor sandbox_swap_tile
                    if (imgui_entity_selection < 4 && unitMapState[tile_index] == UnitType::empty) {
                        auto entity = get_tile_entity_at_position(cursor_position);
                        swap_tile_texture(entity, imgui_entity_selection_to_tileType());
                        mapState[ivec2(tile_index.x, tile_index.y)] = imgui_entity_selection_to_tileType();
                        //                TODO refactor  sandbox_add_unit
                    } else {
                        place_an_ally(tile_index);
                        place_an_enemy(tile_index);
                        particles->emitParticle(get_tile_center_from_index(tile_index), 5);
                    }
                }
            }
        }
    }
}

void Game::level_on_click(int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!has_battle_started) {
            std::cout << imgui_entity_selection << '\n';
            if (imgui_entity_selection < 8 && imgui_entity_selection > 3) {
                auto cursor_position = get_cursor_position();
                ivec2 tile_index = get_tile_index(cursor_position);
                std::cout << "tile_index" << tile_index.x << tile_index.y << !is_tile_out_of_index(tile_index) << '\n';
                if (!is_tile_out_of_index(tile_index)) {
                    place_an_ally(tile_index);
                }
            }
        }
    }
}

void Game::map_on_click(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        auto cursor_position = get_cursor_position();
        ivec2 tile_index = get_tile_index(cursor_position);
        std::cout << "tile_index" << tile_index.x << tile_index.y << !is_tile_out_of_index(tile_index) << '\n';
        if (!is_tile_out_of_index(tile_index)) {
            if(RenderSystem::flash_light_type == 1 && number_of_entity_flash_light > 0){
                particles->emitParticle(cursor_position, rand() % 5 + 1, true);
                number_of_entity_flash_light--;

            }
            if(RenderSystem::dark_mode && RenderSystem::flash_light_type==2 && number_of_shader_flash_light > 0){
                RenderSystem::set_last_firework_time(get_cursor_position());
                number_of_shader_flash_light --;
            }

        }
    }
}

void Game::story_on_click(int button, int action, int mods){


    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // check if next button is pressed
        bool next_pressed = button_clicked(xpos, ypos, next_pos, button_size);
        // check if skip button is pressed
        bool skip_pressed = button_clicked(xpos, ypos, skip_pos, button_size);
        ///
        if (next_pressed || skip_pressed) {
            for(entt::entity entity: m_registry.view<ScreenComponent>()){
                m_registry.destroy(entity);
            }
            for (entt::entity entity : m_registry.view<ShadedMeshRef, UnitProperty>())
            {
                m_registry.destroy(entity);
            }
            // handle next
            if (next_pressed) {
                story_page++;
                if (story_page <= 3) {
                    story_factory(story_page);
                    background_factory();
                } else {
                    level = Level::tutorial;
                    restart(level);
                }
            // handle skip
            } else if (skip_pressed) {
                level = Level::tutorial;
                restart(level);
            }
        }
        
    }
}

void Game::tutorial_on_click(int button, int action, int mods){
    if(!should_place) {
        level_on_click(button, action, mods);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (button_clicked(xpos, ypos, tutorial_pos, tutorial_size)) {
            for(entt::entity entity: m_registry.view<TutorialComponent>()){
                m_registry.destroy(entity);
            }
            tutorial_num++;
            if (tutorial_num <= 16) {
                tutorial_factory(tutorial_num);
            } else {
                tutorial_factory(16);
            }
        } else if (button_clicked (xpos, ypos, skip_t_pos, button_size)) {
            level = Level::level1;
            restart(level);
        }
        
    }
}

void Game::info_on_click(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // info for tiles
        for (entt::entity entity: m_registry.view<Tile>()){
            auto &position = m_registry.get<Position>(entity);
            if (info_tile(position.position) && button_clicked(xpos, ypos, position.position, position.scale)) {
                auto &tile = m_registry.get<Tile>(entity);
                tile_info_factory(tile.type);
            }
        }
        // info for units
        for (entt::entity entity: m_registry.view<UnitProperty>()){
            auto &position = m_registry.get<Position>(entity);
            if (button_clicked(xpos, ypos, position.position, position.scale)) {
                UnitProperty &property = m_registry.get<UnitProperty>(entity);
                unit_info_factory(property.unit_type);
            }
        }
        if (button_clicked(xpos, ypos, done_pos, button_size)) {
            for(entt::entity entity: m_registry.view<InfoComponent>()){
                m_registry.destroy(entity);
            }
            if ((level == Level::level4)  && (!has_battle_started)) {
                RenderSystem::dark_mode = 1;
            }
        }
    }
    // right click to remove a unit
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        for (entt::entity entity: m_registry.view<UnitProperty, Ally>()){
            auto &position = m_registry.get<Position>(entity);
            if (button_clicked(xpos, ypos, position.position, position.scale)) {
                UnitProperty &property = m_registry.get<UnitProperty>(entity);
                int cost = unit_cost[property.unit_type];
                // after removing the unit, give the gold back
                gold[player_index] += cost;
                m_registry.destroy(entity);
            }
        }
    }
}

void Game::result_on_click(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && battle_over) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (button_clicked(xpos, ypos, result_button_pos, button_size)) {
            if (level_res) {
                switch (level) {
                    case Level::level1:
                        level = Level::level2;
                        break;
                    case Level::level2:
                        level = Level::level3;
                        break;
                    case Level::level3:
                        level = Level::level4;
                        break;
                    case Level::level4:
                        level = Level::level5;
                        break;
                    case Level::level5:
                        return;
                    default:
                        break;
                }
                lost_num = 0;
            }
            restart(level);
        }
    }
}

void Game::on_mouse_move(vec2 mouse_pos) {
    (void) mouse_pos;
}

void Game::init_gold(ivec2 income){
    if(game_mode == GameMode::free_mode){
        gold[0] = 999999999;
        gold[1] = 999999999;
        number_of_entity_flash_light = 99999999;
        number_of_shader_flash_light = 99999999;
    }else{
        gold[0] = income[0];
        gold[1] = income[1];
        number_of_entity_flash_light = 20;
        number_of_shader_flash_light = 35;
    }
}

void Game::init_level() {
    /*if(level == Level::sandbox){
        mapState = loader.load_map(level);
        unitMapState = loader.load_units(level);
        return;
    }
    mapState = makeMapState(level);
    unitMapState = makeUnitState(level);*/
    if ((level == Level::level1 || level == Level::level2 || level == Level::level3
         || level == Level::level4 || level == Level::level5)) {
        level_info_factory(get_level_num(level));
    }

        mapState = loader.initial_map_load(level);
        auto income = loader.get_gold_level_builder(level);
        init_gold(income);
        unitHPState = loader.initial_units_hp_load(level);
        unitMapState = loader.initial_units_load(level);

}


void Game::init_map_grid() {
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        float xpos = tile_size.x / 2 + tile_size.x * i;
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            float ypos = tile_size.y / 2 + tile_size.y * j;
            tile_factory(vec2(xpos, ypos), mapState[ivec2(i, j)]);
        }
    }
}

void Game::init_unit_grid() {
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        float xpos = tile_size.x / 2 + tile_size.x * i;
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            float ypos = tile_size.y / 2 + tile_size.y * j;
            if (unitMapState[ivec2(i, j)] != UnitType::empty) {
                auto unit = unit_factory(vec2(xpos, ypos), unitMapState[ivec2(i, j)]);
                auto property = m_registry.get<UnitProperty>(unit);
                property.hp = unitHPState[ivec2(i, j)];
                particles->emitParticle(vec2(xpos, ypos), 1);
            }
        }
    }
}

void imgui_help_menu() {
    if (ImGui::CollapsingHeader("Help")) {
        ImGui::Text("ABOUT THIS DEMO:");
        ImGui::BulletText("Click ally dropdown and select a unit. Click map to add a unit");
        ImGuiHelpImage("help/place_ally.png");
        ImGui::BulletText("Click sandbox and then ally dropdown and select a unit. Click map to add a unit");
        ImGuiHelpImage("help/place_enemy.png");
       }
}

void Game::imgui_game_mode() {
    if (ImGui::CollapsingHeader("Game Mode")) {
        ImGui::Text("Choose a game mode");
        if (ImGui::Button("story mode")) {
            level = Level::story;
            Mix_PlayMusic(background_music, -1);
            Mix_VolumeMusic(20);
            restart(level);
            game_mode = GameMode::story_mode;
        }
        if (ImGui::Button("free mode")) {
            level = Level::sandbox;
            Mix_PlayMusic(background_music, -1);
            Mix_VolumeMusic(20);
            restart(level);
            game_mode = GameMode::free_mode;
        }


    }
}

void Game::imgui_story() {
    if (ImGui::CollapsingHeader("Story")) {
        if (level == Level::level1) {
            ImGui::Text(
                    "Level 1: Some support robots are charging in a small station. This is the perfect time to destroy them");
        }
        if (level == Level::level2) {
            ImGui::Text(
                    "Level 2: Some terminators are roaming around in south east direction. There are only 4 of them. It's perfect time to take down them and out what alphaGo is up to");
        }
        if (level == Level::level3) {
            ImGui::Text(
                    "Level 3: Alpha go must be angry. 4 wizard like units are sent to hunt you. Find a way to break through");
        }
        if (level == Level::level4) {
            ImGui::Text("Level 4: More enemies are coming. Sky looks grim");
        }
        if (level == Level::level5) {
            ImGui::Text(
                    "Level 5: Scientists just created a virus that can destroy alphago. Capture all enemies and upload the virus");
        }


    }
}

void Game::imgui_level_selection_menu() {
    if ((game_mode != GameMode::story_mode) && ImGui::CollapsingHeader("Select a level")) {
        if (ImGui::Button("Sandbox")) {
            level = Level::sandbox;
            restart(level);
        }
        if (ImGui::Button("level1")) {
            level = Level::level1;
            restart(level);
        }
        if (ImGui::Button("level2")) {
            level = Level::level2;
            restart(level);
        }
        if (ImGui::Button("level3")) {
            level = Level::level3;
            restart(level);
        }
        if (ImGui::Button("level4")) {
            level = Level::level4;
            restart(level);
        }
        if (ImGui::Button("level5")) {
            level = Level::level5;
            restart(level);
        }
        if (ImGui::Button("debug path finding")) {
            level = Level::path_finding_debug;
            restart(level);
        }
        if (ImGui::Button("Start Screen")) {
            level = Level::start_screen;
            restart(level);
        }
    }
};

void Game::imgui_battle_control_menu() {
    if (ImGui::CollapsingHeader("Battle")) {
        if (has_battle_started == false) {
            if (ImGui::Button("Start battle")) {
                has_battle_started = true;
                battle_start_in = cool_down_unit;
                RenderSystem::dark_mode = 0;
                for (auto entity: m_registry.view<Particle>()) {
                    m_registry.destroy(entity);
                }
            };
        }
        if (ImGui::Button("Restart level")) restart(level);
    }
};

void Game::imgui_save_sandbox_level(){
    /*std::string map;
    for(int j = 0; j < tile_matrix_dimension.y; j++){
        for(int i = 0; i< tile_matrix_dimension.x; i++){
            map += tileType_to_char(mapState[ivec2(i,j)]);
        }
    }
    nlohmann::json json;
    json["map"] = map;
    save_json("sandbox_map.json", json);*/
    if (game_mode == GameMode::free_mode) {
        loader.level_builder_map(level, gold);
        loader.level_builder_units(level);
    }
    else {
        loader.save_map(level, gold);
        loader.save_units(level);
    }
}

void Game::imgui_particle_menu() {
    if (game_mode == GameMode::free_mode && ImGui::CollapsingHeader("Particle settings")) {
        ImGui::Checkbox("Swarm behavior", &particles->swarm_behavior_toggle);
        ImGui::Checkbox("Gravitational force", &particles->gravity_toggle);
        ImGui::Checkbox("Elastic collision", &particles->elastic_collision_toggle);
        ImGui::Checkbox("Precise collision", &particles->is_precise_collision);
        ImGui::Checkbox("Dark mode", reinterpret_cast<bool *>(&RenderSystem::dark_mode));
        ImGui::Checkbox("Meteor mode", &ParticleSystem::meteor_field);
        ImGui::SliderFloat("Illumination param", &RenderSystem::illumination_param, 1.f, 150.0f);
        ImGui::SliderFloat("Swam radius param", &ParticleSystem::max_distance, 1.f, 1000.0f);
        if (ImGui::Button("Explosion")) ParticleSystem::start_explosion_time = glfwGetTime() + 2.f;

    }
}

void Game::load_grid(std::string map_string) {
    auto entities = m_registry.view<Tile>();
    for (int i = 0; i < tile_matrix_dimension.x; i++) {
        for (int j = 0; j < tile_matrix_dimension.y; j++) {
            int index = i + j * tile_matrix_dimension.x;
            TileType tileType = char_to_tileType(map_string[index]);
            mapState[ivec2(i, j)] = tileType;
            auto entity = entities[get_entity_index_from_tile_index(i, j)];
            swap_tile_texture(entity, tileType);
        }
    }
}

void Game::imgui_load_sandbox_level(){
    restart_without_loading_level(level);
    if (game_mode == GameMode::free_mode) {
        mapState = loader.initial_map_load(level);
        auto income = loader.get_gold_level_builder(level);
        init_gold(income);
        unitHPState = loader.initial_units_hp_load(level);
        unitMapState = loader.initial_units_load(level);
    }
    else {
        mapState = loader.load_map(level);
        auto income = loader.get_gold(level);
        init_gold(income);
        unitHPState = loader.load_units_hp(level);
        unitMapState = loader.load_units(level);
    }
    init_map_grid();
    init_unit_grid();

}
void Game::imgui_save_menu() {
    if (game_mode == GameMode::free_mode && ImGui::CollapsingHeader("Save and reload")) {
        if (ImGui::Button("Save Level")) imgui_save_sandbox_level();
        if (ImGui::Button("Load Level")) imgui_load_sandbox_level();
    }
};

void Game::imgui_sandbox_menu() {
    if (game_mode == GameMode::free_mode && ImGui::CollapsingHeader("Sandbox")) {

            imgui_tile_menu();
            imgui_enemy_menu();

    }
};

void Game::path_finding_menu() {
    if (game_mode == GameMode::free_mode && ImGui::CollapsingHeader("Pathfinding")) {

        ImGui::SliderInt("A* nearby units cost", &A_Star::unit_cost, 0, 100);

    }
};

void Game::imgui_tile_menu() {
    if (ImGui::CollapsingHeader("Tiles")) {
        ImGui::Text("Choose a tile type and click on map to change tiles");
        ImGui::RadioButton("disabled", &imgui_entity_selection, 0);
        ImGui::RadioButton("basic", &imgui_entity_selection, 1);
        ImGuiImage(get_tile_texture_id(TileType::basic));
        ImGui::RadioButton("water", &imgui_entity_selection, 2);
        ImGuiImage(get_tile_texture_id(TileType::water));
        ImGui::RadioButton("forest", &imgui_entity_selection, 3);
        ImGuiImage(get_tile_texture_id(TileType::forest));
    }
}

void Game::imgui_ally_menu() {
    if (ImGui::CollapsingHeader("Ally")) {
        ImGui::Text("Choose an ally type and click on map to place the unit");

        ImGui::RadioButton("player one", &player_index, 0);
        ImGui::RadioButton("player two", &player_index, 1);
        if(game_mode == GameMode::story_mode){
            ImGui::Text("player one gold: %d", gold[0]);
            ImGui::Text("player two gold: %d", gold[1]);
        } else{
            ImGui::SliderInt("player one gold", &gold[0], 100.f, 2000.0f);
            ImGui::SliderInt("player two gold", &gold[1], 100.f, 2000.0f);
        }



        ImGui::RadioButton("disabled", &imgui_entity_selection, 0);
        ImGui::RadioButton("terminator", &imgui_entity_selection, 4);
        ImGui::Text("cost: %d", unit_cost[UnitType::human_terminator]);
        ImGui::Text("maxhp: 200, damage: 10");
//        ImGuiImage(get_tile_texture_id(TileType::basic));
        ImGui::RadioButton("monitor", &imgui_entity_selection, 5);
        ImGui::Text("cost: %d", unit_cost[UnitType::human_monitor]);
        ImGui::Text("maxhp: 1010, damage: 15");
//        ImGuiImage(get_tile_texture_id(TileType::water));
        ImGui::RadioButton("archer", &imgui_entity_selection, 6);
        ImGui::Text("cost: %d", unit_cost[UnitType::human_archer]);
        ImGui::Text("maxhp: 75, damage: 35");

//        ImGuiImage(get_tile_texture_id(TileType::forest));
        ImGui::RadioButton("healer", &imgui_entity_selection, 7);
        ImGui::Text("cost: %d", unit_cost[UnitType::human_healer]);
        ImGui::Text("maxhp: 180, damage: 15");

//        ImGui::Text("***Press p, then you can drag");
//        ImGui::Text(" to change the unit location ***");
//        ImGuiImage(get_tile_texture_id(TileType::forest));
    }
}

void Game::imgui_flash_light_menu() {
    if ((game_mode == GameMode::free_mode || level == Level::level4) && ImGui::CollapsingHeader("Flash Light")) {
        ImGui::Text("Choose a flash light type");
        ImGui::RadioButton("disabled", &RenderSystem::flash_light_type, 0);
        ImGui::RadioButton("Searching flash light", &RenderSystem::flash_light_type, 1);
        ImGui::RadioButton("Mass flash light", &RenderSystem::flash_light_type, 2);
        ImGui::Text("Number of searching flash light: %d", number_of_entity_flash_light);
        ImGui::Text("Number of mass flash light: %d", number_of_shader_flash_light);
    }
}
void Game::imgui_projectile_menu() {
    if (ImGui::CollapsingHeader("Projectile")) {
        ImGui::Checkbox("spline", &A_Star::spline);
        ImGui::Checkbox("path finding", &A_Star::path_finding_projectile);
    }
}

void imgui_remove_all_units(){
    for(auto entity: m_registry.view<UnitProperty>()){
        m_registry.destroy(entity);
    }
}

void Game::imgui_enemy_menu() {
    if (ImGui::CollapsingHeader("Enemy")) {
        ImGui::Text("Choose an enemy type and click on map to place the unit");
        ImGui::RadioButton("disabled", &imgui_entity_selection, 0);
        ImGui::RadioButton("terminator", &imgui_entity_selection, 8);
//        ImGuiImage(get_tile_texture_id(TileType::basic));
        ImGui::RadioButton("monitor", &imgui_entity_selection, 9);
//        ImGuiImage(get_tile_texture_id(TileType::water));
        ImGui::RadioButton("archer", &imgui_entity_selection, 10);
//        ImGuiImage(get_tile_texture_id(TileType::forest));
        ImGui::RadioButton("healer", &imgui_entity_selection, 11);
//        ImGuiImage(get_tile_texture_id(TileType::forest));
        if(!has_battle_started){
            if (ImGui::Button("Remove all units")) {
                imgui_remove_all_units();
                unitMapState.reset(UnitType::empty);
            }
        }
    }
}
void Game::imgui_camera_control_menu() {
    if (ImGui::CollapsingHeader("Camera")) {
        if (ImGui::Button("Shake")) Game::shake = true;
    }
};

void Game::imgui_tutorial_menu() {
    if (ImGui::Button("Start tutorial")) {
        level = Level::tutorial;
        restart(level);
    }
}



void Game::imgui(){
    if(show_imgui){
        ImGui::Begin("Menu");
        // ImGui::SetCursorScreenPos(ImVec2( 200,  200));
        imgui_battle_control_menu();
        imgui_game_mode();
//        imgui_help_menu();
        imgui_level_selection_menu();
        imgui_story();
        imgui_ally_menu();
        imgui_save_menu();
        imgui_sandbox_menu();
        imgui_flash_light_menu();
        imgui_particle_menu();
        imgui_camera_control_menu();
        path_finding_menu();
        imgui_projectile_menu();
        imgui_tutorial_menu();
        ImGui::End();

    }
}
