#include "Transformable.hpp"


void Transformable::move(const vec3& move_vector, const GLfloat& time_delta){
    translation_vector += (move_vector * time_delta);
    matrix_needs_update = true;
}

void Transformable::set_position(const vec3& new_pos){
    translation_vector = new_pos;
    matrix_needs_update = true;
}

void Transformable::enlarge(const vec3& scale_vector){
    enlargement += scale_vector;
    matrix_needs_update = true;
}

void Transformable::set_quaternion(const quat& quaternion) {
	this->quaternion = quaternion;
	matrix_needs_update = true;
}

void Transformable::set_rotation_point(const vec3& new_point){
    rotation_point = new_point;
    matrix_needs_update = true;
}

void Transformable::rotate_x(const float& value) {
	quaternion = rotate(quaternion, radians(value), vec3(1.0f, 0.0f, 0.0f));
	matrix_needs_update = true;
}

void Transformable::rotate_y(const float& value) {
	quaternion = rotate(quaternion, radians(value), vec3(0.0f, 1.0f, 0.0f));
	matrix_needs_update = true;
}

void Transformable::rotate_z(const float& value) {
	quaternion = rotate(quaternion, radians(value), vec3(0.0f, 0.0f, 1.0f));
	matrix_needs_update = true;
}

void Transformable::rotate_all_axis(const float& x_rot, const float& y_rot, const float& z_rot) {
	rotate_x(x_rot);
	rotate_y(y_rot);
	rotate_z(z_rot);
}

mat4 Transformable::get_model_matrix() {
    if (!matrix_needs_update){ return model_matrix; }
    matrix_needs_update = false;
    
    // Order: Translate, Rotate, Scale
    mat4 translation = translate(EngineConstants::IDENTITY_MATRIX, translation_vector);
    
    mat4 rotation = translate(EngineConstants::IDENTITY_MATRIX, -get_rotation_point());
    rotation = rotation * to_mat4(get_quaternion());
    rotation = translate(rotation, get_rotation_point());
    
    mat4 scale_matrix = scale(EngineConstants::IDENTITY_MATRIX, enlargement);
    
    model_matrix = translation * rotation * scale_matrix;
    return model_matrix;
}

std::array<vec3, 3> Transformable::get_local_axis(){
    mat4 rotation_matrix = to_mat4(get_quaternion());
    
    std::array<vec3, 3> new_axis = {
        to_vec3(rotation_matrix * vec4(1.0f, 0.0f, 0.0f, 1.0f)),
        to_vec3(rotation_matrix * vec4(0.0f, 1.0f, 0.0f, 1.0f)),
        to_vec3(rotation_matrix * vec4(0.0f, 0.0f, 1.0f, 1.0f))
    };
    
    return new_axis;
}
