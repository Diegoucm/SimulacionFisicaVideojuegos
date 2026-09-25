#pragma once

#include <cmath>
#include "PxPhysicsAPI.h"

class Vector3D
{
	public:
	float x;
	float y;
	float z;

	Vector3D() : x(0), y(0), z(0) {}
	Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}

	float magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3D normalize() const {
		float mag = magnitude();
		if (mag == 0) return Vector3D(0, 0, 0);
		return Vector3D(x / mag, y / mag, z / mag);
	}

	float dot(const Vector3D& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	Vector3D cross(const Vector3D& v) const {
		return Vector3D(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x
		);
	}

	Vector3D operator+(const Vector3D& other) const {
		return Vector3D(x + other.x, y + other.y, z + other.z);
	}

	Vector3D operator-(const Vector3D& other) const {
		return Vector3D(x - other.x, y - other.y, z - other.z);
	}

	Vector3D operator*(float scalar) const {
		return Vector3D(x * scalar, y * scalar, z * scalar);
	}

	Vector3D operator/(float scalar) const {
		return Vector3D(x / scalar, y / scalar, z / scalar);
	}

	Vector3D& operator+=(const Vector3D& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3D& operator =(const Vector3D& other) {
		if (this != &other) {
			x = other.x;
			y = other.y;
			z = other.z;
		}
		return *this;
	}

	//Wrapper de conversion a PhysX Vector3
	physx::PxVec3 toPxVec3() const {
		return physx::PxVec3(x, y, z);
	}

	Vector3D toVec3D(const physx::PxVec3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}
};

