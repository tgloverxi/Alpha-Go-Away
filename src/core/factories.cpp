#include "factories.hpp"

namespace {
    float init_loading_screen_location_x = window_size_in_game_units.x/2 - 7 * window_size_in_game_units.x;
    nlohmann::json unit_meshes;
    std::string get_tile_path(TileType tileType) {
        switch (tileType) {
            case TileType::basic:
                return "basic_grid.png";
            case TileType::forest:
                return "tree_grid.png";
            case TileType::water:
                return "river_grid.png";
            default:
                assert(false);
                return {};
        }
    }

    ShadedMesh &create_tile_mesh_resource(TileType tileType) {
        std::string tile_texture_path = get_tile_path(tileType);
        std::string key = tile_texture_path;
        ShadedMesh &resource = cache_resource(key);
        if (resource.effect.program.resource == 0)
            RenderSystem::createSprite(resource, textures_path(tile_texture_path), "textured");
        return resource;
    }


    void init_unit_bounding_box(entt::entity entity, UnitType unitType){
        BoundingBox& boundingBox =m_registry.emplace<BoundingBox>(entity);
        boundingBox.vertices = { vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5) };
        switch (unitType) {
            case UnitType::human_terminator:
                boundingBox.vertices = { vec2(-0.1, -0.2), vec2(0.1, -0.2), vec2(0.2, 0.f), vec2(0.15, 0.3), vec2(-0.2, 0.3)};
                return;
            case UnitType::human_monitor:
                return;
            case UnitType::human_archer:
                return ;
            case UnitType::human_healer:
                boundingBox.vertices = { vec2(-0.15, -0.25), vec2(0.1, -0.25), vec2(0.25, 0.1), vec2(0.25, 0.25), vec2(-0.15, 0.25) };
                return ;
            case UnitType::ai_terminator:
                boundingBox.vertices = { vec2(-0.5, -0.1), vec2(-0.25, -0.5), vec2(0.3, -0.5), vec2(0.3, 0.5),
                                         vec2(-0.25, 0.5), vec2(-0.5, 0.15) };
                return ;
            case UnitType::ai_monitor:
                return ;
            case UnitType::ai_archer:
                boundingBox.vertices = { vec2(-0.5, -0.25), vec2(0.f, -0.5), vec2(0.5, -0.25),
                                         vec2(0.5, 0.25), vec2(0.f, 0.5), vec2(-0.5, 0.25) };
                return ;
            case UnitType::ai_healer:
                return;
            default:
                assert(false);
                return;
        }
    }

    vec2 get_unit_scale(ShadedMesh &resource){
        vec2 scale = tile_size;
        float aspect_ratio = resource.mesh.original_size.x / resource.mesh.original_size.y;
        if(aspect_ratio>0) scale.y = tile_size.x/aspect_ratio;
        else scale.x = tile_size.y*aspect_ratio;
        return scale;
    }

    UnitProperty get_unit_property(UnitType unitType, UnitProperty& property){
        switch (unitType) {
            case UnitType::human_terminator:
                property.attackRange = 1;
                property.damage = 10;
                property.hp = 200;
                property.maxhp = 200;
                property.close_combat_damage_modifier = 1.f;
                property.is_human = true;
                return property;
            case UnitType::human_monitor:
                property.attackRange = 2;
                property.damage = 15;
                property.is_human = true;
                property.hp = 1010;
                property.maxhp = 1010;
                property.close_combat_damage_modifier = .8f;
                return property;
            case UnitType::human_archer:
                property.attackRange = 5;
                property.damage = 35;
                property.hp = 75;
                property.maxhp = 75;
                property.close_combat_damage_modifier = .1f;
                property.is_human = true;
                return property ;
            case UnitType::human_healer:
                property.attackRange = 3;
                property.is_human = true;
                property.damage = 15;
                property.hp = 180;
                property.maxhp = 180;
                property.close_combat_damage_modifier = .5f;
                return property ;
            case UnitType::ai_terminator:
                property.attackRange = 1;
                property.damage = 12;
                property.hp = 190;
                property.maxhp = 190;
                property.close_combat_damage_modifier = 1.f;
                return property ;
            case UnitType::ai_monitor:
                property.attackRange = 2;
                property.damage = 13;
                property.hp = 1020;
                property.maxhp = 1020;
                property.close_combat_damage_modifier = .8f;
                return property ;
            case UnitType::ai_archer:
                property.attackRange = 5;
                property.damage = 32;
                property.hp = 70;
                property.maxhp = 70;
                property.close_combat_damage_modifier = .1f;
                return property ;
            case UnitType::ai_healer:
                property.attackRange = 3;
                property.damage = 16;
                property.hp = 190;
                property.maxhp = 190;
                property.close_combat_damage_modifier = .5f;
                return property;
            default:
                assert(false);
                return property;
        }
    }

    void init_unit(entt::entity entity, ShadedMesh &resource, vec2 pos, UnitType unitType){
        auto &motion = m_registry.emplace<Motion>(entity);
        motion.velocity = {0.f, 0.f};
        auto &position = m_registry.emplace<Position>(entity);
        position.position = pos;
        position.angle = 0.f;
        position.scale = get_unit_scale(resource);
        UnitProperty &property = m_registry.emplace<UnitProperty>(entity);
        property.unit_type = unitType;
        get_unit_property(unitType, property);
        // different unit type will have difference properties
        m_registry.emplace<Stand>(entity);
    }

    void init_unit_flag_components(entt::entity entity, UnitType unitType){
        switch (unitType) {
            case UnitType::human_terminator:
                m_registry.emplace<Terminator>(entity);
                m_registry.emplace<Ally>(entity);
                return;
            case UnitType::human_monitor:
                m_registry.emplace<Monitor>(entity);
                m_registry.emplace<Ally>(entity);
                return;
            case UnitType::human_archer:
                m_registry.emplace<Archer>(entity);
                m_registry.emplace<Ally>(entity);
                return ;
            case UnitType::human_healer:
                m_registry.emplace<Healer>(entity);
                m_registry.emplace<Ally>(entity);
                return ;
            case UnitType::ai_terminator:
                m_registry.emplace<Terminator>(entity);
                m_registry.emplace<Enemy>(entity);
                return ;
            case UnitType::ai_monitor:
                m_registry.emplace<Monitor>(entity);
                m_registry.emplace<Enemy>(entity);
                return ;
            case UnitType::ai_archer:
                m_registry.emplace<Archer>(entity);
                m_registry.emplace<Enemy>(entity);
                return ;
            case UnitType::ai_healer:
                m_registry.emplace<Healer>(entity);
                m_registry.emplace<Enemy>(entity);
                return;
            default:
                assert(false);
                return;
        }
    }

    ShadedMesh& create_ui_mesh(std::string screen_texture_path, std::string shader = "textured"){
        std::string key = screen_texture_path;
        ShadedMesh &resource = cache_resource(key);
        if (resource.effect.program.resource == 0)
            RenderSystem::createSprite(resource, textures_path(screen_texture_path), "textured");
        return resource;
    }
}

