#pragma once
#include"position.h"
#include<algorithm>
#include<vector>


enum class node_location
{
	boundary, hole, inside, outside
};
enum class edge_location
{
	boundary, inside
};
enum class triangle_type
{
	domain, ghost
};

struct node;
struct edge;
struct mesh_triangle;
struct tetrahedron;

struct node
{
	pos p;
	double value = 0;
	bool availability;
	int share = 0;
	node_location location;
	std::vector<mesh_triangle*> T;
	std::vector<edge*> BE;
	node()
	{
		p = {};
		availability = false;
		location = node_location::inside;
	}

	node(pos _p, node_location _location, bool _availability = true) :p{ _p }, location{ _location }, availability{ _availability }
	{

	}

};

struct edge
{
	node *start, *end;
	bool availability = true;
	edge_location location = edge_location::boundary;
	inline double length()
	{
		return distance(start->p,end->p);
	}

	edge()
	{
		start = nullptr;
		end = nullptr;
	}

	edge(node *_start, node *_end, edge_location _location, bool _availability = true) :start{ _start }, end{ _end }, location{ _location }, availability{ _availability }
	{

	}
};

struct mesh_triangle
{
	node *a, *b, *c;
	triangle_type type = triangle_type::domain;
	int id = 0;
	inline double area()
	{
		double l1, l2, l3, s;
		l1 = distance(a->p, b->p);
		l2 = distance(a->p, c->p);
		l3 = distance(b->p, c->p);
		s = (l1 + l2 + l3)*0.5;
		return sqrt(s*(s - l1)*(s - l2)*(s - l3));
	}

	inline double min_angle()
	{
		double angle_a, angle_b, angle_c;
		angle_a = angle_between_lines(b->p, a->p, c->p);
		angle_b = angle_between_lines(a->p, b->p, c->p);
		angle_c = angle_between_lines(a->p, c->p, b->p);

		return std::min(std::min(angle_a, angle_b), std::min(angle_b, angle_c));
	}
	

	mesh_triangle()
	{
		a = nullptr;
		b = nullptr;
		c = nullptr;
	}

	void node_change(node *from, node *to)
	{
		for (int i = 0; i < from->share; i++)
		{
			if (this == from->T[i])
			{
				from->T.erase(from->T.begin() + i);
				from->share--;

				break;
			}
		}

		if (this->a == from)
		{
			this->a = to;
			to->T.push_back(this);
			to->share++;
		}

		else if (this->b == from)
		{
			this->b = to;
			to->T.push_back(this);
			to->share++;
		}

		else if (this->c == from)
		{
			this->c = to;
			to->T.push_back(this);
			to->share++;
		}
	}
	
};

struct tetrahedron
{
	node *a, *b, *c, *d;

	tetrahedron()
	{
		a = nullptr;
		b = nullptr;
		c = nullptr;
		d = nullptr;
	}
};



class mesh;
class _2D_;