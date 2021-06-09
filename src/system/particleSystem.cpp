#include "particleSystem.hpp"
#include "../core/game.hpp"
#include <iostream>

ParticleSystem::ParticleSystem(float speed)
        : killSpeed(speed) {
}

vec2 Clamp(vec2 ori, float fixedLength) {
    float v_length = sqrt(ori.x * ori.x + ori.y * ori.y);

    float scale = v_length / fixedLength;

    return {(float) (ori.x / scale), (float) (ori.y / scale)};
}

float get_mass(Position position) {
    return pow(position.scale.y, 2);
}

void perfect_elastic_collision(entt::entity entity_i, entt::entity entity_j) {
    auto&&[position_i, motion_i] = m_registry.get<Position, Motion>(entity_i);
    auto&&[position_j, motion_j] = m_registry.get<Position, Motion>(entity_j);
    float mass_i = get_mass(position_i);
    float mass_j = get_mass(position_j);
    float mass_sum = mass_i + mass_j;
    motion_i.velocity = motion_i.velocity -
                        (position_i.position - position_j.position)
                        * (2 * mass_j) / mass_sum
                        * glm::dot(motion_i.velocity - motion_j.velocity, position_i.position - position_j.position)
                        / (float) pow(glm::distance(position_i.position, position_j.position), 2);
    motion_j.velocity = motion_j.velocity +
                        (position_j.position - position_i.position)
                        * (2 * mass_i) / mass_sum
                        * glm::dot(motion_j.velocity - motion_i.velocity, position_j.position - position_i.position)
                        / (float) pow(glm::distance(position_j.position, position_i.position), 2);
    position_i.position = position_j.position + glm::normalize(position_i.position - position_j.position) *
                                                ((position_i.scale.y + position_j.scale.y) / 2 + 1);
}

bool bounces(const Position &position1, const Position &position2) {
    return glm::distance(position1.position, position2.position) <= position1.scale.y / 2 + position2.scale.y / 2;
}

float gravitational_constant = 0.1;

vec2 getGravitationalAcceleration(Position position) {
    vec2 acceleration = vec2(0, 0);
    for (auto&&[entity, unit_property, unit_position]: m_registry.view<UnitProperty, Position>().each()) {
        vec2 difference = unit_position.position - position.position;
        float distance = glm::length(difference);
        acceleration +=
                glm::normalize(difference) * (float) (gravitational_constant * unit_property.hp / pow(distance, 1));
    }
    return acceleration;
}

vec2 get_location_after_rotation(vec2 location, float angle){
    return vec2(location.x * cos(angle) - location.y * sin(angle), location.y * cos(angle) + location.x * sin(angle));
}

Position get_mesh_bounding_box(entt::entity entity){
    auto &&[meshRef, position] = m_registry.get<ShadedMeshRef, Position>(entity);
    float xMin = 99999, xMax = 0, yMin = 99999, yMax = 0;
    for(auto vertex : meshRef.reference_to_cache->mesh.vertices){
        vec2 vertex_position = get_location_after_rotation(vec2(vertex.position.x, vertex.position.y)*position.scale, position.angle)+position.position;
        xMin = min(vertex_position.x, xMin);
        xMax = max(vertex_position.x, xMax);
        yMin = min(vertex_position.y, yMin);
        yMax = max(vertex_position.y, yMax);
    }
    Position result = {
            .position =  vec2((xMax+xMin)/2, (yMax+yMin)/2),
            .angle = position.angle,
            .scale = vec2(xMax-xMin, yMax-yMin)
    };
    return result;
}

float ParticleSystem::max_distance = 30.f;
float ParticleSystem::start_explosion_time = -10000.f;
bool ParticleSystem::meteor_field = false;