void init_factories(){
    init_unit_resources();
};


entt::entity tile_factory(vec2 pos, TileType tileType) {
    auto entity = m_registry.create();
    ShadedMesh &resource = create_tile_mesh_resource(tileType);
    m_registry.emplace<ShadedMeshRef>(entity, resource);
    auto &position = m_registry.emplace<Position>(entity);
    position.position = pos;
    position.angle = 0.f;
    position.scale = tile_size;
    auto &tile_comp = m_registry.emplace<Tile>(entity);
    tile_comp.type = tileType;

    return entity;
};

entt::entity unit_factory(vec2 pos, UnitType unitType) {
    auto entity = m_registry.create();
    ShadedMesh &resource = create_unit_mesh_resource(unitType, "stand");
    m_registry.emplace<ShadedMeshRef>(entity, resource);
    init_unit_flag_components(entity, unitType);
    init_unit_bounding_box(entity, unitType);
    init_unit(entity, resource, pos, unitType);
    if (m_registry.has<Ally>(entity)) {
        m_registry.emplace<KeyFrameMotion>(entity);
        init_key_frames(entity);
    }
    return entity;
};

ShadedMesh& create_projectile_mesh(std::string screen_texture_path, std::string shader = "textured"){
    std::string key = screen_texture_path ;
    ShadedMesh &resource = cache_resource(key);
    if (resource.effect.program.resource == 0) RenderSystem::createSprite(resource, textures_path(screen_texture_path), "textured");
    return resource;
}

