#include "Vector.h"
#include "Chunk.h"

Vector3::Vector3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

bool operator==(const Vector3& lhs, const Vector3& other)
{
	return lhs.x == other.x && lhs.y == other.y && lhs.z == other.z;
}

bool operator<(const Vector3& lhs, const Vector3& other)
{
	return (lhs.x + lhs.y * chunkSize + lhs.z * chunkHeight * chunkSize) < (other.x + other.y * chunkSize + other.z * chunkHeight * chunkSize);
}