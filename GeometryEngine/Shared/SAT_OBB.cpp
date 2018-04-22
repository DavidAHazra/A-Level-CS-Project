#include "SAT_OBB.hpp"


SAT_OBB::SAT_OBB(const std::vector<vec3>& original_vertices) {
    const size_t shape_size = original_vertices.size();
    
    if (shape_size < 3){
        throw std::runtime_error("Model vertices supplied to SAT_OBB constructor has less than 3 vertices");
    }
    
    std::vector<float> x_values;
    std::vector<float> y_values;
    std::vector<float> z_values;
    
    for (size_t iter = 0; iter < shape_size; ++iter){
        const vec3 vertex = original_vertices.at(iter);
        x_values.push_back(vertex.x);
        y_values.push_back(vertex.y);
        z_values.push_back(vertex.z);
    }
    
    aabb_min.x = *std::min_element(x_values.begin(), x_values.end());
    aabb_min.y = *std::min_element(y_values.begin(), y_values.end());
    aabb_min.z = *std::min_element(z_values.begin(), z_values.end());
    
    aabb_max.x = *std::max_element(x_values.begin(), x_values.end());
    aabb_max.y = *std::max_element(y_values.begin(), y_values.end());
    aabb_max.z = *std::max_element(z_values.begin(), z_values.end());
}

void SAT_OBB::operator=(const SAT_OBB& other){
    this->aabb_min = other.aabb_min;
    this->aabb_max = other.aabb_max;
}

bool SAT_OBB::does_collide(const mat4& this_matrix, const std::array<vec3, 3>& this_axis,
                           const SAT_OBB& other,
                           const mat4& other_matrix, const std::array<vec3, 3>& other_axis) const {

    
    // Sphere collision is a lot faster, if spheres collides: check with more accurate collision detection
	const vec3 Pa = get_center(this_matrix);
	const vec3 Pb = get_center(other_matrix);

    if (!does_sphere_collide(Pa, Pb, other)) { return false; }
    
    const vec3 obb_min = to_vec3(this_matrix * vec4(aabb_min, 1.0f));
    const vec3 obb_max = to_vec3(this_matrix * vec4(aabb_max, 1.0f));
    
    const vec3 other_obb_min = to_vec3(other_matrix * vec4(other.aabb_min, 1.0f));
    const vec3 other_obb_max = to_vec3(other_matrix * vec4(other.aabb_max, 1.0f));
    
    const vec3 axis_delta_A = ((obb_max - obb_min) / 2.0f);
    const vec3 axis_delta_B = ((other_obb_max - other_obb_min) / 2.0f);
    
    const vec3 Ax = this_axis.at(0);
    const vec3 Ay = this_axis.at(1);
    const vec3 Az = this_axis.at(2);
    
    const vec3 Bx = other_axis.at(0);
    const vec3 By = other_axis.at(1);
    const vec3 Bz = other_axis.at(2);
    
    // Calculations
    const vec3 T = Pb - Pa;
    
    const std::array<vec3, 15> Ls = {
        Ax, Ay, Az,
        Bx, By, Bz,
        cross(Ax, Bx),
        cross(Ax, By),
        cross(Ax, Bz),
        cross(Ay, Bx),
        cross(Ay, By),
        cross(Ay, Bz),
        cross(Az, Bx),
        cross(Az, By),
        cross(Az, Bz)
    };
    
    for (size_t iter = 0; iter < Ls.size(); ++iter){
        const vec3 L = Ls.at(iter);
        const float T_dot_L = std::abs(dot(T, L));

		const float comparison_object =
			std::abs(dot(Ax * axis_delta_A.x, L)) +
			std::abs(dot(Ay * axis_delta_A.y, L)) +
			std::abs(dot(Az * axis_delta_A.z, L)) +
			std::abs(dot(Bx * axis_delta_B.x, L)) +
			std::abs(dot(By * axis_delta_B.y, L)) +
			std::abs(dot(Bz * axis_delta_B.z, L));

        
        // If T_dot_L > comparison_object: no collision
        if (T_dot_L > comparison_object){
            return false;
        }
    }
    
    return true;
}

bool SAT_OBB::does_sphere_collide(const vec3 this_pos, const vec3 other_pos, const SAT_OBB& other) const {
    const float this_radius = distance(aabb_min, aabb_max) / 2.0f;
    const float other_radius = distance(other.aabb_min, other.aabb_max) / 2.0f;

    return distance(this_pos, other_pos) <= (this_radius + other_radius);
}
