#ifndef GEOMETRY_H
#define GEOMETRY_H

/**
 * @file geometry.h
 * @brief This file defines structs and functions for rudimentry 
 * geometry applications needed by the mesh generators
 * @date 2019-04-15
 * 
 */
#include "position.h"
#include "linear_algebra.h"

/**
 * @struct line
 * @brief Defines a line
 * 
 * @var line::start
 * Start point of the line
 * 
 * @var line::end
 * End point of the line
 * 
 * @fn line::line(pos _start = {}, pos _end = {})
 * @brief The constructor for line
 * @param _start The start point of the line
 * @param _end The end point of the line
 * 
 */
struct line
{
	pos start, end;

	line(pos _start = {}, pos _end = {}) : start{_start}, end{_end}
	{
	}
};

/**
 * @struct triangle
 * @brief Defines a triangle
 * 
 * @var triangle::a 
 * One of the vertices of the triangle
 * 
 * @var triangle::b
 * One of the vertices of the triangle
 * 
 * @var triangle::c 
 * One of the vertices of the triangle
 * 
 * @fn triangle::triangle(pos _a = {}, pos _b = {}, pos _c = {})
 * @brief The constructor for triangle
 * @param _a One of the vertices of the triangle
 * @param _b One of the vertices of the triangle
 * @param _c One of the vertices of the triangle
 * 
 */
struct triangle
{
	pos a, b, c;
	triangle(pos _a = {}, pos _b = {}, pos _c = {}) : a{_a}, b{_b}, c{_c}
	{
	}
};

/**
 * @struct triangle
 * @brief Defines a triangle with a normal
 * 
 * @var triangle::a 
 * One of the vertices of the plane
 * 
 * @var triangle::b
 * One of the vertices of the plane
 * 
 * @var triangle::c 
 * One of the vertices of the plane
 * 
 * @fn plane::plane(pos _a = {}, pos _b = {}, pos _c = {},pos _normal = {0,0,1})
 * @brief The constructor for triangle
 * @param _a One of the vertices of the plane
 * @param _b One of the vertices of the plane
 * @param _c One of the vertices of the plane
 * @param _normal The normal of the plane
 */
struct plane
{
	pos a,b,c,normal;
	plane(pos _a = {},pos _b = {}, pos _c = {}, pos _normal = {0,0,1}): a{_a}, b{_b}, c{_c}, normal{_normal}
	{

	}
};

//useful function
/**
 * @brief Calculates the distance between 2 points
 * 
 * 
 * @param a 
 * @param b 
 * @return distance between the 2 points
 */
inline const double distance(const pos a, const pos b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

/**
 * @brief Calculates the length of a line
 * 
 * @param l 
 * @return distance of the line
 */
inline const double length_of_line(const line l)
{
	return distance(l.start, l.end);
}

/**
 * @brief Calculates the area of a triangle
 * 
 * @param a one of the vertices of the triangle
 * @param b one of the vertices of the triangle
 * @param c one of the vertices of the triangle
 * @return area of the triangle
 */
const double area_of_triangle(const pos a,const pos b,const pos c);

/**
 * @brief Calculates the volume of a tetrahedron
 * 
 * @param a one of the vertices of the tetrahedron
 * @param b one of the vertices of the tetrahedron
 * @param c one of the vertices of the tetrahedron
 * @param d one of the vertices of the tetrahedron
 * @return volume of the tetrahedron
 */
const double volume_of_tetrahedron(const pos a,const pos b,const pos c,const pos d);

/**
 * @brief Rotates a point with respect to the origin by 
 * the given angle counter-clockwise
 * 
 * @param p point to be rorated
 * @param angle angle of rotation in radians
 * @return rotated point
 */
pos rotate_point(pos p, double angle);	

/**
 * @brief Returns the inclination of the line
 * with respect to the x axis
 * 
 * @param l The line
 * 
 * @return angle wrt x-axis in radians
 */
const double line_inclination_absolute(line l); //0 to 2pi

//Crucial Function for checking intersections
/**
 * @brief Finds the side of a point wrt to a line
 * 
 * @param l The reference line
 * @param p the point
 * @return 0 if line and point are collinear\n
 * @return 1 if point is above the line\n
 * @return -1 if point is below the line
 */
const int side_of_point(line l, pos p);

/**
 * @brief Checks whether the 2 lines are the same
 * 
 * @param l1 line 1
 * @param l2 line 2
 * @return true if lines are the same
 * @return false if lines are different
 */
bool same_line(line l1, line l2);


//Finds the point of intersection of two lines if they intersect
//Code not written yet
pos intersection_point(line, line);

//Checks whether a point lies on the left side of the line
/**
 * @brief Checks whether the point is on the left side
 * of the line if the line points from start to end
 * 
 * @param l line
 * @param p point
 * @return true if point lies on the left side of the line
 * @return false if point is collinear or lies on the right side of the line
 */
bool left_test_2d(const line &l, const pos &p);

/**
 * @brief Checks whether the point is on the left side
 * of the plane
 * 
 * @param p plane
 * @param a point
 * @return true if point lies on the left side of the plane
 * @return false if point is coplanar or lies on the right side of the plane
 */
bool left_test_3d(const plane &p,const pos &a);

/**
 * @brief Checks whether a point and a line are collinear
 * 
 * @param l line
 * @param p point
 * @return true if point and line are collinear
 * @return false else
 */
bool is_collinear(const line &l,const pos &p);

/**
 * @brief Checks whether a point and a plane are coplanar
 * 
 * @param p plane
 * @param a point
 * @return true if point and plane are coplanar
 * @return false else
 */
bool is_coplanar(const plane &p,const pos &a);

//Checks whether two lines intersec or not
bool do_they_intersect(line, line);
bool do_they_intersect(plane, plane);

//rotations
pos rotate_xy(pos, double);
pos rotate_yz(pos, double);
pos rotate_zx(pos, double);

/**
 * @brief calculates the dot product of two position vectors
 * 
 * @param a 
 * @param b 
 * @return double 
 */
inline double dot(pos a, pos b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/**
 * @brief calcualtes the cross product of two position vectors
 * 
 * @param a 
 * @param b 
 * @return pos 
 */
inline pos cross(pos a, pos b)
{
	pos res = {};
	res.x = a.y * b.z - a.z * b.y;
	res.y = b.x * a.z - b.z * a.x;
	res.z = a.x * b.y - a.y * b.x;
	return res;
}

//Finds the angle a-b-c
/**
 * @brief Finds the angle between line ab and bc
 * 
 * @param a point of line ab
 * @param b common point of the two lines
 * @param c point of line bc
 * @return double 
 */
double angle_between_lines(pos a, pos b, pos c);

/**
 * @brief Finds the minimum angle of the triangle
 * 
 * @param a one of the vertices of the triangle
 * @param b one of the vertices of the triangle
 * @param c one of the vertices of the triangle
 * @return the minimum angle 
 */
double max_angle_of_triangle(pos a, pos b, pos c);

/**
 * @brief Finds the maximum angle of the triangle
 * 
 * @param a one of the vertices of the triangle
 * @param b one of the vertices of the triangle
 * @param c one of the vertices of the triangle
 * @return the maximum angle
 */
double min_angle_of_triangle(pos a, pos b, pos c);

#endif