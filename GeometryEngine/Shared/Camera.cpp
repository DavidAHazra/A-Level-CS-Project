#include "Camera.hpp"


void Camera::move(const GLfloat& time_delta, const GLfloat& amount_right, const GLfloat& amount_up, const GLfloat& amount_forward) {
	position += normalise(cross(front_vector, UP_VECTOR)) * amount_right * time_delta;
	position += UP_VECTOR * amount_up * time_delta;
	position += front_vector * amount_forward * time_delta;
}

void Camera::calculate_front_vector(){
    const float local_pitch = radians(pitch);
	const float local_yaw = radians(yaw);
    
    front_vector = normalise(vec3(
                        std::cos(local_yaw) * std::cos(local_pitch),	// x
                        std::sin(local_pitch),                          // y
                        std::sin(local_yaw) * std::cos(local_pitch)     // z
                    ));
}

void Camera::handle_mouse(const float& new_x_pos, const float& new_y_pos) {
	pitch += (previous_mouse_position.y - new_y_pos) * MOUSE_SENSITIVITY;	// Y Reversed since values range top -> bottom
	yaw += (new_x_pos - previous_mouse_position.x) * MOUSE_SENSITIVITY;

	if (pitch > PITCH_MAX) { pitch = PITCH_MAX; }			// Looks weird otherwise
	else if (pitch < -PITCH_MAX) { pitch = -PITCH_MAX; }
    
	previous_mouse_position.x = new_x_pos;
	previous_mouse_position.y = new_y_pos;

    calculate_front_vector();
}
