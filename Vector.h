#pragma once

class Vector3
{
	friend bool operator==(const Vector3& lhs, const Vector3& other);

	friend bool operator<(const Vector3& lhs, const Vector3& other);

public:
	float x, y, z;

	Vector3(float _x, float _y, float _z);
};