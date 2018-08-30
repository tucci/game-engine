#version 330 core

in vec3 frag_pos;
in vec2 frag_uv;
in vec4 frag_pos_light_space;

out vec4 color;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;


float bias = 0.005f;

float shadow_value = 0.5f;


// TODO: move to variance shadow mapping
float calc_shadow(vec4 light_space_pos)
{
    
    vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
    proj_coords  = proj_coords  * 0.5 + 0.5;

	
	if(proj_coords.z > 1.0) {
		return 0;
	}
	
    // Get depth from shadow map. light's pov
    float closestDepth = texture(shadowMap, proj_coords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = proj_coords.z;
    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? shadow_value : 0.0; 

	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, proj_coords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? shadow_value : 0.0;        
        }    
    }
    shadow /= 9.0;

        

    return shadow;


}  



void main(){

	float shadow = calc_shadow(frag_pos_light_space);   
	color = (1.0 - shadow) * texture(textureSampler, frag_uv).rgba;
}