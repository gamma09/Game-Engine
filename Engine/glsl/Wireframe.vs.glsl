#version 400 core

in vec4 position;

uniform mat4 world_matrix;
uniform mat4 projection_view_matrix;

void main(void)
{
    gl_Position = projection_view_matrix * world_matrix * position;
}