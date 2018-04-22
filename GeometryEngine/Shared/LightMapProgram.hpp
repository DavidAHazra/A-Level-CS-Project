#pragma once

#include "Program.hpp"

struct PointLight {
	vec3 position;
	vec3 light_colour;

	float constant_val;
	float linear_val;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

class LightMapProgram : public Program {
    // Very specialised type of program, needs to dynamically adapt to current scene contexts
	// THIS PROGRAM MUST ALSO INCLUDE SHADOWS
    
    /*
	Class able to parse:
	
	...
	uniform LightPoint[$] light_points
	...
	
	This class will substitute the '#' for the number of lights
	Requirements: A LightPoint struct in the shader
	*/

public:
    LightMapProgram(const LightMapProgram& other) = delete;
    void operator=(const LightMapProgram& other) = delete;

    LightMapProgram(const std::string& vertex_shader_path,
                    const std::string& fragment_shader_path,
                    const std::string& geometry_shader_path = std::string());
	virtual ~LightMapProgram();

    inline size_t get_light_count() const { return lights.size(); }
	inline PointLight* get_light(const size_t& index) const { return lights.at(index); }
	void add_light(PointLight* light);
	void set_light_position(const UInt& light_index, const vec3& new_pos);
    void set_light_colour(const UInt& light_index, const vec3& colour);

	void set_light_count();
	void reset();
    
private:
	std::vector<PointLight*> lights;
    
    std::string vertex_path;
    std::string fragment_path;
    std::string geometry_path;
};

