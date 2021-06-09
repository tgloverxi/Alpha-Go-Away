//
// Created by grumpy on 2021-02-10.
//

#ifndef ALPHAGO_AWAY_REGISTRY_HPP
#define ALPHAGO_AWAY_REGISTRY_HPP
#include <entt/entt.hpp>
#include "common.hpp"
#include "constants.hpp"
#include "components/tile.hpp"
extern entt::registry m_registry;
int get_entity_index_from_tile_index(int x, int y);
entt::entity get_tile_entity_at_position(vec2 position);
#endif //ALPHAGO_AWAY_REGISTRY_HPP
