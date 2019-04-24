#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
uniform mat4 transform;
out vec4 vertexColor;

void main(){
	gl_Position = transform * vec4(position, 1);
	vertexColor = vec4(color, 1);
}