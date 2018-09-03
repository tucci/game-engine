#version 330 core

layout (location = 0) in vec3 position;




uniform mat4 projection;
uniform mat4 view;


out vec3 world_pos;



void main()
{
    world_pos = position;

	mat4 rot_view = mat4(mat3(view));
	vec4 clip_pos = projection * rot_view * vec4(position, 1.0);

    gl_Position = clip_pos.xyww;
} 