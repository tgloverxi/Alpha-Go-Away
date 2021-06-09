#version 330 

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int frame;
uniform float one_over_number_of_frame;

void main()
{
	texcoord = in_texcoord;
	texcoord.x =  frame*one_over_number_of_frame + (1.0 - texcoord.x*one_over_number_of_frame);
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
