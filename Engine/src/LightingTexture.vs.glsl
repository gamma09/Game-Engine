#version 400 core

// uniforms
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 world_matrix;
uniform vec3 light_position;
uniform vec4 light_color;

// vertex layout
layout (location = 0) in vec4 vertex;
layout (location = 1) in vec2 texture_coordinate;
layout (location = 2) in vec3 vertex_normal;

// output to the fragment shader
out VS_OUT
{
	vec4 fragment_color;
	vec2 texture_coord;
} vs_out;

void main(void)
{
	// Calculates corrected vertex normals
	mat4 model_view_matrix = view_matrix * world_matrix;
	mat3 model_normal_matrix;
	model_normal_matrix[0] = normalize(model_view_matrix[0].xyz);
	model_normal_matrix[1] = normalize(model_view_matrix[1].xyz);
	model_normal_matrix[2] = normalize(model_view_matrix[2].xyz);
	vec3 normals = normalize(model_normal_matrix * vertex_normal);

	// get light direction and magnitude
	vec4 ecPosition1;
	vec3 ecPosition2;
	ecPosition1 = model_view_matrix * vertex;
	ecPosition2 = ecPosition1.xyz / ecPosition1.w;
	vec3 light_direction = normalize(light_position - ecPosition2);

	// get fragment color
	float fdot = max(0.0, dot(normals, light_direction));
	vs_out.fragment_color.rgb = light_color.rgb * fdot;
	vs_out.fragment_color.a = light_color.a;

	vs_out.texture_coord = texture_coordinate;

	gl_Position = projection_matrix * view_matrix * world_matrix * vertex;
}