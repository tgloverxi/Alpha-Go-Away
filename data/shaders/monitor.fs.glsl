#version 330

// From Vertex Shader
in vec3 vcolor;
in vec2 vpos; // Distance from local origin

// Application data
uniform vec3 fcolor;
uniform int  is_enemy;

// Output color
layout(location = 0) out vec4 color;

void main()
{
	if(is_enemy > 0){
		color = vec4(vec3(1.0, 1.0, 1.0) - vcolor, 1.0);
	}else{
		color = vec4(fcolor * vcolor, 1.0);
	}
}