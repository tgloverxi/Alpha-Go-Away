#include "registry.hpp"

entt::registry m_registry;

int get_entity_index_from_tile_index(int x, int y){
    return x * ((tile_matrix_dimension.y - tile_matrix_dimension.x * tile_matrix_dimension.y) /
                (tile_matrix_dimension.x - 1))
           - y
           + tile_matrix_dimension.x * tile_matrix_dimension.y - 1;
}

entt::entity get_tile_entity_at_position(vec2 position) {
    auto tile_entities = m_registry.view<Tile>();
    int x = floor(position.x / tile_size.x);
    int y = floor(position.y / tile_size.y);
    int index = get_entity_index_from_tile_index(x, y);
    return tile_entities[index];
};

