//
// Created by grumpy on 2021-03-13.
//

#ifndef ALPHAGO_AWAY_UNIT_HPP
#define ALPHAGO_AWAY_UNIT_HPP
#include "core/factories.hpp"
#include "core/common.hpp"
#include "core/registry.hpp"
#include "components/units.hpp"
ShadedMesh &create_unit_mesh_resource(UnitType unitType, std::string unit_state);
void init_unit_resources();
void remove_all_unit_state_components(entt::entity entity);
void unit_attack(entt::entity entity, UnitType unitType);
void unit_stand(entt::entity entity, UnitType unitType);
void unit_walk(entt::entity entity, UnitType unitType);
void unit_die(entt::entity entity, UnitType unitType);
void init_key_frames(entt::entity entity);

#endif //ALPHAGO_AWAY_UNIT_HPP