entt::entity projectile_factory(entt::entity unit, UnitType unitType, entt::entity target){
    auto entity = m_registry.create();

        switch (unitType) {
            case UnitType::human_monitor:

                m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("human_tank_pro.png", "textured"));
                m_registry.emplace<AllyProjectile>(entity);

                break;
            case UnitType::human_archer:
                m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("human_archer_pro.png", "textured"));
                m_registry.emplace<AllyProjectile>(entity);


                break;
            case UnitType::human_healer:
                m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("human-healer_pro.png", "textured"));
                m_registry.emplace<AllyProjectile>(entity);

                break;

            case UnitType::ai_monitor:
                m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("ai_tank_pro.png", "textured"));
                m_registry.emplace<EnemyProjectile>(entity);

                break;
            case UnitType::ai_archer:
                m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("ai_archer_pro.png", "textured"));
                m_registry.emplace<EnemyProjectile>(entity);

                break;
            case UnitType::ai_healer:
                m_registry.emplace<ShadedMeshRef>(entity, create_projectile_mesh("ai_healer_pro.png", "textured"));

                //RenderSystem::createSprite(resource, textures_path("ai_healer_pro.png"), "textured");
                m_registry.emplace<EnemyProjectile>(entity);
                break;
            case UnitType::human_terminator:
                return entt::null;
            case UnitType::ai_terminator:
                return entt::null;
            default:
                assert(false);
                break;
        }

    auto&&[target_position, target_motion] = m_registry.get<Position,Motion>(target);
    auto& position = m_registry.emplace<Position>(entity);
    auto&& [unit_pos, unit_prop] = m_registry.get<Position, UnitProperty>(unit);
    position.position = unit_pos.position;
    vec2 dir = glm::normalize( target_position.position-unit_pos.position);
    position.angle = atan2(dir.y, dir.x);
    position.scale = tile_size/2.f;
    auto& projectile_prop = m_registry.emplace<ProjectileProperty>(entity);
    projectile_prop.damage = unit_prop.damage;
//    if(m_registry.has<Projectiles>(unit)){
//        auto &projectile = m_registry.get<Projectiles>(unit);
//
//        projectile.pro.push_back(entity);
//    } else {
//        auto &projectile = m_registry.emplace<Projectiles>(unit);
//        projectile.pro.push_back(entity);
//    }
    int x_0, x_3;
    int y_0 = unit_pos.position.y + rand() % 30;
    int y_3 = target_position.position.y + rand() % 60;
    if (unit_pos.position.x > target_position.position.x) {
        x_0 = unit_pos.position.x + rand() % 20;
        x_3 = target_position.position.x - rand() % 60;
    } else {
        x_0 = unit_pos.position.x - rand() % 30;
        x_3 = target_position.position.x + rand() % 60;
    }
    projectile_prop.spoints[0] = {x_0, y_0};
    projectile_prop.spoints[1] = unit_pos.position;
    projectile_prop.spoints[2] = target_position.position;
    projectile_prop.spoints[3] = {x_3, y_3};
    auto& motion = m_registry.emplace<Motion>(entity);
    motion.velocity = glm::normalize(dir) * projectile_speed;
    projectile_prop.actualTarget = target;
    m_registry.emplace<ProjectileTimer>(entity);


    BoundingBox& boundingBox =m_registry.emplace<BoundingBox>(entity);
    boundingBox.vertices = { vec2(-0.5, -0.5), vec2(0.5, -0.5), vec2(0.5, 0.5), vec2(-0.5, 0.5) };
    return entity;
}
entt::entity explosion_factory(vec2 pos) {
    auto entity = m_registry.create();
    std::string key = "explosion";
    ShadedMesh &resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
        RenderSystem::createSprite(resource, textures_path("unit/explosion.png"), "animation_textured");
    resource.number_of_frames = 12;
    m_registry.emplace<ShadedMeshRef>(entity, resource);
    auto& position = m_registry.emplace<Position>(entity);
    position.position = pos;
    position.angle = 0.f;
    position.scale = tile_size;
    m_registry.emplace<Explosion>(entity);
    return entity;
};

entt::entity ui_factory(std::string texture_path, vec2 pos, vec2 size, std::string shader = "textured", float parallax_speed = 0.f){
    auto entity = m_registry.create();
    m_registry.emplace<ShadedMeshRef>(entity, create_ui_mesh(texture_path, shader));
    auto &position = m_registry.emplace<Position>(entity);
    position.position = pos;
    position.angle = 0.f;
    position.scale = size;
    if((texture_path.find("buttons") == std::string::npos) &&
       (texture_path.find("tutorial") == std::string::npos) &&
       (texture_path.find("info") == std::string::npos)){
        auto& sc = m_registry.emplace<ScreenComponent>(entity);
        sc.parallax_speed = parallax_speed;
    }
    if(texture_path.find("buttons") != std::string::npos){
        m_registry.emplace<ButtonComponent>(entity);
    } 
    if(texture_path.find("tutorial") != std::string::npos){
        m_registry.emplace<TutorialComponent>(entity);
    }
    if(texture_path.find("info") != std::string::npos){
        m_registry.emplace<InfoComponent>(entity);
    }
    return entity;
}

