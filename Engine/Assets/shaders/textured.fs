#version 330 core

in vec3 frag_pos;
in vec2 frag_uv;
in vec3 frag_normal;




in vec4 frag_pos_light_space;

out vec4 frag_color;







uniform sampler2D shadowMap;

uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D metallic_map;
uniform sampler2D roughness_map;
uniform sampler2D ao_map;


uniform samplerCube irradiance_map;
uniform samplerCube prefilter_map;
uniform sampler2D brdf_lut;

uniform vec3 camera_pos;

uniform vec3 light_pos;



const float PI = 3.14159265359;


const float bias = 0.005f;
const float shadow_value = 0.5f;


// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_map, frag_uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(frag_pos);
    vec3 Q2  = dFdy(frag_pos);
    vec2 st1 = dFdx(frag_uv);
    vec2 st2 = dFdy(frag_uv);

    vec3 N   = normalize(frag_normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

// TODO: move to variance shadow mapping
// https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch08.html
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

	 

	
	


	vec3 albedo     = pow(texture(albedo_map, frag_uv).rgb, vec3(2.2));
    float metallic  = texture(metallic_map, frag_uv).r;
    float roughness = texture(roughness_map, frag_uv).r;
	float ao        = texture(ao_map, frag_uv).r;
	vec3 N			= getNormalFromMap();

	// Vector from point to camera
    vec3 V = normalize(camera_pos - frag_pos);
	vec3 R = reflect(-V, N);
	

	

	
    
	// Different BRDF
	// http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    //for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance

		// Incoming light vector
        vec3 L = normalize(light_pos - frag_pos);
		// Half way vector
        vec3 H = normalize(V + L);
		// Calculate attention based on how far the light is
        float distance = length(light_pos - frag_pos);
        float attenuation = 1.0 / (distance * distance);

        vec3 radiance = vec3(10, 10, 10);

		//vec3 radiance = texture(env_map, frag_pos).rgb;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // In order to converse energy, the total enery cannot be greater than 1
		// So once we calculated the spceular component
		// we must subtract that to get our diffuse component
        vec3 kD = vec3(1.0) - kS;

        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  


    vec3 irradiance = texture(irradiance_map, N).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilter_map, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;


	float exposure = 1f;
	color *= exposure;

    // Reinhard HDR tonemapping
    color = color / (color + vec3(1.0));

	

    // gamma 
    color = pow(color, vec3(1.0/2.2)); 

	float shadow = calc_shadow(frag_pos_light_space);  
    frag_color  = (1 - shadow) * vec4(color, 1.0);



	
	//frag_color =  vec4(normalize(frag_normal), 1.0f).rgba; 
	//frag_color =  vec4(N, 1.0f).rgba;
	
	//frag_color =  texture(metallic_map, frag_uv).rgba;
}