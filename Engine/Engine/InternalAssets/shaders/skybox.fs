#version 330 core

in vec3 world_pos;

out vec4 color;

uniform samplerCube skybox;

void main()
{   
	// Sample for env map
	vec3 env_color = texture(skybox, world_pos).rgb;
	// Tone map
	env_color = env_color/ (env_color+ vec3(1.0));
	// Gamma correct
    env_color = pow(env_color, vec3(1.0/2.2)); 

	color = vec4(env_color, 1.0f);
}