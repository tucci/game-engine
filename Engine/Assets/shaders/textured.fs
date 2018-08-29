#version 330 core

in vec3 frag_pos;
in vec2 frag_uv;
in vec4 frag_pos_light_space;

out vec4 color;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;


float bias = 0.0025f;


float calc_shadow(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 0.5 : 0.0; 

    return shadow;
}  



void main(){

	float shadow = calc_shadow(frag_pos_light_space);   
	color = (1.0 - shadow) * texture(textureSampler, frag_uv).rgba;
}