void ParticleSystem::update() {
    for (auto &cur: m_registry.view<Particle>()) {

        vec2 acceleration = {0, 0};

        auto &p = m_registry.get<Particle>(cur);
        p.life -= killSpeed; // reduce life
//		p.life -= 0.1f; // reduce life

        if (p.life > 0.0f)
        {    // particle is alive, thus update
			auto &curpos = m_registry.get<Position>(cur);
			auto &curmot = m_registry.get<Motion>(cur);
            if (gravity_toggle) {
                acceleration += getGravitationalAcceleration(curpos);
            }

			if (p.life < 0.99f && swarm_behavior_toggle)
			{

                int count = 0;
                vec2 avgSpeed = {0, 0};
                vec2 avgPos = {0, 0};

                for (auto &entity : m_registry.view<Particle>()) {
                    auto &np = m_registry.get<Particle>(entity);
                    auto &pos = m_registry.get<Position>(entity);
                    auto &mot = m_registry.get<Motion>(entity);


                    float distance = sqrt((pos.position.x - curpos.position.x) * (pos.position.x - curpos.position.x) +
                                          (pos.position.y - curpos.position.y) * (pos.position.y - curpos.position.y));

                    if (np.life < 0.99f && entity != cur && distance < ParticleSystem::max_distance) {
                        avgSpeed = {avgSpeed.x + mot.velocity.x, avgSpeed.y + mot.velocity.y};
                        avgPos = {avgPos.x + pos.position.x, avgPos.y + pos.position.y};
                        count++;
                    }
                }
                if (count > 0) {

                    avgSpeed = {avgSpeed.x / count, avgSpeed.y / count};

                    avgSpeed = Clamp(avgSpeed, 1);

                    acceleration += vec2(avgSpeed.x - curmot.velocity.x, avgSpeed.y - curmot.velocity.y);

                    acceleration = Clamp(acceleration, FIXED_SPEED);
                }
            }

            curmot.velocity = {curmot.velocity.x + acceleration.x, curmot.velocity.y + acceleration.y};
            if (swarm_behavior_toggle) {
                curmot.velocity = Clamp(curmot.velocity, 5);
            }

            curpos.position += curmot.velocity;
            if(is_precise_collision){
                Position precise_bounding_box = get_mesh_bounding_box(cur);
                vec2 size = precise_bounding_box.scale;
                if((precise_bounding_box.position.y < size.y/2 && curmot.velocity.y<0) ||
                   (precise_bounding_box.position.y > window_size_in_game_units.y - size.y/2 && curmot.velocity.y>0)||
                   (precise_bounding_box.position.x < size.x/2 && curmot.velocity.x<0)||
                   (precise_bounding_box.position.x > tile_matrix_dimension.x*tile_size.x - size.x/2 && curmot.velocity.x>0)){
                    curmot.velocity.x *= -0.2;
                    curmot.velocity.y *= -0.2;
                    if(curpos.position.y < size.y/2) curpos.position.y = size.y/2;
                    else if(curpos.position.y > window_size_in_game_units.y - size.y/2) curpos.position.y = window_size_in_game_units.y - size.y/2;
                    if(curpos.position.x < size.x/2) curpos.position.x = size.x/2;
                    else if(curpos.position.x > window_size_in_game_units.x - size.x/2) curpos.position.x = window_size_in_game_units.x - size.x/2;
                }
            }else{
                if (is_out_of_boundary(cur)) {
                    m_registry.destroy(cur);
                }
            }

            p.color.a -= killSpeed * 10.f;
        } else {
            m_registry.destroy(cur);
        }
    }

    if (elastic_collision_toggle) {
        auto entities = m_registry.view<Particle>();
        for (unsigned int i = 0; i < entities.size(); i++) {

            Position &motion_i = m_registry.get<Position>(entities[i]);
            for (unsigned int j = i + 1; j < entities.size(); j++) {

                Position &motion_j = m_registry.get<Position>(entities[j]);
                if (bounces(motion_i, motion_j)) {
                    perfect_elastic_collision(entities[i], entities[j]);
                }

            }

        }
    }


}


void ParticleSystem::emitParticle(vec2 pos, int amount, bool emit_light) {
    for (int i = 0; i < amount; i++) {
        auto entity = m_registry.create();
        std::string key = "particle";
        ShadedMesh &resource = cache_resource(key);
        if (resource.effect.program.resource == 0) {
            // create a procedural circle
            std::vector<ColoredVertex> vertices;
            std::vector<uint16_t> indices;

            constexpr float z = -0.1f;
            constexpr int NUM_TRIANGLES = 62;

            for (int i = 0; i < NUM_TRIANGLES; i++) {
                // Point on the circle
                ColoredVertex v;
                v.position = {0.5f * std::cos(float(PI) * 2.0f * static_cast<float>(i) / NUM_TRIANGLES),
                              0.5f * std::sin(float(PI) * 2.0f * static_cast<float>(i) / NUM_TRIANGLES),
                              z};

                vertices.push_back(v);

                // Point on the circle ahead by on eposition in counter-clockwise direction
                v.position = {0.5f * std::cos(float(PI) * 2.0f * static_cast<float>(i + 1) / NUM_TRIANGLES),
                              0.5f * std::sin(float(PI) * 2.0f * static_cast<float>(i + 1) / NUM_TRIANGLES),
                              z};

                vertices.push_back(v);

                // Circle center
                v.position = {0, 0, z};

                vertices.push_back(v);

                // Indices
                // Note, one could create a mesh with less redundant vertices
                indices.push_back(static_cast<uint16_t>(i * 3 + 0));
                indices.push_back(static_cast<uint16_t>(i * 3 + 1));
                indices.push_back(static_cast<uint16_t>(i * 3 + 2));
            }
            resource.mesh.vertices = vertices;
            resource.mesh.vertex_indices = indices;
            RenderSystem::createColoredMesh(resource, key, true);
        }



        float random1 = ((rand() % 100) - 50) / 10.0f;
        float random2 = ((rand() % 100) - 50) / 10.0f;
        float rColor = 0.5f + ((rand() % 100) / 100.0f);
        float rScale = 10.f + 35.f * (rand() % 20) / 20;

        m_registry.emplace<ShadedMeshRef>(entity, resource);
        auto &motion = m_registry.emplace<Motion>(entity);
        motion.velocity = {random1, random2};
        auto &position = m_registry.emplace<Position>(entity);
        position.position = pos + random1;
        position.angle = 0.f;
        position.scale = {rScale, rScale};

        auto &particle = m_registry.emplace<Particle>(entity);
        particle.color = vec4(rColor, rColor, rColor, 1.0f);
        particle.life = 1.f;
        if(emit_light){
            m_registry.emplace<Light>(entity);
        }
    }
}

