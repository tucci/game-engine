#version 330 core


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

uniform mat4 model;
uniform mat4 projection_view;
uniform mat4 light_space_mat;

out vec2 frag_uv;
out vec3 frag_pos;
out vec4 frag_pos_light_space;

void main(){
	
	frag_pos = vec3(model * vec4(position, 1.0f));
	frag_uv = uv;


	frag_pos_light_space = light_space_mat * vec4(frag_pos, 1.0f);
	gl_Position = projection_view * model * vec4(position, 1.0f); 
	
};