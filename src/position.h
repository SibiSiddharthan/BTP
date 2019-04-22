
#ifndef POSITION_HPP
#define POSITION_HPP

/**
 * @file position.h
 * @brief Defines the fundamental data type pos that is used
 * throughout the code
 * 
 * @date 2019-04-15
 * 
 */
#include "preprocess.h"
#include <fmt/format.h>
/**
 * @struct pos
 * @brief A point in 3d space
 * 
 * @var x 
 * x - coordinate of point
 * 
 * @var y 
 * y - coordinate of point
 * 
 * @var z
 * z - coordinate of point
 * 
 * @fn pos::pos(double _x = 0, double _y = 0, double _z = 0)
 * @brief The constructor for pos
 * @param _x x-coordinate of point
 * @param _y y-coordinate of point
 * @param _z z-coordinate of point
 * 
 * @fn inline double norm()
 * @brief Returns the distance between the point and the origin
 * This can also be interpreted as the norm of the position vector
 */
struct pos
{
	double x = 0, y = 0, z = 0;

	pos(double _x = 0, double _y = 0, double _z = 0) : x{_x}, y{_y}, z{_z}
	{
	}

	inline double norm()
	{
		return sqrt(x * x + y * y + z * z);
	}
};

namespace std
{
template <>
/**
 * @struct hash<pos>
 * @brief A custom hash function for pos inside
 * namepsace std.
 * 
 */
struct hash<pos>
{
	inline std::size_t operator()(const pos &p) const
	{
		using std::hash;
		return (((hash<double>()(p.x) ^ hash<double>()(p.y) << 1) >> 1) ^ (hash<double>()(p.z) << 1));
	}
};
} // namespace std

//operators;
/**
 * @brief Calculates p + q
 * 
 * @return p + q 
 */
inline pos operator+(const pos p, const pos q)
{
	pos result;
	result.x = p.x + q.x;
	result.y = p.y + q.y;
	result.z = p.z + q.z;
	return result;
}

/**
 * @brief Calcualtes p - q
 * 
 * @param p 
 * @param q 
 * @return p - q
 */
inline pos operator-(const pos p, const pos q)
{
	pos result;
	result.x = p.x - q.x;
	result.y = p.y - q.y;
	result.z = p.z - q.z;
	return result;
}

/**
 * @brief Calculates p * x
 * 
 * @param p 
 * @param x 
 * @return p * x 
 */
inline pos operator*(const pos p, const double x)
{
	pos result;
	result.x = p.x * x;
	result.y = p.y * x;
	result.z = p.z * x;
	return result;
}

/**
 * @brief Calculates p / x
 * 
 * @param p 
 * @param x 
 * @return p / x
 */

inline pos operator/(const pos p, const double x)
{
	pos result;
	result.x = p.x / x;
	result.y = p.y / x;
	result.z = p.z / x;
	return result;
}

/**
 * @brief Defines res = res + p
 * 
 * @param res 
 * @param p 
 */
inline void operator+=(pos &res, const pos p)
{
	res.x += p.x;
	res.y += p.y;
	res.z += p.z;
}

/**
 * @brief Defines res = res - p
 * 
 * @param res 
 * @param p 
 */
inline void operator-=(pos &res, const pos p)
{
	res.x -= p.x;
	res.y -= p.y;
	res.z -= p.z;
}

/**
 * @brief Defines res = res*x
 * 
 * @param res 
 * @param x 
 */
inline void operator*=(pos &res, const double x)
{
	res.x *= x;
	res.y *= x;
	res.z *= x;
}

/**
 * @brief Defines res = res/x
 * 
 * @param res 
 * @param x 
 */
inline void operator/=(pos &res, const double x)
{
	res.x /= x;
	res.y /= x;
	res.z /= x;
}

/**
 * @brief Not equal operator for pos
 * 
 * 
 * @param a 
 * @param b 
 * @return true if a and b are same
 * @return false else
 */
inline bool operator!=(const pos a, const pos b)
{
	return (a.x != b.x || a.y != b.y || a.z != b.z);
}

/**
 * @brief Equality operator for pos
 * 
 * Calculates the distance between a and b, if the distance is less
 * than epsilon a and b are considered to be the same point
 * @param a 
 * @param b 
 * @return true if and b are same
 * @return false else
 */
inline bool operator==(const pos a, const pos b)
{
	return ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z) < epsilon); //* square root is not required
}

/**
 * @brief ostrewam overload for pos
 * 
 * @param out 
 * @param p 
 * @return x:p.x y:p.y z:p.z
 */
inline std::ostream &operator<<(std::ostream &out, const pos p)
{
	out << "x: " << p.x << " y: " << p.y << " z: " << p.z << std::endl;
	return out;
}

namespace fmt
{

template <>
/**
 * @brief fmt formater for pos
 */
struct formatter<pos>
{
	template <typename ParseContext>
	constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const pos &p, FormatContext &ctx)
	{
		return format_to(ctx.out(), "x:{:.3f} y:{:.3f} z:{:.3f}", p.x, p.y,p.z);
	}
};
} // namespace fmt

#endif
