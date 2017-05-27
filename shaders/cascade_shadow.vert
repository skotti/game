#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coords;

const int NUM_CASCADES = 3;

out VS_OUT {
	vec3 model_pos;
	vec3 normal;
	vec2 tex_coords;
	vec4 light_pos[NUM_CASCADES];
	float obj_depth;
} vs_out;

out vec4 points_edge[8];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 points[8];

uniform mat4 light_view[NUM_CASCADES];
out vec4 l_coord;

void main() {
    for (int i = 0; i < NUM_CASCADES; i++) {
        vs_out.light_pos[i] = light_view[i] * model * vec4(position, 1.0f);
    }
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.normal = transpose(inverse(mat3(model))) * normal;
    vs_out.tex_coords = tex_coords;
    vs_out.model_pos = vec3(model * vec4(position, 1.0));
    vs_out.obj_depth = gl_Position.z;// / gl_Position.w;
    for (int i = 0; i < 8; i++)
			points_edge[i] = points[i];
		l_coord = gl_Position;
}