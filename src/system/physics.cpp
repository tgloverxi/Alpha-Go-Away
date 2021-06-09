// internal
#include "physics.hpp"

#include "logger/debug.hpp"

namespace {
    vec2 get_bounding_box(const Position &position) {
        return {abs(position.scale.x), abs(position.scale.y)};
    }

    bool satHelper(const entt::entity e1, const entt::entity e2) {
//        if(!(m_registry.has<BoundingBox>(e1) && m_registry.has<BoundingBox>(e2))) return false;
        BoundingBox bb1 = m_registry.get<BoundingBox>(e1); // bounding box for first entity
        BoundingBox bb2 = m_registry.get<BoundingBox>(e2); // bounding box for second entity
        if (bb1.vertices.size() <= 0 || bb2.vertices.size() <= 0) { // all unit entities should have a bounding box
            return false;
        }
        for (int i = 0; i < bb1.transformed_vertices.size(); i++) {
            auto current = bb1.transformed_vertices[i];
            auto next = bb1.transformed_vertices[(i + 1) % bb1.transformed_vertices.size()];
            auto edge = next - current;
            vec2 axis = vec2(-edge.y, edge.x);

            auto e1MaxProj = NULL;
            auto e2MaxProj = NULL;
            auto e1MinProj = NULL;
            auto e2MinProj = NULL;
            for (vec2 v : bb1.transformed_vertices) { // project entity 1's bb vertices onto the axis
                auto projection = dot(axis, v);
                if (e1MaxProj == NULL || projection > e1MaxProj) {
                    e1MaxProj = projection;
                }
                if (e1MinProj == NULL || projection < e1MinProj) {
                    e1MinProj = projection;
                }
            }
            for (vec2 v : bb2.transformed_vertices) { // project entity 2's bb vertices onto the axis
                auto projection = dot(axis, v);
                if (e2MaxProj == NULL || projection > e2MaxProj) {
                    e2MaxProj = projection;
                }
                if (e2MinProj == NULL || projection < e2MinProj) {
                    e2MinProj = projection;
                }
            }
            if (e1MaxProj < e2MinProj || e1MinProj > e2MaxProj) {
                return false;
            }
        }
        return true;
    }

    bool collides(const entt::entity e1, const entt::entity e2) {
        bool e1Check = satHelper(e1, e2);
        bool e2Check = satHelper(e2, e1);
        return (e1Check && e2Check);
    }

    void update_velocity_and_facing_dir(entt::entity entity_i, entt::entity entity_j) {
        auto &&[position_i, motion_i, property_i] = m_registry.get<Position, Motion, UnitProperty>(entity_i);
        auto &&[position_j, motion_j] = m_registry.get<Position, Motion>(entity_j);
        if (property_i.path.size()>0) {
            std::pair<int, int> nextStep = property_i.path[0];
            auto next_tile_center = get_tile_center_from_index(ivec2(nextStep.first, nextStep.second));
            if (glm::distance(next_tile_center, position_i.position)<1) {
                property_i.path.erase(property_i.path.begin());
            }
            vec2 next_position = get_tile_center_from_index(ivec2(nextStep.first, nextStep.second));
            motion_i.velocity = glm::normalize(next_position - position_i.position) * unit_speed;
        }else{
            motion_i.velocity *= 0.f;
        }

        if (motion_i.velocity.x != 0) {
            position_i.scale.x = -1 * sign(motion_i.velocity.x) * abs(position_i.scale.x);
        } else if ((position_j.position - position_i.position).x > 0) {
            position_i.scale.x = -abs(position_i.scale.x);
        } else {
            position_i.scale.x = abs(position_i.scale.x);
        }
    }

    void set_transformed_bounding_box(entt::entity entity) {
        if (m_registry.has<BoundingBox, Position>(entity)) {
            auto&&[bb, position] = m_registry.get<BoundingBox, Position>(entity);
            Transform transform;
            transform.translate(position.position);
            transform.rotate(position.angle);
            transform.scale(position.scale);
            std::vector<vec2> temp_vector;
            for (int i = 0; i < bb.vertices.size(); i++) {
                auto transformed = transform.mat * vec3(bb.vertices[i].x, bb.vertices[i].y, 1.f);
                temp_vector.emplace_back(transformed.x, transformed.y);
                bb.transformed_vertices = temp_vector;
            }
        }
    }

