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

struct line
{
	pos start, end;

	line(pos s = {0, 0, 0}, pos e = {0, 0, 0})
	{
		start = s;
		end = e;
	}
};

struct triangle
{
	pos a, b, c;
	triangle(pos p = {}, pos q = {}, pos r = {})
	{
		a = p;
		b = q;
		c = r;
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

//useful function
inline double distance(const pos a, const pos b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

inline double length_of_line(line l)
{
	return distance(l.start, l.end);
}

double area_of_triangle(pos a, pos b, pos c);
pos rotate_point(pos, double);			//counter-clockwise
double line_inclination_absolute(line); //0 to 2pi

//Crucial Function for checking intersections
int side_of_point(line, pos);
bool same_line(line, line);
bool unique_pos(pos, pos *, const int);
bool unique_pos(pos, std::vector<pos> &);

//Finds the point of intersection of two lines if they intersect
//Code not written yet
pos intersection_point(line, line);

//Checks whether a point lies on the left side of the line
bool left_test_2d(line, pos);

//Checks whether a point and a line are collinear
bool collinear_test(line, pos);

//Checks whether two lines intersec or not
bool do_they_intersect(line, line);

//rotations
pos rotate_xy(pos, double);
pos rotate_yz(pos, double);
pos rotate_zx(pos, double);

inline double dot(pos a, pos b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline pos cross(pos a, pos b)
{
	pos res = {};
	res.x = a.y * b.z - a.z * b.y;
	res.y = b.x * a.z - b.z * a.x;
	res.z = a.x * b.y - a.y * b.x;
	return res;
}

//Finds the angle a-b-c
double angle_between_lines(pos a, pos b, pos c);

double max_angle_of_triangle(pos a, pos b, pos c);
double min_angle_of_triangle(pos a, pos b, pos c);