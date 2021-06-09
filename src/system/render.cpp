// internal
#include "render.hpp"
#include "render_components.hpp"
#include "../core/game.hpp"

#include <iostream>
#include <imgui.h>
#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"
void RenderSystem::drawTexturedMesh(entt::entity entity, const mat3 &projection, vec2 off)
{
	auto& position = m_registry.get<Position>(entity);
	auto& mesh_ref = m_registry.get<ShadedMeshRef>(entity);
    auto& texmesh = *mesh_ref.reference_to_cache;
    int number_of_frames = texmesh.number_of_frames;
	// Transformation code, see Rendering and Transformation in the template specification for more info
	// Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
	Transform transform;

	transform.translate({position.position.x + off.x, position.position.y + off.y});
    transform.rotate(position.angle);
	transform.scale(position.scale);



	// Setting shaders
	glUseProgram(texmesh.effect.program);
	glBindVertexArray(texmesh.mesh.vao);
	gl_has_errors();

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();

	GLint transform_uloc = glGetUniformLocation(texmesh.effect.program, "transform");
	GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
    GLint frame_uloc = glGetUniformLocation(texmesh.effect.program, "frame");
    GLint one_over_number_of_frame = glGetUniformLocation(texmesh.effect.program, "one_over_number_of_frame");
	gl_has_errors();

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(texmesh.effect.program, "in_texcoord");
	GLint in_color_loc = glGetAttribLocation(texmesh.effect.program, "in_color");
	if (in_texcoord_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), reinterpret_cast<void*>(sizeof(vec3))); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texmesh.texture.texture_id);
	}
	else if (in_color_loc >= 0)
	{
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(0));
		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(sizeof(vec3)));

	}
	else
	{
		throw std::runtime_error("This type of entity is not yet supported");
	}
	gl_has_errors();

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(texmesh.effect.program, "fcolor");

	glUniform3fv(color_uloc, 1, (float*)&texmesh.texture.color);

	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();
	GLsizei num_indices = size / sizeof(uint16_t);
	//GLsizei num_triangles = num_indices / 3;

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.mat);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    glUniform1i(frame_uloc, ((int) (floor(frame_num)) % number_of_frames));
    glUniform1f(one_over_number_of_frame, 1.f / number_of_frames);
	gl_has_errors();

	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
}

void RenderSystem::drawParticle(const mat3& projection)
{
    float explosion_duration = ParticleSystem::start_explosion_time - glfwGetTime();
    if(explosion_duration<0 && explosion_duration>-2.f){
        for(auto entity: m_registry.view<Particle>()){
            m_registry.destroy(entity);
        }
        return;
    }
    std::vector<vec2> particle_positions;
    std::vector<vec2> particle_sizes;
    std::vector<entt::entity> particle_entities;
    for(auto&&[entity, particle, shadedMeshRef, position]: m_registry.view<Particle, ShadedMeshRef, Position>().each()){
        particle_positions.push_back(position.position);
        particle_sizes.push_back(position.scale);
        if(particle_entities.empty()){
            particle_entities.push_back(entity);
        }
        gl_has_errors();
    }
    if(particle_entities.empty()) return;
    auto& mesh_ref = m_registry.get<ShadedMeshRef>(particle_entities[0]);
    auto& texmesh = *mesh_ref.reference_to_cache;


    // Transformation code, see Rendering and Transformation in the template specification for more info
    // Incrementally updates transformation matrix, thus ORDER IS IMPORTANT
    Transform transform;
//    transform.translate(motion.position);
    transform.rotate(0);


//    transform.scale(motion.scale);

    // Setting shaders
    glUseProgram(texmesh.effect.program);
    glBindVertexArray(texmesh.mesh.vao);
    gl_has_errors();

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    gl_has_errors();

    GLint transform_uloc = glGetUniformLocation(texmesh.effect.program, "transform");
    GLint projection_uloc = glGetUniformLocation(texmesh.effect.program, "projection");
    gl_has_errors();

    // Setting vertex and index buffers
    glBindBuffer(GL_ARRAY_BUFFER, texmesh.mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texmesh.mesh.ibo);
    gl_has_errors();

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(texmesh.effect.program, "in_position");
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), reinterpret_cast<void*>(0));
    gl_has_errors();

    GLuint time_uloc       = glGetUniformLocation(texmesh.effect.program, "time");
    glUniform1f(time_uloc, static_cast<float>(glfwGetTime() * 1.0f));
    gl_has_errors();

    GLuint meteor_uloc       = glGetUniformLocation(texmesh.effect.program, "meteor");
    glUniform1i(meteor_uloc, (int)ParticleSystem::meteor_field);
    gl_has_errors();

    GLuint particle_explosion_time_uloc       = glGetUniformLocation(texmesh.effect.program, "particle_explosion_time");
    glUniform1f(particle_explosion_time_uloc, static_cast<float>(ParticleSystem::start_explosion_time));
    gl_has_errors();


    // Get number of indices from index buffer, which has elements uint16_t
    GLint size = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    gl_has_errors();
    GLsizei num_indices = size / sizeof(uint16_t);
    //GLsizei num_triangles = num_indices / 3;

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.mat);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);
    gl_has_errors();

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    GLfloat offsets[particle_positions.size()*3];
    for (int i = 0; i < particle_positions.size(); i++){
        offsets[i*3] = (GLfloat) particle_positions[i].x;
        offsets[i*3+1] = (GLfloat) particle_positions[i].y;
        offsets[i*3+2] = (GLfloat) particle_sizes[i].y;
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(offsets), offsets, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(0, 3);
    glDrawArraysInstanced(GL_TRIANGLES, 0, num_indices, particle_positions.size()*3);


//    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);

    glBindVertexArray(0);
}

