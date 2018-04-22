#version 330 core
in vec3 texture_coords;

out vec4 color;

uniform samplerCube texture_cube;

void main(){    
    color = texture(texture_cube, texture_coords);
}
