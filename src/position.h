#pragma once
#include "preprocess.h"


struct pos
{
	double x = 0, y = 0, z = 0;

	pos(double _x = 0, double _y = 0, double _z = 0) : x{_x}, y{_y}, z{_z}
	{
	}

	inline double norm()
	{
		return sqrt(x*x + y*y + z*z);
	}
	
};

namespace std
{
template<>
struct hash<pos>
{
	inline std::size_t operator()(const pos &p) const
	{
		using std::hash;
		return (((hash<double>()(p.x) ^ hash<double>()(p.y) << 1 ) >> 1) ^ (hash<double>()(p.z) << 1));
	}
};
}

//operators;
inline pos operator+(const pos p, const pos q)
{
	pos result;
	result.x = p.x + q.x;
	result.y = p.y + q.y;
	result.z = p.z + q.z;
	return result;
}

inline pos operator-(const pos p, const pos q)
{
	pos result;
	result.x = p.x - q.x;
	result.y = p.y - q.y;
	result.z = p.z - q.z;
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

inline void operator+=(pos &res, const pos p)
{
	res.x += p.x;
	res.y += p.y;
	res.z += p.z;
}

inline void operator-=(pos &res, const pos p)
{
	res.x -= p.x;
	res.y -= p.y;
	res.z -= p.z;
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

