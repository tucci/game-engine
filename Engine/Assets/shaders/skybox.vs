#version 330 core

layout (location = 0) in vec3 position;
out vec3 texcoords;

uniform mat4 transform;
const float skybox_size = 100.0;
void main()
{
    texcoords = position;
    gl_Position = transform * vec4(skybox_size  * position, 1.0);
} 