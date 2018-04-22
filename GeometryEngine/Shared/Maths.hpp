#pragma once
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Matrix4x4.hpp"
#include "Quaternion.hpp"

typedef Vector2<float> vec2;
typedef Vector3<float> vec3;
typedef Vector4<float> vec4;
typedef Matrix4x4<float> mat4;
typedef Quaternion<float> quat;

// === Constants ===
namespace Constants {
	static const float EPSILON = 0.01f;
	static const float EPSILON_2 = 0.0001f;
	static const float PI_OVER_180 = 0.01745329251f;
}

// === Maths Functions that use all 4 of the structs above ===
template <typename T>
inline T length(const Vector2<T>& vector) {
	return std::sqrt((vector.x * vector.x) + (vector.y * vector.y));
}

template <typename T>
inline T length(const Vector3<T>& vector) {
	return std::sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));
}

template <typename T>
inline T length(const Vector4<T>& vector) {
	return std::sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z) + (vector.w * vector.w));
}

template <typename T>
inline T magnitude(const Vector3<T>& vector) {
	return length<T>(vector);
}

template <typename T>
inline T distance(const Vector2<T>& vector1, const Vector2<T>& vector2) {
	return length<T>(vector2 - vector1);
}

template <typename T>
inline T distance(const Vector3<T>& vector1, const Vector3<T>& vector2) {
	return length<T>(vector2 - vector1);
}

template <typename T>
inline T distance(const Vector4<T>& vector1, const Vector4<T>& vector2) {
	return length<T>(vector2 - vector1);
}

template <typename T>
inline T magnitude(const Vector4<T>& vector) {
	return length<T>(vector);
}

template <typename T>
inline Vector3<T> normalise(const Vector3<T>& vector) {
	return vector / magnitude<T>(vector);
}

template <typename T>
inline Vector4<T> normalise(const Vector4<T>& vector) {
	return vector / magnitude<T>(vector);
}

template <typename T>
inline T dot(const Vector3<T>& vector1, const Vector3<T>& vector2) {
	return (vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z);
}

template <typename T>
inline T dot(const Vector4<T>& vector1, const Vector4<T>& vector2) {
	return (vector1.x * vector2.x) + (vector1.y * vector2.y) + (vector1.z * vector2.z) + (vector1.w * vector2.w);
}

template <typename T>
inline T dot(const Quaternion<T>& quat1, const Quaternion<T>& quat2) {
	return quat1.x * quat2.x + quat1.y * quat2.y + quat1.z * quat2.z + quat1.w * quat2.w;
}

template <typename T>
inline Vector3<T> cross(const Vector3<T>& vector1, const Vector3<T>& vector2) {
	return Vector3<T>(
			(vector1.y * vector2.z) - (vector2.y * vector1.z),
			(vector1.z * vector2.x) - (vector2.z * vector1.x),
			(vector1.x * vector2.y) - (vector2.x * vector1.y)
		);
}

template <typename T>
inline const T* value_ptr(const Vector3<T>& vector) {
	return &(vector.x);
}

template <typename T>
inline T* value_ptr(const Vector3<T>& vector) {
	return &(vector.x);
}

template <typename T>
inline T* value_ptr(Matrix4x4<T>& matrix) {
	return &(matrix[0].x);
}

template <typename T>
inline const T* value_ptr(const Matrix4x4<T>& matrix) {
	return &(matrix[0].x);
}

template <typename T>
inline Vector3<T> to_vec3(const Vector4<T>& vector) {
	return Vector3<T>(vector.x, vector.y, vector.z) / vector.w;
}

template <typename T>
inline T length_squared(const Vector3<T>& vector) {
	return dot<T>(vector, vector);
}

template <typename T>
inline T length_squared(const Vector4<T>& vector) {
	return dot<T>(vector, vector);
}

template <typename T>
inline T radians(const T& degrees) {
	return degrees * static_cast<T>(Constants::PI_OVER_180);
}

template <typename T>
inline T mix(const T& x, const T& y, const T& a) {
	return static_cast<T>(x + a * (y - x));
}

template <typename T>
Matrix4x4<T> look_at(const Vector3<T>& eye, const Vector3<T>& center, const Vector3<T>& up) {
	const Vector3<T> f(normalise<T>(center - eye));
	const Vector3<T> s(normalise<T>(cross<T>(f, up)));
	const Vector3<T> u(cross<T>(s, f));

	Matrix4x4<T> result(1);
	result[0][0] = s.x;
	result[1][0] = s.y;
	result[2][0] = s.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] = -f.x;
	result[1][2] = -f.y;
	result[2][2] = -f.z;
	result[3][0] = -dot<T>(s, eye);
	result[3][1] = -dot<T>(u, eye);
	result[3][2] = dot<T>(f, eye);
	return result;
}

