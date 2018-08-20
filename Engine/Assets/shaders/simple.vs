#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
uniform mat4 transform;

out vec3 frag_color;

void main(){
	gl_Position = transform * vec4(position, 1); 
	frag_color = color;
};