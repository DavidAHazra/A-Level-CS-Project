#version 330 core

layout (location = 0) in vec3 position;

out vec3 texture_coords;

uniform mat4 VP;
uniform mat4 model;

void main(){
    gl_Position = VP * model * vec4(position, 1.0f);
    texture_coords = position;
}  
