#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

uniform float time;
uniform bool do_explode;

in Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
} vertex_in[3];

out Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
} vertex;

vec3 get_normal(){
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal){
	const lowp float magnitude = 5.0f;
    lowp vec3 direction = normal * time * magnitude;
    return position + vec4(direction, 0.0f);
}

void main(){
    for (int i = 0; i < gl_in.length(); ++i){
		vec4 pos = gl_in[i].gl_Position;

		if (do_explode){
			pos = explode(gl_in[i].gl_Position, get_normal());
		}

		gl_Position = pos;

        vertex.position = vertex_in[i].position;
        vertex.normal = vertex_in[i].normal;
        vertex.texture_coords = vertex_in[i].texture_coords;

        EmitVertex();
    }

    EndPrimitive();
}
