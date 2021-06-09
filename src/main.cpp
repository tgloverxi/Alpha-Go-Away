// stlib
#include <chrono>
#include <iostream>

// internal
#include "core/common.hpp"
#include "core/game.hpp"
#include "system/render.hpp"
#include "system/physics.hpp"
#include "system/ai.hpp"
#include "logger/debug.hpp"
#include "core/constants.hpp"
#include <imgui.h>
#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"
#include "gui/gui.hpp"

using Clock = std::chrono::high_resolution_clock;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
// Entry point
int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
#ifdef __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";

#endif

    // Initialize the main systems
	Game game(window_size_in_px);
	RenderSystem renderer(*game.window);
    init_factories();
    ParticleSystem particleSystem(0.0005f);
    game.particles = &particleSystem;

	// Set all states to default
	auto t = Clock::now();


	ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(game.window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
//    bool show_demo_window = true;

    game.restart();
	// Variable timestep loop
	while (!game.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


//        if (show_demo_window)
//            ImGui::ShowDemoWindow(&show_demo_window);



		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms = static_cast<float>((std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count()) / 1000.f;
		t = now;
        DebugSystem::clearDebugComponents();

        game.update(elapsed_ms, window_size_in_game_units);
        game.particles->update();
        renderer.draw(window_size_in_game_units);
	}
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	m_registry.clear<>();



    return EXIT_SUCCESS;
}