    vec2 get_spos(std::vector<vec2> points, float t) {
        int p1 = (int)t + 1;
        int p2 = p1 + 1;
        int p3 = p2 + 1;
        int p0 = p1 - 1;
        
        float tt = pow(t, 2);

        float q0 = t * (t - 1.f) * (t - 1.f);
        float q1 = tt * (2.f * t - 3) + 1.0f;
        float q2 = -(tt * (2.f * t - 3));
        float q3 = tt * (t - 1.f);

        float tx = (points[p0].x * q0 + points[p1].x * q1 + points[p2].x * q2 + points[p3].x * q3);
        float ty = (points[p0].y * q0 + points[p1].y * q1 + points[p2].y * q2 + points[p3].y * q3);

        return{ tx, ty };
    }

    vec2 get_tangent(std::vector<vec2> points, float t) {
        int p1 = (int)t + 1;
        int p2 = p1 + 1;
        int p3 = p2 + 1;
        int p0 = p1 - 1;

        float tt = pow(t, 2);

        float q0 = 3 * tt - 4 * t + 1;
        float q1 = 6 * tt - 6 * t;
        float q2 = -6 * tt + 6 * t;
        float q3 = 3 * tt - 2 * t;

        float tx = (points[p0].x * q0 + points[p1].x * q1 + points[p2].x * q2 + points[p3].x * q3);
        float ty = (points[p0].y * q0 + points[p1].y * q1 + points[p2].y * q2 + points[p3].y * q3);

        return{ tx, ty };
    }

}

bool is_out_of_boundary(entt::entity entity) {
    auto &&[position, motion] = m_registry.get<Position, Motion>(entity);
    if (position.position.y < map_y_min) {
        position.position.y = map_y_min;
        if (motion.velocity.y < 0) {
            motion.velocity.y = 0;
        }
        return true;
    } else if (position.position.y > map_y_max) {
        position.position.y = map_y_max;
        if (motion.velocity.y > 0) {
            motion.velocity.y = 0;
        }
        return true;
    }
    if (position.position.x < map_x_min) {
        position.position.x = map_x_min;
        if (motion.velocity.x < 0) {
            motion.velocity.x = 0;
        }
        return true;
    } else if (position.position.x > map_x_max) {
        position.position.x = map_x_max;
        if (motion.velocity.x > 0) {
            motion.velocity.x = 0;
        }
        return true;
    }
    return false;
}

