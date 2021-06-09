#ifndef ALPHAGO_AWAY_AI_HPP
#define ALPHAGO_AWAY_AI_HPP
#include <vector>
#include "system/utils/a_star.hpp"
#include "system/utils/kd-tree.hpp"
#include "core/common.hpp"
#include "core/game.hpp"
#include <entt/entt.hpp>
#include "core/registry.hpp"
#include "components/units.hpp"
#include "components/tile.hpp"
#include "components/motion.hpp"
#include "entities/unit.hpp"
#include "core/factories.hpp"
#include <random>


void ai_update(float elapsed_ms);



#endif //ALPHAGO_AWAY_AI_HPP