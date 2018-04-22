#pragma once

#include "Renderable.hpp"

class Transformable : public Renderable {
public:
    Transformable(const Transformable& other) = delete;
    void operator=(const Transformable& other) = delete;
    
public:
    inline Transformable() : Renderable() {}
	virtual inline ~Transformable() {}
    
    // Position
    // ======== ======== ======== =========
    
	inline vec3 get_position() { return to_vec3(get_model_matrix() * vec4(0.0f, 0.0f, 0.0f, 1.0f)); }
    inline vec3 get_move_vector() const { return move_vector; }
    
    void set_position(const vec3& new_pos);
    inline void set_move_vector(const vec3& new_vector){ move_vector = new_vector; }
    
    void move(const vec3& move_vector, const float& time_delta = 1.0f);
    inline void move_by_vector(const float& time_delta = 1.0f) { move(move_vector, time_delta); }
    
    // ======== ======== ======== =========
    // Rotation
    // ======== ======== ======== =========
    
	void set_quaternion(const quat& quaternion);
    inline quat get_quaternion() const { return quaternion; }
	inline quat get_rotation() const { return get_quaternion(); }
    inline vec3 get_rotation_point() const { return rotation_point; }
    
    void set_rotation_point(const vec3& new_point);
	void rotate_x(const float& value);
	void rotate_y(const float& value);
	void rotate_z(const float& value);
	void rotate_all_axis(const float& x_rot, const float& y_rot, const float& z_rot);
    
    // ======== ======== ======== =========
    // Enlargement
    // ======== ======== ======== =========
    
    inline vec3 get_enlargement() const { return enlargement; }
    inline void set_enlargement(const vec3 new_enlarge) { enlargement = new_enlarge; }
    void enlarge(const vec3& scale_vector);
    inline void enlarge(const GLfloat& scale_factor) { enlarge(vec3(scale_factor)); }
    
    // ======== ======== ======== =========
    
    // Model view
    mat4 get_model_matrix();
    std::array<vec3, 3> get_local_axis();
    
protected:
    vec3 translation_vector;
    
	quat quaternion;
	vec3 rotation_point;

    vec3 move_vector;
    vec3 enlargement = vec3(1.0f);
    
    mat4 model_matrix = EngineConstants::IDENTITY_MATRIX;
    bool matrix_needs_update = true;
};

