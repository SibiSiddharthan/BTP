#include "geometry.h"

//useful funtions

const double area_of_triangle(const pos a, const pos b, const pos c)
{
	double l1, l2, l3, s;
	l1 = distance(a, b);
	l2 = distance(a, c);
	l3 = distance(b, c);
	s = (l1 + l2 + l3) * 0.5;
	return sqrt(s * (s - l1) * (s - l2) * (s - l3));
}

const double volume_of_tetrahedron(const pos a, const pos b, const pos c, const pos d)
{
	return fabs((dot(cross(a - d, b - d), c - d))) / 6;
}

pos rotate_point(pos p, double angle)
{
	pos temp = p;
	p.x = temp.x * cos(angle) - temp.y * sin(angle);
	p.y = temp.x * sin(angle) + temp.y * cos(angle);
	return p;
}

const double line_inclination_absolute(line l)
{
	pos temp = l.end - l.start;
	if (fabs(l.end.x - l.start.x) > epsilon)
	{
		double m = (l.end.y - l.start.y) / ((l.end.x - l.start.x));
		double angle = atan(m);

		if (angle >= 0)
		{
			if (temp.x >= 0) //1st quadrant
				return angle;
			else // 3rd quadrant
				return pi + angle;
		}
		else
		{
			angle = fabs(angle);
			if (temp.x >= 0) //4th quadrant
				return 2 * pi - angle;

			else //2nd quadrant
				return pi - angle;
		}
	}

	else
	{
		if (temp.y > 0)
			return pi * 0.5;

		else if (temp.y < 0)
			return 1.5 * pi;
	}

	return 0;
}

//Returns 1 on one side -1 on other side 0 if coincident
const int side_of_point(line l, pos p)
{
	double a = (p.y - l.start.y) * (l.end.x - l.start.x) - (p.x - l.start.x) * (l.end.y - l.start.y);

	if (a > 0)
		return 1;

	else if (a < 0)
		return -1;

	else
		return 0;
}

bool same_line(line l1, line l2)
{
	if ((l1.start == l2.start && l1.end == l2.end) || (l1.start == l2.end && l1.end == l2.start))
		return true;
	else
		return false;
}

bool do_they_intersect(line a, line b)
{
	if ((a.start != b.start && a.start != b.end && a.end != b.start && a.end != b.end))
	{
		if (!(side_of_point(a, b.start) == 0 && side_of_point(a, b.end) == 0 && //concurrent line segments
			  side_of_point(b, a.start) == 0 && side_of_point(b, a.end) == 0))
		{
			if ((((side_of_point(a, b.start) + side_of_point(a, b.end)) == 0) && ((side_of_point(b, a.start) + side_of_point(b, a.end)) == 0)) || //
				(((side_of_point(a, b.start) + side_of_point(a, b.end)) == 0) && (side_of_point(b, a.start) * side_of_point(b, a.end) == 0)) ||   //
				(((side_of_point(b, a.start) + side_of_point(b, a.end)) == 0) && (side_of_point(a, b.start) * side_of_point(a, b.end)) == 0))
				return true;

			else
				return false;
		}

		else
			return false;
	}

	else
		return false;
}

