#version 330 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 in_texture_coords;

uniform mat4 model;
uniform mat4 VP;
out vec2 texture_coords;

void main() {
    gl_Position = VP * model * vec4(position.xy, 0.0f, 1.0f);
    texture_coords = in_texture_coords;
}
