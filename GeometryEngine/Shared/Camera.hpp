#pragma once
#include "EngineHeader.hpp"


class Camera {
	const float PITCH_MAX = 89.0f;
	const float MOUSE_SENSITIVITY = 0.05f;
	const vec3 UP_VECTOR = vec3(0.0f, 1.0f, 0.0f);

public:
	inline mat4 get_view() const { return look_at(position, position + front_vector, UP_VECTOR); }
    void calculate_front_vector();

	inline mat4 get_custom_view() const { return custom_view; }
	inline void set_custom_view(const mat4& view) { custom_view = view; }

	// Position
	inline vec3 get_position() const { return position; }
	inline void set_position(const vec3& new_position) { position = new_position; }

	inline vec3 get_front_vector() { calculate_front_vector(); return front_vector; }
	
	inline float get_pitch() const { return pitch; }
	inline float get_yaw() const { return yaw; }

	inline void reset() { pitch = 0.0f; yaw = -90.0f; calculate_front_vector(); }
    inline vec2 get_previous_coords() { return previous_mouse_position; }
    
private:
	float pitch = 0.0f;
	float yaw = -90.0f;
	vec3 position = vec3(0.0f, 0.0f, 3.0f);
	vec3 front_vector = vec3(0.0f, 0.0f, -1.0f);

    vec2 previous_mouse_position;
	mat4 custom_view;

public:
    // Public non-static member functions
	void handle_mouse(const float& new_x_pos, const float& new_y_pos);
	void move(const float& time_delta, const float& amount_right = 0.0f,
              const float& amount_up = 0.0f, const float& amount_forward = 0.0f);
};
