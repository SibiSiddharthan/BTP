#pragma once
#include "position.h"

struct line
{
	pos start, end;

	line(pos _start = {}, pos _end = {}) : start{_start}, end{_end}
	{
	}
};

struct triangle
{
	pos a, b, c;
	triangle(pos _a = {}, pos _b = {}, pos _c = {}) : a{_a}, b{_b}, c{_c}
	{
	}
};

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
