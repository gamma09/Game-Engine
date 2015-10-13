#version 400 core

// Uniforms
uniform sampler2D texture_unit;

// Varying
in VS_OUT
{
	vec4 fragment_color;
	vec2 texture_coord;
} fs_in;

out vec4 color;

void main(void) 
{ 
	color = fs_in.fragment_color * texture(texture_unit, fs_in.texture_coord);
}