#pragma once
#include "preprocess.h"
#include "linear_algebra.h"

struct pos
{
	double x = 0, y = 0, z = 0;

	pos(double _x = 0, double _y = 0, double _z = 0) : x{_x}, y{_y}, z{_z}
	{
	}
};


//operators;
inline pos operator+(const pos p, const pos q)
{
	pos result;
	result.x = q.x + p.x;
	result.y = q.y + p.y;
	result.z = q.z + p.z;
	return result;
}

inline pos operator-(const pos p, const pos q)
{
	pos result;
	result.x = q.x - p.x;
	result.y = q.y - p.y;
	result.z = q.z - p.z;
	return result;
}

inline pos operator*(const pos p, const double x)
{
	pos result;
	result.x = p.x * x;
	result.y = p.y * x;
	result.z = p.z * x;
	return result;
}

inline pos operator/(const pos p, const double x)
{
	pos result;
	result.x = p.x / x;
	result.y = p.y / x;
	result.z = p.z / x;
	return result;
}

/*
! Not Working
inline pos& operator+=(pos &res, const pos p)
{
	res.x += p.x;
	res.y += p.y;
	res.z += p.z;
	return res;
}

inline pos operator-=(pos &res, const pos p)
{
	res.x -= p.x;
	res.y -= p.y;
	res.z -= p.z;
	return res;
}

inline void operator*=(pos &res, const double x)
{
	res.x *= x;
	res.y *= x;
	res.z *= x;
}

inline void operator/=(pos &res, const double x)
{
	res.x /= x;
	res.y /= x;
	res.z /= x;
}

*/
inline bool operator!=(const pos a, const pos b)
{
	return (a.x != b.x || a.y != b.y || a.z != b.z);
}

inline bool operator==(const pos a, const pos b)
{
	return ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) < epsilon); //* square root is not required
}

inline std::ostream &operator<<(std::ostream &out, const pos p)
{
	out << "x: " << p.x << " y: " << p.y << " z: " << p.z << std::endl;
	return out;
}