void screenUpdate(float frame){
    int bg_num = ((int) floor(frame)) % 15;
    for(auto&& [entity, position]: m_registry.view<Position,LoadingScreen>().each()){
        position.position.x = init_loading_screen_location_x + window_size_in_game_units.x * bg_num;
    }

}

void loading_screen_factory(){

//    ui_factory("buttons/PlayButton.jpg", {map_x_max/2, map_y_max/2 + 70}, {150, 70});
//    ui_factory("buttons/HelpButton.jpg", {map_x_max/2, map_y_max/2 + 160},  {150, 70});
//    ui_factory("buttons/QuitButton.jpg", {map_x_max/2, map_y_max/2+ 250},  {150, 70});
    ui_factory( "gameTitle.png", {window_size_in_game_units.x/2, window_size_in_game_units.y/2- 160}, {360, 200});
    auto loading_entity = ui_factory( "BgScreens/long-loading-screen.png", {init_loading_screen_location_x, window_size_in_game_units.y/2},
                {window_size_in_game_units.x*15, window_size_in_game_units.y});
    m_registry.emplace<LoadingScreen>(loading_entity);
}

void background_factory(){
    ui_factory("bg_stars.png", { window_size_in_game_units.x / 2, window_size_in_game_units.y / 2 }, { window_size_in_game_units.x * 7, window_size_in_game_units.y }, "textured", 2.f);
    ui_factory( "bg.png", {window_size_in_game_units.x / 2, window_size_in_game_units.y/2}, {window_size_in_game_units.x * 7, window_size_in_game_units.y}, "textured", 1.f);
}

void story_factory(int story_num){
    if (story_num == 0) {
        ui_factory("buttons/next.png", next_pos, button_size);
        ui_factory("buttons/skip.png", skip_pos, button_size);
    }
    ui_factory("Story/story" + std::to_string(story_num) + ".png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2},
               {tile_matrix_dimension.x*tile_size.x, tile_matrix_dimension.y*tile_size.y});
    if (story_num == 1) {
        for (int i = 0; i < 4; i++)
            unit_factory({tile_matrix_dimension.x*tile_size.x/2 - 200 + i * 100, tile_matrix_dimension.y*tile_size.y-100}, UnitType::ai_terminator);
    } else if (story_num == 2) {
        unit_factory({tile_matrix_dimension.x*tile_size.x/2 - 200, tile_matrix_dimension.y*tile_size.y-100}, UnitType::human_monitor);
        unit_factory({tile_matrix_dimension.x*tile_size.x/2 - 100, tile_matrix_dimension.y*tile_size.y-100}, UnitType::human_archer);
        unit_factory({tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y-100}, UnitType::human_healer);
        unit_factory({tile_matrix_dimension.x*tile_size.x/2 + 100, tile_matrix_dimension.y*tile_size.y-100}, UnitType::human_terminator);
    }
}

void tutorial_factory(int tutorial_num) {
    ui_factory("tutorial/tutorial" + std::to_string(tutorial_num) + ".png", tutorial_pos, tutorial_size);
    ui_factory("buttons/skip_tutorial.png", skip_t_pos, button_size);
    if (tutorial_num == 2) {
        ui_factory("tutorial/tutorial_click.png", {map_x_min + 80, map_y_min + tile_size.y * 3}, {150, 100});
        ui_factory("tutorial/tutorial_mark.png", {map_x_min, map_y_min + tile_size.y * 3}, {70, 50});
    } else if (tutorial_num == 3) {
        ui_factory("tutorial/tutorial_click.png", {map_x_max - 160, map_y_min + tile_size.y * 5}, {150, 100});
        ui_factory("tutorial/tutorial_mark.png", {map_x_max - 80, map_y_min + tile_size.y * 5}, {70, 50});
    }
}

