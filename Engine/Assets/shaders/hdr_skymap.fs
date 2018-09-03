#version 330 core



in vec3 local_pos;

out vec4 frag_color;

uniform sampler2D equirectangular_map;


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(local_pos)); 
    vec3 color = texture(equirectangular_map, uv).rgb;
    
    frag_color = vec4(color, 1.0);
}