bool do_they_intersect(plane p1, plane p2)
{
	int a = int(left_test_3d(p1, p2.a)) + int(left_test_3d(p1, p2.b)) + int(left_test_3d(p1, p2.c));
	int b = int(left_test_3d(p2, p1.a)) + int(left_test_3d(p2, p1.b)) + int(left_test_3d(p2, p1.c));
	//printf("%d %d\n",a,b);
	if ((a < 3 && a > 0) && (b > 0 && b < 3))
		return true;
	else
		return false;

	/*
	//p1 is base, p2 is tested
	pos point_of_intersection;
	double dot_line_plane_normal;
	pos line_vector;
	pos cross_a, cross_b, cross_c, cross_d, cross_e,cross_f,cross_g;

	//segment 1 a-b
	line_vector = p2.b - p2.a;
	dot_line_plane_normal = dot(line_vector, p1.normal);
	if (dot_line_plane_normal != 0)
	{
		point_of_intersection = p2.a + line_vector * ((dot(p1.a, p1.normal) - dot(p2.a, p1.normal)) / dot_line_plane_normal);
		cross_a = cross(p1.b - p1.a, point_of_intersection - p1.a);
		cross_b = cross(p1.c - p1.b, point_of_intersection - p1.b);
		cross_c = cross(p1.a - p1.c, point_of_intersection - p1.c);
		if (dot(cross_a, cross_b) > 0 && dot(cross_a, cross_c) > 0)
		{
			return true;
		}
	}

	else
	{
		//base segement 1 a-b
		cross_d = cross(p2.b - p2.a, p1.a - p2.a);
		cross_e = cross(p2.b - p2.a, p1.b - p2.a);
		cross_f = cross(p1.b - p1.a, p2.a - p1.a);
		cross_g = cross(p1.b - p1.a, p2.b - p1.a);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}

		//base segement 2 b-c
		cross_d = cross(p2.b - p2.a, p1.b - p2.a);
		cross_e = cross(p2.b - p2.a, p1.c - p2.a);
		cross_f = cross(p1.c - p1.b, p2.a - p1.b);
		cross_g = cross(p1.c - p1.b, p2.b - p1.b);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}

		//base segement 3 c-a
		cross_d = cross(p2.b - p2.a, p1.c - p2.a);
		cross_e = cross(p2.b - p2.a, p1.a - p2.a);
		cross_f = cross(p1.a - p1.c, p2.a - p1.c);
		cross_g = cross(p1.a - p1.c, p2.b - p1.c);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}
	}

	//segment 2 b-c
	line_vector = p2.c - p2.b;
	dot_line_plane_normal = dot(line_vector, p1.normal);
	if (dot_line_plane_normal != 0)
	{
		point_of_intersection = p2.b + line_vector * ((dot(p1.a, p1.normal) - dot(p2.b, p1.normal)) / dot_line_plane_normal);
		cross_a = cross(p1.b - p1.a, point_of_intersection - p1.a);
		cross_b = cross(p1.c - p1.b, point_of_intersection - p1.b);
		cross_c = cross(p1.a - p1.c, point_of_intersection - p1.c);
		if (dot(cross_a, cross_b) > 0 && dot(cross_a, cross_c) > 0)
		{
			return true;
		}
	}

	else
	{
		//base segement 1 a-b
		cross_d = cross(p2.c - p2.b, p1.a - p2.b);
		cross_e = cross(p2.c - p2.b, p1.b - p2.b);
		cross_f = cross(p1.b - p1.a, p2.b - p1.a);
		cross_g = cross(p1.b - p1.a, p2.c - p1.a);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}

		//base segement 2 b-c
		cross_d = cross(p2.c - p2.b, p1.b - p2.b);
		cross_e = cross(p2.c - p2.b, p1.c - p2.b);
		cross_f = cross(p1.c - p1.b, p2.b - p1.b);
		cross_g = cross(p1.c - p1.b, p2.c - p1.b);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}

		//base segement 3 c-a
		cross_d = cross(p2.c - p2.b, p1.c - p2.b);
		cross_e = cross(p2.c - p2.b, p1.a - p2.b);
		cross_f = cross(p1.a - p1.c, p2.b - p1.c);
		cross_g = cross(p1.a - p1.c, p2.c - p1.c);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}
	}

	//segment 3 c-a
	line_vector = p2.c - p2.a;
	dot_line_plane_normal = dot(line_vector, p1.normal);
	if (dot_line_plane_normal != 0)
	{
		point_of_intersection = p2.c + line_vector * ((dot(p1.a, p1.normal) - dot(p2.c, p1.normal)) / dot_line_plane_normal);
		cross_a = cross(p1.b - p1.a, point_of_intersection - p1.a);
		cross_b = cross(p1.c - p1.b, point_of_intersection - p1.b);
		cross_c = cross(p1.a - p1.c, point_of_intersection - p1.c);
		if (dot(cross_a, cross_b) > 0 && dot(cross_a, cross_c) > 0)
		{
			return true;
		}
	}

	else
	{
		//base segement 1 a-b
		cross_d = cross(p2.a - p2.c, p1.a - p2.c);
		cross_e = cross(p2.a - p2.c, p1.b - p2.c);
		cross_f = cross(p1.b - p1.a, p2.c - p1.a);
		cross_g = cross(p1.b - p1.a, p2.a - p1.a);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}

		//base segement 2 b-c
		cross_d = cross(p2.a - p2.c, p1.b - p2.c);
		cross_e = cross(p2.a - p2.c, p1.c - p2.c);
		cross_f = cross(p1.c - p1.b, p2.c - p1.b);
		cross_g = cross(p1.c - p1.b, p2.a - p1.b);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}

		//base segement 3 c-a
		cross_d = cross(p2.a - p2.c, p1.c - p2.c);
		cross_e = cross(p2.a - p2.c, p1.a - p2.c);
		cross_f = cross(p1.a - p1.c, p2.c - p1.c);
		cross_g = cross(p1.a - p1.c, p2.a - p1.c);
		if (dot(cross_d, p1.normal) * dot(cross_e, p1.normal) < 0 &&
			dot(cross_f, p1.normal) * dot(cross_g, p1.normal) < 0)
		{
			return true;
		}
	}

	return false;
	*/
}

