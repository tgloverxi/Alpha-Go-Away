//
// Created by grumpy on 2021-03-06.
//
#define STB_IMAGE_IMPLEMENTATION
#include "gui.hpp"
void ImGuiImage(GLuint texture_id){
    ImGui::Image((void*)(intptr_t)texture_id, ImVec2(tile_size.x, tile_size.y));
}

void ImGuiHelpImage(std::string screen_texture_path){
    std::string key = screen_texture_path;
    ShadedMesh &resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
        RenderSystem::createSprite(resource, textures_path(screen_texture_path), "textured");
    ImGui::Image((void*)(intptr_t)resource.texture.texture_id, ImVec2(320, 180));
}

