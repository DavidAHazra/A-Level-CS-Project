#pragma once
#include "Vector3.hpp"


template <typename T>
struct Quaternion {
private:
	typedef const T& Type;
	typedef const Quaternion<T>& Other;
	typedef Quaternion<T>& Self;

public:
	T w;
	T x;
	T y;
	T z;

	inline Quaternion() : w(1), x(0), y(0), z(0) {}
	inline Quaternion(Type scalar) : w(scalar), x(scalar), y(scalar), z(scalar) {}
	inline Quaternion(Type w, Type x, Type y, Type z) : w(w), x(x), y(y), z(z) {}
	inline Quaternion(Other other) : w(other.w), x(other.x), y(other.y), z(other.z) {}
	Quaternion(const Vector3<T>& vector) {
		const Vector3<T> temp = vector / static_cast<T>(2);
		Vector3<T> first(std::cos(temp.x), std::cos(temp.y), std::cos(temp.z));
		Vector3<T> second(std::sin(temp.x), std::sin(temp.y), std::sin(temp.z));

		w = first.x * first.y * first.z + second.x * second.y * second.z;
		x = second.x * first.y * first.z - first.x * second.y * second.z;
		y = first.x * second.y * first.z + second.x * first.y * second.z;
		z = first.x * first.y * second.z - second.x * second.y * first.z;
	}

	// Arithmetic Operators
	inline Self operator=(Other other) {
		w = other.w; x = other.x; y = other.y; z = other.z; return *this;
	}

	Vector3<T> operator*(const Vector3<T>& vector) const {
		float num = x * static_cast<T>(2);
		float num2 = y * static_cast<T>(2);
		float num3 = z * static_cast<T>(2);
		float num4 = x * num;
		float num5 = y * num2;
		float num6 = z * num3;
		float num7 = x * num2;
		float num8 = x * num3;
		float num9 = y * num3;
		float num10 = w * num;
		float num11 = w * num2;
		float num12 = w * num3;

		return Vector3<T>(
			(static_cast<T>(1) - (num5 + num6)) * vector.x + (num7 - num12) * vector.y + (num8 + num11) * vector.z,
			(num7 + num12) * vector.x + (static_cast<T>(1) - (num4 + num6)) * vector.y + (num9 - num10) * vector.z,
			(num8 - num11) * vector.x + (num9 + num10) * vector.y + (static_cast<T>(1) - (num4 + num5)) * vector.z
			);
	}

	Quaternion<T> operator*(Other other) const {
		return Quaternion<T>(
			-x * other.x - y * other.y - z * other.z + w * other.w,
			x * other.w + y * other.z - z * other.y + w * other.x,
			-x * other.z + y * other.w + z * other.x + w * other.y,
			x * other.y - y * other.x + z * other.w + w * other.z
			);
	}

	inline Quaternion<T> operator-() const {
		return Quaternion<T>(-w, -x, -y, -z);
	}

	inline Quaternion<T> operator*(Type scalar) const {
		return Quaternion<T>(w * scalar, x * scalar, y * scalar, z * scalar);
	}

	inline Quaternion<T> operator/(Type scalar) const {
		return Quaternion<T>(w / scalar, x / scalar, y / scalar, z / scalar);
	}

	inline Quaternion<T> operator+(Other other) const {
		return Quaternion<T>(
			w + other.w,
			x + other.x,
			y + other.y,
			z + other.z
			);
	}

	//inline Quaternion<T> operator+(Other other) {
	//	return Quaternion<T>(x + other.x, y + other.y, z + other.z);
	//}

	//inline Quaternion<T> operator-(Other other) {
	//	return Quaternion<T>(x - other.x, y - other.y, z - other.z);
	//}

	//inline Quaternion<T> operator+() {
	//	return Quaternion<T>(x, y, z);
	//}

	//inline Quaternion<T> operator-() {
	//	return Quaternion<T>(-x, -y, -z);
	//}

	//inline Quaternion<T> operator*(Type scalar) {
	//	return Quaternion<T>(x * scalar, y * scalar, z * scalar);
	//}

	//inline Quaternion<T> operator/(Type scalar) {
	//	return Quaternion<T>(x / scalar, y / scalar, z / scalar);
	//}

	//// Comparison Operators
	//inline bool operator==(Other other) {
	//	return (x == other.x) && (y == other.y) && (z == other.z);
	//}

	//inline bool operator!=(Other other) {
	//	// Simplified using DeMorgan'second Law ^:)
	//	return (x != other.x) || (y != other.y) || (z != other.z);
	//}

	//// Compound Assignment Operators
	//inline Self operator+=(Other other) {
	//	x += other.x; y += other.y; z += other.z; return *this;
	//}

	//inline Self operator-=(Other other) {
	//	x -= other.x; y -= other.y; z -= other.z; return *this;
	//}

	//inline Self operator*=(Type scalar) {
	//	x *= scalar; y *= scalar; z *= scalar; return *this;
	//}

	//inline Self operator/=(Type scalar) {
	//	x /= scalar; y /= scalar; z /= scalar; return *this;
	//}
};

typedef Quaternion<float> quat;