// TODO
pos intersection_point(line a, line b)
{
	pos result = {};
	return result;
}

/*
Shifts origin to start of line segment and aligns the y axis along it
and checks whether the point is on the left or right side of the line
* If the point is collinear the left test passes
*/

bool left_test_2d(const line &l, const pos &p)
{
	pos p1 = l.end - l.start, p2 = p - l.start;
	if (p1.x * p2.y - p2.x * p1.y > 0)
		return true;
	else
		return false;
}

bool left_test_3d(const plane &p, const pos &a)
{
	pos centroid = (p.a + p.b + p.c) / 3;
	pos vec = a - centroid;
	pos normal = centroid + p.normal * 0.5;

	if (vec.x * normal.x + vec.y * normal.y + vec.z * normal.z < 0) //Normals are pointed outwards
		return true;
	else
		return false;
}

//If the absolute value of the square of the area of the triangle is less than epsilon then the function returns true
bool is_collinear(const line &l, const pos &p)
{
	double sq_area;
	double l1, l2, l3, s;
	l1 = distance(l.start, p);
	l2 = distance(l.end, p);
	l3 = distance(l.start, l.end);
	s = (l1 + l2 + l3) * 0.5;
	sq_area = (s * (s - l1) * (s - l2) * (s - l3));

	if (fabs(sq_area) < epsilon)
		return true;
	else
		return false;
}

bool is_coplanar(const plane &p, const pos &x)
{
	double volume = fabs((dot(cross(p.a - x, p.b - x), p.c - x))) / 6;
	if (volume < epsilon)
		return true;
	else
		return false;
}

//rotations
pos rotate_xy(pos p, double angle)
{
	column_vector v(3);
	v(0) = p.x;
	v(1) = p.y;
	v(2) = p.z;
	v = rotation_xy(angle) * v;
	p.x = v(0);
	p.y = v(1);
	p.z = v(2);
	return p;
}

pos rotate_yz(pos p, double angle)
{
	column_vector v(3);
	v(0) = p.x;
	v(1) = p.y;
	v(2) = p.z;
	v = rotation_yz(angle) * v;
	p.x = v(0);
	p.y = v(1);
	p.z = v(2);
	return p;
}

pos rotate_zx(pos p, double angle)
{
	column_vector v(3);
	v(0) = p.x;
	v(1) = p.y;
	v(2) = p.z;
	v = rotation_zx(angle) * v;
	p.x = v(0);
	p.y = v(1);
	p.z = v(2);
	return p;
}

/*Shifts origin to b
Finds the inclination of ba,bc
Rotates the points by the minimum angle clockwise and finds the included angle*/
double angle_between_lines(pos a, pos b, pos c)
{
	a = a - b; //shifting origin to b
	c = c - b;
	b = b - b;
	double angle_a = line_inclination_absolute({b, a});
	double angle_c = line_inclination_absolute({b, c});

	if (std::min(angle_a, angle_c) == angle_a)
	{
		c = rotate_point(c, (2 * pi) - angle_a);
		return line_inclination_absolute({b, c});
	}
	else
	{
		a = rotate_point(a, (2 * pi) - angle_c);
		return line_inclination_absolute({b, a});
	}
}

double max_angle_of_triangle(pos a, pos b, pos c)
{
	double angle_a, angle_b, angle_c;
	angle_a = angle_between_lines(b, a, c);
	angle_b = angle_between_lines(a, b, c);
	angle_c = angle_between_lines(a, c, b);

	return std::max(std::max(angle_a, angle_b), std::max(angle_b, angle_c));
}

double min_angle_of_triangle(pos a, pos b, pos c)
{
	double angle_a, angle_b, angle_c;
	angle_a = angle_between_lines(b, a, c);
	angle_b = angle_between_lines(a, b, c);
	angle_c = angle_between_lines(a, c, b);

	return std::min(std::min(angle_a, angle_b), std::min(angle_b, angle_c));
}
