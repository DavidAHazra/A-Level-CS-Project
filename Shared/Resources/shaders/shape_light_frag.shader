#version 330 core

struct CustomMaterial {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    lowp float shininess;
};

struct PointLight {
    lowp vec3 position;
	lowp vec3 light_colour;

    lowp float constant_val;
    lowp float linear_val;
    lowp float quadratic;

    lowp vec3 ambient;
    mediump vec3 diffuse;
    lowp vec3 specular;
};

#define LIGHT_COUNT $	// Where '$' will be replaced dynamically in C++ part of program
#if (LIGHT_COUNT > 0)
    // If this is not done, a null light could cause negative-infinity brightness (yikes)
    uniform PointLight lights[LIGHT_COUNT];	// Array of PointLight objects
#endif

in Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
} vertex;

out vec4 colour;

uniform vec3 view_position;
uniform float far_plane;
uniform CustomMaterial material;
uniform samplerCube depth_map;
uniform bool use_shadows;
uniform bool use_light_colour;

const lowp int SAMPLE_SIZE = 20;
const lowp vec3 sample_disk[SAMPLE_SIZE] = vec3[](
   vec3(1, 1, 1),	vec3(1, -1, 1),		vec3(-1, -1, 1),	vec3(-1, 1, 1), 
   vec3(1, 1, -1),	vec3(1, -1, -1),	vec3(-1, -1, -1),	vec3(-1, 1, -1),
   vec3(1, 1, 0),	vec3(1, -1, 0),		vec3(-1, -1, 0),	vec3(-1, 1, 0),
   vec3(1, 0, 1),	vec3(-1, 0, 1),		vec3(1, 0, -1),		vec3(-1, 0, -1),
   vec3(0, 1, 1),	vec3(0, -1, 1),		vec3(0, -1, -1),	vec3(0, 1, -1)
);

float shadow_calculation(const PointLight light) {
    lowp vec3 frag_to_light_vec = vertex.position - light.position;
    lowp float current_depth = length(frag_to_light_vec);

	// PFC Testing
    lowp  float shadow_result = 0.0f;
    lowp float bias = max(0.05f * (1.0f - dot(vertex.normal, normalize(light.position - vertex.position))), 0.5f);
    lowp  float view_distance = length(view_position - vertex.position);
    lowp float disk_radius = (1.0f + (view_distance / far_plane)) / 50.0f;

    for(int sample_iter = 0; sample_iter < SAMPLE_SIZE; ++sample_iter){
        float closest_depth = texture(depth_map, frag_to_light_vec + sample_disk[sample_iter] * disk_radius).r;
        closest_depth *= far_plane;	// Undo linear mapping

        if((current_depth - bias) > closest_depth){ shadow_result += 1.0f; }
    }

    shadow_result /= float(SAMPLE_SIZE);
    return shadow_result;
}

vec3 calculate_point(PointLight light, CustomMaterial material, vec3 normal, vec3 view_dir, int index){
	lowp vec3 light_delta = light.position - vertex.position;
    lowp vec3 light_direction = normalize(light_delta);

	// Diffusion
	lowp float diffusion_factor = max(dot(normal, light_direction), 0.0f);

	// Reflection (Specular Shading)
	lowp vec3 reflection_direction;

    reflection_direction = normalize(light_direction + view_dir);

	lowp float specular_factor = pow(max(dot(view_dir, reflection_direction), 0.0f), material.shininess);

	// Attenuation
	lowp float distance_val = length(light_delta);
	lowp float attenuation = 1.0f / (light.constant_val + light.linear_val * distance_val + light.quadratic * (distance_val * distance_val));

	// Loading model textures
	lowp vec3 diffuse_tex = vec3(texture(material.texture_diffuse, vertex.texture_coords));
	lowp vec3 spec_tex = vec3(texture(material.texture_specular, vertex.texture_coords));
		
	// Phong combination + Add attenuation
	lowp vec3 return_ambience =	light.ambient * diffuse_tex * attenuation;
	lowp vec3 return_diffusion = light.diffuse * diffusion_factor * diffuse_tex * attenuation;
	lowp vec3 return_specular =	light.specular * specular_factor * spec_tex * attenuation;

    // Add light colour if enabled
    if (use_light_colour){
        return_ambience *= light.light_colour;
        return_diffusion *= light.light_colour;
        return_specular *= light.light_colour;
    }

	if ((index == 0) && (use_shadows)){
		lowp float shadow_factor = shadow_calculation(light);
		lowp vec3 lighting = (return_ambience + (1.0f - shadow_factor) * (return_diffusion + return_specular));
		return lighting;

	} else {
		lowp vec3 value = (return_ambience + return_diffusion + return_specular);
        return value;
	}
}

void main() {   
	highp vec3 result = vec3(0.0f);

#if (LIGHT_COUNT > 0)
	for (int light_iter = 0; light_iter < LIGHT_COUNT; ++light_iter){
		result += calculate_point(lights[light_iter], material, normalize(vertex.normal), normalize(view_position - vertex.position), light_iter);
	}

#else
    result = vec3(texture(material.texture_diffuse, vertex.texture_coords));
#endif

	colour = vec4(result, texture(material.texture_diffuse, vertex.texture_coords).a);
}
