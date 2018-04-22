#pragma once

#include "Transformable.hpp"
#include "ResourceHandler.hpp"


class Shape : public Transformable {
public:
    inline static SHADER_ID GENERIC_ID() { return "Shape"; };
    inline virtual SHADER_ID get_identifier(){ return Shape::GENERIC_ID(); }
    
public:
	Shape(const Shape& other) = delete;
	void operator=(const Shape& other) = delete;

	inline virtual ~Shape() {}
    virtual void render(const SHADER_ID& id) = 0;                                                   
    virtual std::vector<vec3> get_personal_vertices();

    static void add_texture_to_resource_handler(const std::string& identifier, const UInt& id);
    static UInt load_texture_from_rgba(const float& red, const float& green, const float& blue, const float& alpha = 1.0f);
	static UInt load_texture_from_file(const std::string& path, const std::string& directory);
    inline static UInt load_texture_from_rgba(const Colour& colour) {
        return load_texture_from_rgba(colour.red, colour.green, colour.blue, colour.alpha);
    }
    
    vec3 get_vertex_position(const size_t& index);
    
    
protected:
	inline Shape() : Transformable() {}
	virtual void evaluate_changed() = 0;
    
    std::vector<float> vertices;
};

