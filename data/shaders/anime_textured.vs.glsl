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

void main()
{
	// add offset to textcoord.x
	texcoord = in_texcoord;
	texcoord.x =  0.163 * frame + 0.1667 * texcoord.x; //- texcoord.x * 0.5;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}
