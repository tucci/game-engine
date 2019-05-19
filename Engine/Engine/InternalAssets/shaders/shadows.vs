#version 330 core
layout (location = 0) in vec3 pos;



uniform mat4 model;
uniform mat4 projection_view;

void main()
{
    gl_Position = projection_view * model * vec4(pos, 1.0);
}