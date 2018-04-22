#pragma once
#include <cassert>
#include <memory>
#include "Vector4.hpp"


template <typename T>
struct Matrix4x4 {
private:
	typedef const T& Type;
	typedef const Matrix4x4<T>& Other;
	typedef Matrix4x4<T>& Self;
	typedef Vector4<T> col_T;

	col_T column[4];

public:
	inline Matrix4x4() {
		column[0] = col_T(1, 0, 0, 0);
		column[1] = col_T(0, 1, 0, 0);
		column[2] = col_T(0, 0, 1, 0);
		column[3] = col_T(0, 0, 0, 1);
	}

	inline Matrix4x4(Type scalar) {
		column[0] = col_T(scalar, 0, 0, 0);
		column[1] = col_T(0, scalar, 0, 0);
		column[2] = col_T(0, 0, scalar, 0);
		column[3] = col_T(0, 0, 0, scalar);
	}

	inline Matrix4x4(const col_T& col_1, const col_T& col_2, const col_T& col_3, const col_T& col_4) {
		column[0] = col_1;
		column[1] = col_2;
		column[2] = col_3;
		column[3] = col_4;
	}

	inline Matrix4x4(Other other) {
		memcpy(column, other.column, sizeof(column));
	}

	// Arithmetic Operators
	inline Self operator=(Other other) {
		memcpy(column, other.column, sizeof(column)); return *this;
	}

	inline Matrix4x4<T> operator+(Other other) const {
		return Matrix4x4<T>(
			column[0] + other.column[0],
			column[1] + other.column[1],
			column[2] + other.column[2],
			column[3] + other.column[3]
			);
	}

	inline Matrix4x4<T> operator-(Other other) const {
		return Matrix4x4<T>(
			column[0] - other.column[0],
			column[1] - other.column[1],
			column[2] - other.column[2],
			column[3] - other.column[3]
			);
	}

	inline Matrix4x4<T> operator+() const {
		return Matrix4x4<T>(column[0], column[1], column[2], column[3]);
	}

	inline Matrix4x4<T> operator-() const {
		return Matrix4x4<T>(-column[0], -column[1], -column[2], -column[3]);
	}

	inline Matrix4x4<T> operator*(Type scalar) const {
		return Matrix4x4<T>(column[0] * scalar, column[1] * scalar, column[2] * scalar, column[3] * scalar);
	}

	Matrix4x4<T> operator*(Other other) const {
		const col_T A_col_0 = column[0];
		const col_T A_col_1 = column[1];
		const col_T A_col_2 = column[2];
		const col_T A_col_3 = column[3];

		const col_T B_col_0 = other.column[0];
		const col_T B_col_1 = other.column[1];
		const col_T B_col_2 = other.column[2];
		const col_T B_col_3 = other.column[3];

		return Matrix4x4<T>(
			col_T(A_col_0 * B_col_0[0] + A_col_1 * B_col_0[1] + A_col_2 * B_col_0[2] + A_col_3 * B_col_0[3]),
			col_T(A_col_0 * B_col_1[0] + A_col_1 * B_col_1[1] + A_col_2 * B_col_1[2] + A_col_3 * B_col_1[3]),
			col_T(A_col_0 * B_col_2[0] + A_col_1 * B_col_2[1] + A_col_2 * B_col_2[2] + A_col_3 * B_col_2[3]),
			col_T(A_col_0 * B_col_3[0] + A_col_1 * B_col_3[1] + A_col_2 * B_col_3[2] + A_col_3 * B_col_3[3])
		);
	}

	inline col_T operator*(col_T& vector) const {
		return (column[0] * vector[0]) + (column[1] * vector[1]) + (column[2] * vector[2]) + (column[3] * vector[3]);
	}

	inline Matrix4x4<T> operator/(Type scalar) const {
		return Matrix4x4<T>(column[0] / scalar, column[1] / scalar, column[2] / scalar, column[3] / scalar);
	}

	// Comparison Operators
	inline bool operator==(Other other) const {
		return (column[0] == other.column[0]) && (column[1] == other.column[1]) &&
			   (column[2] == other.column[2]) && (column[3] == other.column[3]);
	}

	inline bool operator!=(Other other) const {
		return (column[0] != other.column[0]) || (column[1] != other.column[1]) ||
			   (column[2] != other.column[2]) || (column[3] != other.column[3]);
	}

	// Compound Assignment Operators
	inline Self operator+=(Other other) {
		column[0] += other.column[0];
		column[1] += other.column[1]; 
		column[2] += other.column[2]; 
		column[3] += other.column[3]; 
		return *this;
	}

	inline Self operator-=(Other other) {
		column[0] -= other.column[0];
		column[1] -= other.column[1];
		column[2] -= other.column[2];
		column[3] -= other.column[3];
		return *this;
	}

	inline Self operator*=(Type scalar) {
        column[0] *= scalar;
        column[1] *= scalar;
        column[2] *= scalar;
        column[3] *= scalar;
		return *this;
	}

	// Component Access Operators
	inline col_T& operator[](const unsigned long& index) {
		assert((index <= 3) && (index >= 0));
		return column[index];
	}

	inline const col_T& operator[](const unsigned long& index) const {
		assert((index <= 3) && (index >= 0));
		return column[index];
	}
};
