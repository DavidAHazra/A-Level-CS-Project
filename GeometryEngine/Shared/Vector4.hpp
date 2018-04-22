#pragma once
#include "Vector3.hpp"

template <typename T>
struct Vector4 {
private:
	typedef const T& Type;
	typedef const Vector4<T>& Other;
	typedef Vector4<T>& Self;

public:
	T x;
	T y;
	T z;
	T w;

	inline Vector4() : x(0), y(0), z(0), w(0) {}
	inline Vector4(Type scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
	inline Vector4(Type x, Type y, Type z, Type w) : x(x), y(y), z(z), w(w) {}
	inline Vector4(const Vector3<T>& vector, Type w) : x(vector.x), y(vector.y), z(vector.z), w(w) {}
	inline Vector4(Other other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	// Arithmetic Operators
	inline Self operator=(Other other) {
		x = other.x; y = other.y; z = other.z; w = other.w; return *this;
	}

	inline Vector4<T> operator+(Other other) const {
		return Vector4<T>(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	inline Vector4<T> operator-(Other other) const {
		return Vector4<T>(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	inline Vector4<T> operator+() const {
		return Vector4<T>(x, y, z, w);
	}

	inline Vector4<T> operator-() const {
		return Vector4<T>(-x, -y, -z, -w);
	}

	inline Vector4<T> operator*(Type scalar) const {
		return Vector4<T>(x * scalar, y * scalar, z * scalar, w * scalar);
	}

	inline Vector4<T> operator/(Type scalar) const {
		return Vector4<T>(x / scalar, y / scalar, z / scalar, w / scalar);
	}

	// Comparison Operators
	inline bool operator==(Other other) const {
		return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
	}

	inline bool operator!=(Other other) const {
		// Simplified using DeMorgan's Law ^:)
		return (x != other.x) || (y != other.y) || (z != other.z) || (w != other.w);
	}

	// Compound Assignment Operators
	inline Self operator+=(Other other) {
		x += other.x; y += other.y; z += other.z; w += other.w; return *this;
	}

	inline Self operator-=(Other other) {
		x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this;
	}

	inline Self operator*=(Type scalar) {
		x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this;
	}

	inline Self operator/=(Type scalar) {
		x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this;
	}

	// Access Operators
	inline T& operator[](const unsigned long& index) {
		return (&x)[index];
	}

	inline const T& operator[](const unsigned long& index) const {
		return (&x)[index];
	}
};