void physics_update(float elapsed_ms) {

    if (DebugSystem::in_debug_mode) {
        for (auto&&[entity, boundingBox] :m_registry.view<BoundingBox>().each()) {
            if (m_registry.valid(entity)) {
                for (auto &vertices : boundingBox.transformed_vertices) {
                    auto dotSize = vec2(5.f, 5.f);
                    DebugSystem::createLine(vertices, dotSize);
                }
            }
        }
    }
//    for (auto &entity: m_registry.view<UnitProperty>()) {
//        auto &property = m_registry.get<UnitProperty>(entity);
//        std::cout << "test" << std::endl;
//        if (property.selected) {
//            // Mark the selected unit
//            auto& bb = m_registry.get<BoundingBox>(entity);
//            for (auto& vertices : bb.transformed_vertices) {
//                auto dotSize = vec2(5.f, 5.f);
//                std::cout << "draw" << std::endl;
//                DebugSystem::createLine(vertices, dotSize);
//            }
//        }else if(property.selected_release){
//            //draw the moving  trajectory
//            auto &motion= m_registry.get<Motion>(entity);
//            auto &position = m_registry.get<Position>(entity);
//            vec2 tri_pos = {(position.position.x-property.init_pos.x)/2+property.init_pos.x, (position.position.y-property.init_pos.y)/2+property.init_pos.y};
//            float x1 = position.position.x-property.init_pos.x;
//            float y1 = position.position.y-property.init_pos.y;
//            // use dot product to calculate the angle
//
//            vec2 v1 = normalize(vec2({x1, y1}));
//
//            float angle = acos(dot(v1, {1.f, 0.f}));
//            if (y1 < 0.f) {
//                //clock wise
//                angle *= -1.f;
//            }
//            if (y1==0){
//
//                DebugSystem::createDirectTri(tri_pos,{x1/2,30},0.f);
//                std::cout << "tri" << std::endl;
//            } else if (x1==0){
//                DebugSystem::createDirectTri(tri_pos,{30,y1/2},M_PI/2*y1/abs(y1));
//                std::cout << "tri" << std::endl;
//            }else {
//                DebugSystem::createDirectTri(tri_pos, {abs((position.position.x-property.init_pos.x)/2),abs((position.position.y-property.init_pos.y)/2)},angle);
//                std::cout << "tri" << std::endl;
//            }
//            property.selected_release = false;
//        }
//    }

    float step_seconds = 1.0f * (elapsed_ms / 1000.f);
    for (auto&&[entity, motion, position, unit_property, bounding_box]: m_registry.view<Motion, Position, UnitProperty, BoundingBox>().each()) {
        if (m_registry.valid(unit_property.actualTarget) &&
            m_registry.has<UnitProperty, Motion, Position, BoundingBox>(unit_property.actualTarget) ) {
            update_velocity_and_facing_dir(entity, unit_property.actualTarget);
            position.position += motion.velocity * step_seconds;
        }
        set_transformed_bounding_box(entity);
        is_out_of_boundary(entity);
        //basic health bar
        vec2 pos = {position.position.x, position.position.y - tile_size.y/2};
        //vec2 scale = {unit_property.hp, 5};
        float max_hp_bar_length = (tile_size.x-10);
        vec2 scale = {max_hp_bar_length/unit_property.maxhp * unit_property.hp, 5};
        DebugSystem::createLine(vec2(pos.x+(scale.x-max_hp_bar_length)/2, pos.y), scale);
    }

    for (auto&&[entity, motion, position, projectile_property]: m_registry.view<Motion, Position, ProjectileProperty>().each()) {
        if (m_registry.valid(projectile_property.actualTarget)) {
           auto& target_position = m_registry.get<Position>(projectile_property.actualTarget);
            if(A_Star::spline){
                position.position = get_spos(projectile_property.spoints, projectile_property.t);
                vec2 tangent = get_tangent(projectile_property.spoints, projectile_property.t);
                position.angle = atan2(tangent.y,tangent.x);
                projectile_property.t += 0.02f;
            }else{
                vec2 dir = glm::normalize( target_position.position-position.position);
                position.angle = atan2(dir.y,dir.x);
                motion.velocity = glm::normalize(dir) * projectile_speed;
                position.position += motion.velocity * step_seconds;
            }
            set_transformed_bounding_box(entity);
            if(is_out_of_boundary(entity)){
                m_registry.destroy(entity);
            }

            if(m_registry.valid(projectile_property.actualTarget) && m_registry.valid(entity) && collides(entity, projectile_property.actualTarget)){
                auto& target_prop = m_registry.get<UnitProperty>(projectile_property.actualTarget);
                target_prop.hp -= projectile_property.damage;
                if(target_prop.hp<=0){m_registry.destroy(projectile_property.actualTarget);}
                m_registry.destroy(entity);
            }
        }else{
            m_registry.destroy(entity);
        }

    }


}

void physics_update_keyframe(float elapsed_ms) {
    for (auto&& [entity, pos, keyframe] : m_registry.view<Position, KeyFrameMotion>().each()) {
        vec3 currFrame, nextFrame;
        unsigned int frame_index = keyframe.currFrame;
        int next_frame_index = (frame_index == keyframe.keyframes.size() - 1? -1 : frame_index + 1);
        currFrame = keyframe.keyframes[frame_index];
        if (next_frame_index > 0) {
            // interpolate next position
            nextFrame = keyframe.keyframes[next_frame_index];
            float diff_x = nextFrame.x - currFrame.x;
            float diff_y = nextFrame.y - currFrame.y;
            float diff_angle = nextFrame.z - currFrame.z;
            float time_passed = (keyframe.time_gap - keyframe.time_left)/keyframe.time_gap;
            if (keyframe.time_left <= 0) {
                keyframe.time_left = keyframe.time_gap;
                keyframe.currFrame = next_frame_index;
                continue;
            }
            pos.position.y = time_passed * diff_y + currFrame.y;
            pos.angle = time_passed * diff_angle + currFrame.z;
            keyframe.time_left -= elapsed_ms;
        }
        else {
            pos.angle = currFrame.z;
            pos.position = vec2(currFrame.x, currFrame.y);
            m_registry.remove<KeyFrameMotion>(entity);  
        }
    }
}
