#pragma once
#include "EngineHeader.hpp"
#include "Shape.hpp"


class SAT_OBB {
    // Seperated-Axis-Theorem Oriented Bounding Box
    // Class used for detecting collisions between oriented cuboids
    
public:
    // Constructors
    SAT_OBB(const std::vector<vec3>& original_vertices);
    inline SAT_OBB(const SAT_OBB& other) : aabb_min(other.aabb_min), aabb_max(other.aabb_max) {}
    void operator=(const SAT_OBB& other);
    
    // Deleted Constructors
    SAT_OBB() = delete;

public:
    // Public Member Functions
    bool does_collide(const mat4& this_matrix, const std::array<vec3, 3>& this_axis,
                      const SAT_OBB& other,
                      const mat4& other_matrix, const std::array<vec3, 3>& other_axis) const;
    
    bool does_sphere_collide(const vec3 this_pos, const vec3 other_pos, const SAT_OBB& other) const;
    
    inline vec3 get_aabb_min() const { return aabb_min; }
    inline vec3 get_aabb_max() const { return aabb_max; }
    
public:
    // Static Functions
    static inline vec3 get_center(const mat4& matrix) { return to_vec3(matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f)); }
    static inline vec3 get_real_axis(const mat4& matrix, const vec3& axis) {
        return normalise(to_vec3(matrix * vec4(axis, 0.0f)));
    }
    
private:
    // Private Member Variables
    vec3 aabb_min;
    vec3 aabb_max;
};


struct Collidable  {
    // Struct made for convinient useage of SAT_OBB class
    
    static Collidable make_collidable(Shape& shape){
        return Collidable {
            SAT_OBB(shape.get_personal_vertices()),
            shape.get_model_matrix(),
            shape.get_local_axis()
        };
    }
    
    SAT_OBB bounding_box;
    mat4 model_matrix;
    std::array<vec3, 3> local_axis;
};
