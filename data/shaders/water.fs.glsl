#version 330
#define max_particle 20
uniform sampler2D screen_texture;
uniform float time;
uniform float last_firework_time;
uniform int dark_mode;
uniform int flash_light_type;
uniform float illumination_param;
uniform float darken_screen_factor;
uniform float particle_x[max_particle];
uniform float particle_y[max_particle];
uniform float cursor_x;
uniform float cursor_y;
in vec2 texcoord;


layout(location = 0) out vec4 color;

/*by musk License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

	messing around with my previous shader . . .


*/

//bounce count drasticaly reduces performance for long simulations
//setting them to zero disables bouncing but drastically improves framerate
#define max_floor_bounce 10
#define max_wall_bounce 16

//motion blur! reccomended
#define motion_blur


//brightness conntrols the how much light each particle emits
//reduce brightness if you put in more particles
#define brightness 2.6


#define light_duration 1000.0


//solve 2nd order polynomial, return the maximum of two solutions
//could have optimised for this particular example... meh...
float second(float a, float b, float c)
{
    float x1 = (-b+sqrt((b*b-2.0*a*c)))/(a);
    float x2= (-b-sqrt((b*b-2.0*a*c)))/(a);
    return max(x1, x2);
}

//given initial values compute position after t seconds
vec3 physics(vec3 pos, vec3 vel, vec3 acc, float t)
{
    //this loop processes upto max_bounces collisions... nice :)
    for (int i=0; i<max_floor_bounce; i++)
    {
        float tc = second(acc.y, vel.y, pos.y);
        //now we know that there will be a collision with the plane
        //in exactly tc seconds

        if (t>tc)//if time is greater than time of collision
        {
            t-=tc;//process the collision
            pos.x = pos.x + vel.x*tc + acc.x*tc*tc*.5;
            pos.y = .1;
            vel = vel + acc*tc;
            vel.y*=-.5;
        }
        else break;//it wont collide, yay!
    }

    pos = pos + vel*t + acc*t*t*.5;// x = v*t + .5*a*t^2

    float ar = 2.0;
    float hwall = 8.0*ar;
    float right_wall = 16.0 * 0.5;

    for (int i=0; i<max_wall_bounce; i++)
    {
        if (pos.x>+right_wall) pos.x = 2.0*right_wall-pos.x;
        else if (pos.x<-hwall) pos.x = -2.0*hwall-pos.x;
        else break;
    }
    return pos;
}

float hash(float x)
{
    return fract(sin(x*.0127863)*17143.321);//decent hash for noise generation
}

float hash(vec2 x)
{
    return fract(cos(dot(x.xy, vec2(0.37311, 53.21))*121.1126143)*412.0);
}

vec3 cc(vec3 color, float factor, float factor2)// color modifier
{
    float w = color.x+color.y+color.z;
    return mix(color, vec3(w)*factor, w*factor2);
}

float get_partilce_light_intensity_contribution(vec2 dir){
    vec2 temp = dir/(800.0/300.0);
    float s = sqrt(dot(temp, temp));
    s*=800.0*.05;
    return 1.0/(s+1.0);
}

vec4 particle_entity_flash_light(vec2 uv)
{
    float acc = .0;
    for (int sampleIndex = 0; sampleIndex<max_particle; sampleIndex++)
    {
        if (particle_x[sampleIndex] <= -1) break;
        vec2 p = vec2((particle_x[sampleIndex]/1600.0-0.5), (0.5-particle_y[sampleIndex]/800.0));
        vec2 temp = (((uv-0.5))-p.xy)*illumination_param;

        acc+=get_partilce_light_intensity_contribution(temp);
    }
    vec3 part_color = vec3(1.0, 1.0, 1.0);
    part_color = normalize(abs(part_color));
    vec3 color = vec3(.01, .01, .01)+part_color*acc*brightness;


    color *=.8;
    color -= length(uv)*.005;
    color = cc(color, .5, .5);
    color = vec3(1.2)*color/(vec3(1.0)+color);
    color += hash(uv.xy+color.xy)*.02;

    vec4 fragColor = vec4(color, 1.0);
    return fragColor;
}


