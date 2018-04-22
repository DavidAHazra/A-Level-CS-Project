#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadow_matrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main(){
    for(int face = 0; face < 6; ++face) {	// Cube (6 faces)
        gl_Layer = face; // Defines the face to render

        for(int triangle_vertex = 0; triangle_vertex < 3; ++triangle_vertex) {	// Take triangle as input (3 vertices)
            FragPos = gl_in[triangle_vertex].gl_Position;
            gl_Position = shadow_matrices[face] * FragPos;
            EmitVertex();
        }    

        EndPrimitive();
    }
}  