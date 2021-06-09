#ifndef ALPHAGO_AWAY_PARTICLESYSTEM_HPP
#define ALPHAGO_AWAY_PARTICLESYSTEM_HPP
#include <vector>

#include <entt/entt.hpp>
//#include "gui/imgui_impl_glfw.h"
//#include "gui/imgui_impl_opengl3.h"

#include "components/motion.hpp"
#include "core/registry.hpp"
#include "system/render.hpp"
#include "system/physics.hpp"


// Represents a single particle and its state
struct Particle {
    vec4 color;
    float life;
};
struct Light{};


// ParticleGenerator acts as a container for rendering a large number of
// particles by repeatedly spawning and updating particles and killing
// them after a given amount of time.
class ParticleSystem
{
public:
    // constructor
    ParticleSystem(float speed);
    // update all particles
    void update();
    // render all particles
//    void Draw();
	static float max_distance;
	static float start_explosion_time;
	static bool meteor_field;

	const float FIXED_SPEED = 2;

	bool swarm_behavior_toggle = false;
	bool gravity_toggle = true;
	bool elastic_collision_toggle = true;

    void emitParticle(vec2 pos, int amount, bool emit_light = false);

    bool is_precise_collision = false;
private:
    // state
//    std::vector<entt::entity> particles;
//    unsigned int amount;
    // render state
//    Shader shader;
//    Texture2D texture;
//    unsigned int VAO;
    // initializes buffer and vertex attributes
//    void init();
    // returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
//    unsigned int firstUnusedParticle();
    // respawns particle
    float killSpeed = 0.5f;
	//float killSpeed = 0.00005f;
};

#endif
