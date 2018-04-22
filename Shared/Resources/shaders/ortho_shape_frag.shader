#version 330 core

struct CustomMaterial {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    lowp float shininess;
};

in vec2 texture_coords;

out vec4 colour;

uniform CustomMaterial material;


void main() {
    colour = vec4(vec3(texture(material.texture_diffuse, texture_coords)), 1.0f);
}
