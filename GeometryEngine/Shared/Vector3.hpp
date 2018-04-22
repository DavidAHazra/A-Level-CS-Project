#pragma once


template <typename T>
struct Vector3 {
private:
    typedef const T& Type;
    typedef const Vector3<T>& Other;
	typedef Vector3<T>& Self;
    
public:
    T x;
    T y;
    T z;
    
	inline Vector3() : x(0), y(0), z(0) {}
    inline Vector3(Type scalar) : x(scalar), y(scalar), z(scalar) {}
    inline Vector3(Type x, Type y, Type z) : x(x), y(y), z(z) {}
    inline Vector3(Other other) : x(other.x), y(other.y), z(other.z) {}

	// Arithmetic Operators
	inline Self operator=(Other other) {
		x = other.x; y = other.y; z = other.z; return *this;
	}

	inline Vector3<T> operator+(Other other) const {
		return Vector3<T>(x + other.x, y + other.y, z + other.z);
	}

	inline Vector3<T> operator-(Other other) const {
		return Vector3<T>(x - other.x, y - other.y, z - other.z);
	}

	inline Vector3<T> operator+() const {
		return Vector3<T>(x, y, z);
	}

	inline Vector3<T> operator-() const {
		return Vector3<T>(-x, -y, -z);
	}

	inline Vector3<T> operator*(Type scalar) const {
		return Vector3<T>(x * scalar, y * scalar, z * scalar);
	}

	inline Vector3<T> operator/(Type scalar) const {
		return Vector3<T>(x / scalar, y / scalar, z / scalar);
	}

	// Comparison Operators
	inline bool operator==(Other other) const {
		return (x == other.x) && (y == other.y) && (z == other.z);
	}

	inline bool operator!=(Other other) const {
		// Simplified using DeMorgan's Law ^:)
		return (x != other.x) || (y != other.y) || (z != other.z);
	}

	// Compound Assignment Operators
	inline Self operator+=(Other other) {
		x += other.x; y += other.y; z += other.z; return *this;
	}

	inline Self operator-=(Other other) {
		x -= other.x; y -= other.y; z -= other.z; return *this;
	}

	inline Self operator*=(Type scalar) {
		x *= scalar; y *= scalar; z *= scalar; return *this;
	}

	inline Self operator/=(Type scalar) {
		x /= scalar; y /= scalar; z /= scalar; return *this;
	}

	// Access Operators
	inline T& operator[](const unsigned long& index) {
		return (&x)[index];
	}

	inline const T& operator[](const unsigned long& index) const {
		return (&x)[index];
	}
};