template <typename T>
Quaternion<T> quat_from_angle_axis(const T& angle, const Vector3<T>& axis) {
	const T s = std::sin(angle / static_cast<T>(2));

	return Quaternion<T>(
		std::cos(angle / static_cast<T>(2)),
		axis.x * s,
		axis.y * s,
		axis.z * s
		);
}

template <typename T>
Matrix4x4<T> ortho(const T& left, const T& right, const T& bottom, const T& top) {
	Matrix4x4<T> result(1);
	result[0][0] = static_cast<T>(2) / (right - left);
	result[1][1] = static_cast<T>(2) / (top - bottom);
	result[2][2] = -static_cast<T>(1);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);
	return result;
}

template <typename T>
Matrix4x4<T> perspective(const T& fov, const T& aspect_ratio, const T& near_plane, const T& far_plane) {
	const T theta = std::tan(fov / static_cast<T>(2));

	Matrix4x4<T> result(static_cast<T>(0));
	result[0][0] = static_cast<T>(1) / (aspect_ratio * theta);
	result[1][1] = static_cast<T>(1) / (theta);
	result[2][2] = -(far_plane + near_plane) / (far_plane - near_plane);
	result[2][3] = -static_cast<T>(1);
	result[3][2] = -(static_cast<T>(2) * far_plane * near_plane) / (far_plane - near_plane);

	return result;
}

template <typename T>
Quaternion<T> rotate(const Quaternion<T>& quat, const T& angle, const Vector3<T>& axis) {
	Vector3<T> temp = axis;

	T len = length<T>(temp);

	if (std::abs(len - static_cast<T>(1)) > static_cast<T>(Constants::EPSILON)) {
		T len_recip = static_cast<T>(1) / len;
		temp *= len_recip;
	}

	const T sin_theta = std::sin(angle / static_cast<T>(2));
	temp *= sin_theta;

	return quat * Quaternion<T>(std::cos(angle / static_cast<T>(2)), temp.x, temp.y, temp.z);
}

template <typename T>
Matrix4x4<T> translate(const Matrix4x4<T>& matrix, const Vector3<T>& vector) {
	Matrix4x4<T> result(matrix);
	result[3] = matrix[0] * vector[0] + matrix[1] * vector[1] + matrix[2] * vector[2] + matrix[3];
	return result;
}

template <typename T>
Matrix4x4<T> to_mat4(const Quaternion<T>& quat) {
	Matrix4x4<T> result(1);

	T qxx(quat.x * quat.x);
	T qyy(quat.y * quat.y);
	T qzz(quat.z * quat.z);
	T qxz(quat.x * quat.z);
	T qxy(quat.x * quat.y);
	T qyz(quat.y * quat.z);
	T qwx(quat.w * quat.x);
	T qwy(quat.w * quat.y);
	T qwz(quat.w * quat.z);

	result[0][0] = 1 - 2 * (qyy + qzz);
	result[0][1] = 2 * (qxy + qwz);
	result[0][2] = 2 * (qxz - qwy);

	result[1][0] = 2 * (qxy - qwz);
	result[1][1] = 1 - 2 * (qxx + qzz);
	result[1][2] = 2 * (qyz + qwx);

	result[2][0] = 2 * (qxz + qwy);
	result[2][1] = 2 * (qyz - qwx);
	result[2][2] = 1 - 2 * (qxx + qyy);
	return result;
}

template <typename T>
Matrix4x4<T> scale(const Matrix4x4<T>& matrix, const Vector3<T>& vector) {
	Matrix4x4<T> result(0);
	result[0] = matrix[0] * vector[0];
	result[1] = matrix[1] * vector[1];
	result[2] = matrix[2] * vector[2];
	result[3] = matrix[3];
	return result;
}

template <typename T>
Quaternion<T> SLERP(const Quaternion<T>& quat1, const Quaternion<T>& quat2, const T& a) {
	Quaternion<T> temp = quat2;
	T cos_theta = dot(quat1, quat2);

	if (cos_theta < static_cast<T>(0)) {
		temp = -quat2;
		cos_theta = -cos_theta;
	}

	if (cos_theta > (static_cast<T>(1) - Constants::EPSILON)) {
		return Quaternion<T>(
			mix(quat1.w, temp.w, a),
			mix(quat1.x, temp.x, a),
			mix(quat1.y, temp.y, a),
			mix(quat1.z, temp.z, a)
			);
	} else {
		const T angle = std::acos(cos_theta);
		const T f = std::sin((static_cast<T>(1) - a) * angle);
		const T u = std::sin(a * angle);

		return quat1 * (temp * u + f) / sin(angle);
	}
}