vec4 shader_particle_flash_light(vec2 uv)
{


    uv-=0.5;
    uv.x *= 2.0;
    uv*=16.0;

    float acc = .0;

    float t = time - last_firework_time;

    float mt = mod(t, light_duration);
    float seed = t-mt;//the seed to generate new variables

    for (int sampleIndex = 0; sampleIndex<max_particle; sampleIndex++)
    {
        #ifdef motion_blur
        float tnoise = hash(uv.xy+vec2(sampleIndex, sampleIndex)-vec2(acc))*(1.0/24.0);
        #else
        float tnoise = .0;
        #endif

        float iseed = seed + float(sampleIndex);//the seed for each particle

        float iangle = 3.14159*2.0*hash(iseed);
        float imagnitude = 64.0*hash(iseed+15.420)*hash(seed+14.2)+4.0;
        float angle = 3.14159*2.0*hash(seed);
        float magnitude = 148.0*hash(seed)*hash(seed+4.0);
        vec2 pos = vec2((cursor_x/1600.0-0.5)*32.0, (1.0-cursor_y/800.0)*16.0);
        vec3 p = (
        physics(
        vec3(pos.x, pos.y, 8.0), //initial position
        vec3(cos(iangle), sin(iangle), .0)*imagnitude+
        vec3(cos(angle), sin(angle), .0)*magnitude,
        vec3(.0, -60.0, .0), //acceleration
        mt+tnoise));


        vec2 temp = (uv-p.xy+vec2(.0, 8.0))*illumination_param/100.0;
        temp/=800.0/300.0;//adjust the size of the particles
        float s = sqrt(dot(temp, temp));
        //s-=1.0;
        s*=800.0*.05;
        //s = min(1.0,max(.0,s));

        acc+=1.0/(s+1.0);
    }

    vec3 part_color = vec3(cos(seed*13.1230), cos(seed*15.5310), cos(seed*17.55));
    part_color = normalize(abs(part_color));
    vec3 color = vec3(.01, .01, .01)+part_color*acc*brightness;

    //color = max(color,1.);

    //color = mix(color*color,color,1.4);
    color *=.8;
    color -= length(uv)*.005;
    color = cc(color, .5, .5);
    color = vec3(1.2)*color/(vec3(1.0)+color);
    color += hash(uv.xy+color.xy)*.02;

    vec4 fragColor = vec4(color, 1.0);
    return fragColor;
}





vec2 distort(vec2 uv)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //float x = sin(time * 0.1 + uv.x * 12) * 0.007;
    //float y = sin(time * 0.1 + uv.y * 12) * 0.007;
    //uv.x += x;
    //uv.y += y;

    //uv.x = max(0.001, uv.x);
    //uv.x = min(0.999, uv.x);
    //uv.y = max(0.001, uv.y);
    //uv.y = min(0.999, uv.y);
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    vec2 coord = uv.xy;
    return coord;
}

vec4 color_shift(vec4 in_color)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // HANDLE THE COLOR SHIFTING HERE (you may want to make it blue-ish)
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //vec4 color = in_color + vec4(-0.1, 0.0, 0.25, 0.0);
    return in_color;
}

vec4 fade_color(vec4 in_color)
{
    vec4 color = in_color;
    if (darken_screen_factor > 0)
    color -= darken_screen_factor * vec4(0.8, 0.8, 0.8, 0);

    return color;
}

void main()
{
    vec2 coord = distort(texcoord);

    vec4 in_color = texture(screen_texture, coord);
    color = color_shift(in_color);
    color = fade_color(color);
    if (texcoord.x<0.75 && dark_mode > 0){
        if(flash_light_type==2){
            color *= shader_particle_flash_light(texcoord);
        }else{
            color *= particle_entity_flash_light(texcoord);
        }
    }

}
