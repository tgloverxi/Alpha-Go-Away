// Header
#include "debug.hpp"
#include "system/render.hpp"

#include <cmath>
#include <iostream>

#include "system/render_components.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 2
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace DebugSystem 
{
	void createLine(vec2 position, vec2 scale) {
		auto entity = m_registry.create();


		std::string key = "thick_line";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
			constexpr float z = -0.1f;
			vec3 red = { 0.8,0.1,0.1 };

			// Corner points
			ColoredVertex v;
			v.position = {-0.5,-0.5,z};
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);

			// Two triangles
			resource.mesh.vertex_indices.push_back(0);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(3);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(2);
			resource.mesh.vertex_indices.push_back(3);

			RenderSystem::createColoredMesh(resource, "colored_mesh");
		}

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		m_registry.emplace<ShadedMeshRef>(entity, resource);

		// Create motion
		auto& motion = m_registry.emplace<Motion>(entity);
        auto& entity_position = m_registry.emplace<Position>(entity);
        entity_position.angle = 0.f;
		motion.velocity = { 0, 0 };
        entity_position.position = position;
        entity_position.scale = scale;

		m_registry.emplace<DebugComponent>(entity);
	}

    //Everytime when you move a unit, it will draw the move trajectory for you
    void createDirectTri(vec2 position, vec2 scale,float angle) {
        auto entity = m_registry.create();

        std::string key = "direct_triangle";
        ShadedMesh& resource = cache_resource(key);
        if (resource.effect.program.resource == 0) {
            // create a procedural circle
            constexpr float z = -0.1f;
            //vec3 red = { 0.8,0.1,0.1 };
            vec3 orange = {1.0, 0.27,0.0};

            // Corner points
            ColoredVertex v;
            v.position = {-0.5,-0.5,z};
            v.color = orange;
            resource.mesh.vertices.push_back(v);

            v.position = { 0.5,0,z };
            v.color = orange;
            resource.mesh.vertices.push_back(v);

            v.position = { -0.5,0.5,z };
            v.color = orange;
            resource.mesh.vertices.push_back(v);



            // Two triangles
            resource.mesh.vertex_indices.push_back(0);
            resource.mesh.vertex_indices.push_back(2);
            resource.mesh.vertex_indices.push_back(1);



            RenderSystem::createColoredMesh(resource, "colored_mesh");
        }

        // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
        m_registry.emplace<ShadedMeshRef>(entity, resource);
        //ShadedMeshRef& mesh_ref = ECS::registry<ShadedMeshRef>.get(entity);
        //mesh_ref.depth = -1;
        // Create motion
        auto& motion = m_registry.emplace<Motion>(entity);
        auto& entity_position = m_registry.emplace<Position>(entity);
        entity_position.angle = angle;
        motion.velocity = { 0, 0 };
        entity_position.position = position;
        entity_position.scale = scale;

        m_registry.emplace<DebugComponent>(entity);
    }


    void clearDebugComponents() {
		// Clear old debugging visualizations
        for(const auto entity: m_registry.view<DebugComponent>()) {
            m_registry.destroy(entity);
        }
	}

	bool in_debug_mode = false;
}