// Draw the intermediate texture to the screen, with some distortion to simulate water
void RenderSystem::drawToScreen()
{




	// Setting shaders
	glUseProgram(screen_sprite.effect.program);
	glBindVertexArray(screen_sprite.mesh.vao);
	gl_has_errors();

	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(&window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	
	// Disable alpha channel for mapping the screen texture onto the real screen
	glDisable(GL_BLEND); // we have a single texture without transparency. Areas with alpha <1 cab arise around the texture transparency boundary, enabling blending would make them visible.
	glDisable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, screen_sprite.mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_sprite.mesh.ibo); // Note, GL_ELEMENT_ARRAY_BUFFER associates indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// Draw the screen texture on the quad geometry
	gl_has_errors();
	// Set clock
	GLuint time_uloc       = glGetUniformLocation(screen_sprite.effect.program, "time");
    GLuint illumination_uloc       = glGetUniformLocation(screen_sprite.effect.program, "illumination_param");
    GLuint last_firework_time_uloc       = glGetUniformLocation(screen_sprite.effect.program, "last_firework_time");
    GLuint dark_mode_uloc       = glGetUniformLocation(screen_sprite.effect.program, "dark_mode");
	GLuint dead_timer_uloc = glGetUniformLocation(screen_sprite.effect.program, "darken_screen_factor");
    GLuint flash_light_type_uloc      = glGetUniformLocation(screen_sprite.effect.program, "flash_light_type");

    glUniform1f(time_uloc, static_cast<float>(glfwGetTime() * 1.0f));
    glUniform1f(illumination_uloc, static_cast<float>(RenderSystem::illumination_param));
    glUniform1i(dark_mode_uloc, RenderSystem::dark_mode);
    glUniform1f(last_firework_time_uloc, static_cast<float>(RenderSystem::last_firework_time));
    glUniform1i(flash_light_type_uloc, RenderSystem::flash_light_type);

    auto& screen =  m_registry.get<ScreenState>(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
    GLuint cursor_x_uloc       = glGetUniformLocation(screen_sprite.effect.program, "cursor_x");
    glUniform1f(cursor_x_uloc, (GLfloat)(RenderSystem::cursor_position.x));
    gl_has_errors();

    GLuint cursor_y_uloc       = glGetUniformLocation(screen_sprite.effect.program, "cursor_y");
    glUniform1f(cursor_y_uloc, (GLfloat)(RenderSystem::cursor_position.y));
    gl_has_errors();


    // Set the vertex position and vertex texture coordinates (both stored in the same VBO)
	GLint in_position_loc = glGetAttribLocation(screen_sprite.effect.program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	GLint in_texcoord_loc = glGetAttribLocation(screen_sprite.effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // note the stride to skip the preceeding vertex position
	gl_has_errors();


    if(RenderSystem::dark_mode>0){
        auto view = m_registry.view<Light, ShadedMeshRef, Position>();
        int size = view.size_hint();
        GLfloat particle_x[size+1];
        GLfloat particle_y[size+1];
        int i = 0;
        for(auto&&[entity, shadedMeshRef, position]: view.each()){
            particle_x[i] = position.position.x;
            particle_y[i] = position.position.y;
            i++;
        }
        particle_x[i] = -1;
        particle_y[i] = -1;
        GLint particle_x_uloc = glGetUniformLocation(screen_sprite.effect.program, "particle_x");
        glUniform1fv(particle_x_uloc, size+1,(const GLfloat*)  &particle_x);
        GLint particle_y_uloc = glGetUniformLocation(screen_sprite.effect.program, "particle_y");
        glUniform1fv(particle_y_uloc, size+1, (const GLfloat*) &particle_y);
        gl_has_errors();
    }



	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_sprite.texture.texture_id);

	// Draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr); // two triangles = 6 vertices; nullptr indicates that there is no offset from the bound index buffer
	glBindVertexArray(0);
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw(vec2 window_size_in_game_units)
{
    frame_num += 0.2f;
	// Getting size of window
	ivec2 frame_buffer_size; // in pixels
	glfwGetFramebufferSize(&window, &frame_buffer_size.x, &frame_buffer_size.y);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// Clearing backbuffer
	glViewport(0, 0, frame_buffer_size.x, frame_buffer_size.y);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 1, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();

	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;
	float right = window_size_in_game_units.x;
	float bottom = window_size_in_game_units.y;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	mat3 projection_2D{ { sx, 0.f, 0.f },{ 0.f, sy, 0.f },{ tx, ty, 1.f } };

	// Draw all textured meshes that have a position and size component

	vec2 off = { 0, 0 };
	if (Game::shake)
	{
		float tempx = rand() % 100 + 1;
		float tempy = rand() % 100 + 1;
		float x = 0;
		float y = 0;
		if (tempx < 50)
		{
			x = rand() % 10 + 1;
		}
		else {
			x = -(rand() % 10 + 1);
		}
		if (tempy < 50)
		{
			y = rand() % 10 + 1;
		}
		else {
			y = -(rand() % 10 + 1);
		}
		off = { x, y };
	}

	for(entt::entity entity: m_registry.view<ScreenComponent>()){
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
	}

    for (entt::entity entity : m_registry.view<ShadedMeshRef, Tile>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }

    for (entt::entity entity : m_registry.view<ShadedMeshRef, Explosion>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }

    for (entt::entity entity : m_registry.view<ShadedMeshRef, ProjectileProperty>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }

    for (entt::entity entity : m_registry.view<ShadedMeshRef, UnitProperty>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }
    
//    for (entt::entity entity : m_registry.view<ShadedMeshRef, Particle>())
//    {
//        drawTexturedMesh(entity, projection_2D);
//        gl_has_errors();
//    }
    
    drawParticle(projection_2D);
    
    for (entt::entity entity : m_registry.view<TutorialComponent>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }
    
    for (entt::entity entity : m_registry.view<ButtonComponent>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }
    
    for (entt::entity entity : m_registry.view<InfoComponent>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }


    for (entt::entity entity : m_registry.view<ShadedMeshRef, DebugComponent>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }
    
    for (entt::entity entity : m_registry.view<ShadedMeshRef, resultComponent>())
    {
        drawTexturedMesh(entity, projection_2D, off);
        gl_has_errors();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	// Truely render to the screen
    drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(&window);
}

void gl_has_errors()
{
	GLenum error = glGetError();

	if (error == GL_NO_ERROR)
		return;
	
    std::cout << "error is:    " << error << std::endl;
	const char* error_str = "";
	while (error != GL_NO_ERROR)
	{
		switch (error)
		{
		case GL_INVALID_OPERATION:
			error_str = "INVALID_OPERATION";
			break;
		case GL_INVALID_ENUM:
			error_str = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error_str = "INVALID_VALUE";
			break;
		case GL_OUT_OF_MEMORY:
			error_str = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error_str = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}

		std::cerr << "OpenGL:" << error_str << std::endl;
		error = glGetError();
	}
	throw std::runtime_error("last OpenGL error:" + std::string(error_str));
}
