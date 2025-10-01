#pragma once

namespace KamataEngine {

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;
};

inline Vector3 operator*(const Vector3& v, float scalar) {
    return Vector3{ v.x * scalar, v.y * scalar, v.z * scalar };
}

inline Vector3 operator*(float scalar, const Vector3& v) {
    return Vector3{ v.x * scalar, v.y * scalar, v.z * scalar };
}

inline Vector3& operator+=(Vector3& lhs, const Vector3& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
    return Vector3{ lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
}

} // namespace KamataEngine
