#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;
uniform int meteor;
uniform float particle_explosion_time;
float hash(float x)
{
    return fract(sin(x*.0127863)*17143.321);
}
vec4 explode(vec4 position, vec2 velocity)
{
    vec2 dir = velocity*(2.0-particle_explosion_time+time);
    return position + vec4(dir, 0.0, 0.0);
}

float smooth_random(vec2 center){
    return tan(sin(time*center.x*center.y)*1.7);
}

vec4 rotate(vec4 position, vec2 center){
    return position+ vec4(sign(center.x)*sin(smooth_random(center)),sign(center.y)*cos(smooth_random(center)), 0.0, 0.0)/10.0;
}

void explosion(){
    vec2 center = (gs_in[0].texCoords.xy + gs_in[1].texCoords.xy + gs_in[2].texCoords.xy);
    vec2 hash_velocity = (vec2(hash(center.x), hash(center.y))-0.5) * 2;
    //	vec2 hash_velocity = center;
    gl_Position = explode(gl_in[0].gl_Position, hash_velocity);
    //	gl_Position = gl_in[0].gl_Position;
    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, hash_velocity);
    //	gl_Position = gl_in[1].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, hash_velocity);
    //	gl_Position = gl_in[2].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    EndPrimitive();
}

void move(){

    gl_Position = gl_in[0].gl_Position;
    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;

    TexCoords = gl_Position.xy;
    EmitVertex();
    EndPrimitive();
}

void meteor_field(){
    vec2 center = (gs_in[0].texCoords.xy + gs_in[1].texCoords.xy + gs_in[2].texCoords.xy)/3.0;
    gl_Position = rotate(gl_in[0].gl_Position, center);
    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = rotate(gl_in[1].gl_Position, center);

    TexCoords = gl_Position.xy;
    EmitVertex();
    gl_Position = rotate(gl_in[2].gl_Position, center);

    TexCoords = gl_Position.xy;
    EmitVertex();
    EndPrimitive();
}

void main() {
    if (particle_explosion_time-time>0){
        explosion();
    }else if(meteor>0){
        meteor_field();
    } else {
        move();
    }
}