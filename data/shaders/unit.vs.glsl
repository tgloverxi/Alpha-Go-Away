#version 330 

// Input attributes
in vec3 in_position;
in vec3 in_color;

out vec3 vcolor;
out vec2 vpos;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int  is_enemy;

void main()
{
	vpos = in_position.xy;
	vcolor = in_color;
	vec3 pos = projection * transform * vec3(vpos.x, vpos.y, 1.0); // why not simply *in_position.xyz ?
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}