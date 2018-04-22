#pragma once


template <typename T>
struct Vector2 {
private:
    typedef const T& Type;
    typedef const Vector2<T>& Other;
	typedef Vector2<T>& Self;
    
public:
    T x;
    T y;
    
	inline Vector2() : x(0), y(0) {}
    inline Vector2(Type scalar) : x(scalar), y(scalar) {}
    inline Vector2(Type x, Type y) : x(x), y(y) {}
    inline Vector2(Other other) : x(other.x), y(other.y) {}

	// Arithmetic Operators
	inline Self operator=(Other other) {
		x = other.x; y = other.y; return *this;
	}

	inline Vector2<T> operator+(Other other) const {
		return Vector2<T>(x + other.x, y + other.y);
	}

	inline Vector2<T> operator-(Other other) const {
		return Vector2<T>(x - other.x, y - other.y);
	}

	inline Vector2<T> operator+() const {
		return Vector2<T>(x, y);
	}

	inline Vector2<T> operator-() const {
		return Vector2<T>(-x, -y);
	}

	inline Vector2<T> operator*(Type scalar) const {
		return Vector2<T>(x * scalar, y * scalar);
	}

	inline Vector2<T> operator/(Type scalar) const {
		return Vector2<T>(x / scalar, y / scalar);
	}

	// Comparison Operators
	inline bool operator==(Other other) const {
		return (x == other.x) && (y == other.y);
	}

	inline bool operator!=(Other other) {
		// Simplified using DeMorgan's Law ^:)
		return (x != other.x) || (y != other.y);
	}

	// Compound Assignment Operators
	inline Self operator+=(Other other) {
		x += other.x; y += other.y; return *this;
	}

	inline Self operator-=(Other other) {
		x -= other.x; y -= other.y; return *this;
	}

	inline Self operator*=(Type scalar) {
		x *= scalar; y *= scalar; return *this;
	}

	inline Self operator/=(Type scalar) {
		x /= scalar; y /= scalar; return *this;
	}

	// Access Operators
	inline T& operator[](const unsigned long& index) {
		return (&x)[index];
	}

	inline const T& operator[](const unsigned long& index) const {
		return (&x)[index];
	}
};