entt::entity result_factory(bool res, int level_num, int lost_num) {
    auto entity = m_registry.create();
    if (res) {
        if (level_num == 5) {
            m_registry.emplace<ShadedMeshRef>(entity, create_ui_mesh("final_win.png", "textured"));
        } else {
            m_registry.emplace<ShadedMeshRef>(entity, create_ui_mesh("win.png", "textured"));
            ui_factory("buttons/next_level.png", result_button_pos, button_size);
        }
    } else {
        if (lost_num < 3) {
            m_registry.emplace<ShadedMeshRef>(entity, create_ui_mesh("lose.png", "textured"));
        } else {
            m_registry.emplace<ShadedMeshRef>(entity, create_ui_mesh("lose_hint.png", "textured"));
        }
        ui_factory("buttons/replay.png", result_button_pos, button_size);
    }
    auto &position = m_registry.emplace<Position>(entity);
    position.position = {window_size_in_game_units.x/2 - 200, window_size_in_game_units.y/2};
    position.angle = 0.f;
    position.scale = {560, 360};
    m_registry.emplace<resultComponent>(entity);
    return entity;
}

void tile_info_factory(TileType tileType) {
    entt::entity tile = tile_factory ({390, 270}, tileType);
    m_registry.emplace<InfoComponent>(tile);
    m_registry.remove<Tile>(tile);
    auto &position = m_registry.get<Position>(tile);
    position.scale = {100,100};
    switch (tileType) {
        case TileType::basic:
            ui_factory("info/info_basic_tile.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case TileType::forest:
            ui_factory("info/info_forest_tile.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case TileType::water:
            ui_factory("info/info_water_tile.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        default:
            assert(false);
            break;
    }
//    ui_factory("info/info_bg.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
    ui_factory("buttons/info_done.png", done_pos, button_size);
}

void unit_info_factory(UnitType unitType) {
    entt::entity unit = unit_factory ({390, 270}, unitType);
    m_registry.emplace<InfoComponent>(unit);
    m_registry.remove<UnitProperty>(unit);
    auto &position = m_registry.get<Position>(unit);
    position.scale = {120,120};
    switch (unitType) {
        case UnitType::human_terminator:
            ui_factory("info/info_h_terminator.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::human_monitor:
            ui_factory("info/info_h_monitor.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::human_archer:
            ui_factory("info/info_h_archer.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::human_healer:
            ui_factory("info/info_h_healer.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::ai_terminator:
            ui_factory("info/info_a_terminator.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::ai_monitor:
            ui_factory("info/info_a_monitor.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::ai_archer:
            ui_factory("info/info_a_archer.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        case UnitType::ai_healer:
            ui_factory("info/info_a_healer.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
            break;
        default:
            assert(false);
            break;
    }
//    ui_factory("info/info_bg.png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2}, {900, 600});
    ui_factory("buttons/info_done.png", done_pos, button_size);
}

void level_info_factory(int level_num) {
    ui_factory("info/level" + std::to_string(level_num) + ".png", {tile_matrix_dimension.x*tile_size.x/2, tile_matrix_dimension.y*tile_size.y/2},
               {1200, 650});
    ui_factory("buttons/info_done.png", done_pos, button_size);
}

int get_level_num(Level l) {
    switch (l) {
        case Level::level1:
            return 1;
        case Level::level2:
            return 2;
        case Level::level3:
            return 3;
        case Level::level4:
            return 4;
        case Level::level5:
            return 5;
        default:
            return 0;
    }
}

int button_clicked(double x_pos, double y_pos, vec2 b_pos, vec2 b_size) {
    bool x_in_button = (x_pos <= b_pos.x + b_size.x/2) && (x_pos >= b_pos.x - b_size.x/2);
    bool y_in_button = (y_pos <= b_pos.y + b_size.y/2) && (y_pos >= b_pos.y - b_size.y/2);
    return (x_in_button && y_in_button);
}

int info_tile(vec2 pos) {
    bool x = (pos.x == map_x_min);
    bool y = (pos.y <= map_y_min + tile_size.y * 5) && (pos.y >= map_y_min + tile_size.y * 3);
    return (x && y);
}

void swap_tile_texture(entt::entity entity, TileType tileType) {
    auto &tile_comp = m_registry.get<Tile>(entity);
    if (tile_comp.type != tileType) {
        tile_comp.type = tileType;
        ShadedMesh &resource = create_tile_mesh_resource(tileType);
        m_registry.replace<ShadedMeshRef>(entity, resource);
    }
};

GLuint get_tile_texture_id(TileType tileType) {
    ShadedMesh &resource = create_tile_mesh_resource(tileType);
    return resource.texture.texture_id;
};
