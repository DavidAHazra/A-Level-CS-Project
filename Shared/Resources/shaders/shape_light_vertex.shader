#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 in_texture_coords;

out Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
} vertex_in;

uniform mat4 VP;
uniform mat4 model;

void main() {
	gl_Position = VP * model * vec4(position, 1.0f);

	vertex_in.position = vec3(model * vec4(position, 1.0f));
    vertex_in.normal = mat3(transpose(inverse(model))) * normal;
    vertex_in.texture_coords = in_texture_coords;
}
