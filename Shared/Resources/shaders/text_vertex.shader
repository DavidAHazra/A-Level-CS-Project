#version 330 core

layout (location = 0) in vec4 vertex;
out vec2 texture_coords;

uniform mat4 model;
uniform mat4 VP;

void main(){
	gl_Position = VP * model * vec4(vertex.xy, 0.0f, 1.0f);
    texture_coords = vertex.zw;
}
