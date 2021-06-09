#ifndef ALPHAGO_AWAY_COMMON_HPP
#define ALPHAGO_AWAY_COMMON_HPP

// stlib
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>

// glfw (OpenGL)
#define NOMINMAX
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
#include <glm/ext/vector_uint2.hpp>  // uvec2
#include <imgui.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <iostream>

#include <entt/entt.hpp>
#include <fstream>
#include <filesystem>
#include <string>


namespace fs = std::filesystem;



using namespace glm;
static const float PI = 3.14159265359f;

// Simple utility functions to avoid mistyping directory name

// be sure to change this to your own path! I can't figure out how to get the relative path.


//inline std::string data_path() { return "C:/Users/billz/OneDrive/Desktop/school/cpsc427/AIphaGo-Away/data"; };
inline std::string data_path() { return "data"; };
//std::string temp = std::filesystem::current_path().string();
/*std::string path = path_full.substr(0, path_full.size() - 21);
inline std::string data_path() {
	return path;
};*/
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string json_path(const std::string& name){ return data_path() + "/json/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };
void load_json(const std::string& file_name, nlohmann::json& json);
void save_json(const std::string& file_name, nlohmann::json& json);

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

#endif //ALPHAGO_AWAY_COMMON_HPP