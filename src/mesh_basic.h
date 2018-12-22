#pragma once
#include "geometry.h"

enum class node_location
{
	boundary,
	hole,
	inside,
	outside
};
enum class edge_location
{
	boundary,
	inside
};
enum class triangle_type
{
	domain,
	ghost
};

struct node;
struct edge;
struct mesh_triangle;
struct tetrahedron;

struct node
{
	pos p;
	double value = 0;
	uint64_t share = 0;
	uint64_t id = 0;
	node_location location;
	bool availability;
	std::vector<uint64_t> T;
	std::vector<uint64_t> BE;
	node()
	{
		p = {};
		availability = false;
		location = node_location::inside;
		id = 0;
	}

	node(pos _p, uint64_t _id, node_location _location, bool _availability = true) :
		p{_p},
		id{_id},
		location{_location},
		availability{_availability}
	{
	}

	
};

//*For forming a set
inline bool operator<(const node &a, const node &b)
{
	return a.id < b.id;
}

struct edge
{
	uint64_t start, end;
	uint64_t id;
	edge_location location = edge_location::boundary;
	bool availability = true;

	edge()
	{
		start = 0;
		end = 0;
		id = 0;
	}

	edge(uint64_t _start, uint64_t _end, uint64_t _id, edge_location _location , bool _availability = true) :
		start{_start},
		end{_end},
		id{_id},
		location{_location},
		availability{_availability}
	{
	}
};

struct mesh_triangle
{
	uint64_t a, b, c;
	uint64_t id = 0;
	triangle_type type = triangle_type::domain;

	mesh_triangle()
	{
		a = 0;
		b = 0;
		c = 0;
	}

	mesh_triangle(uint64_t _a,uint64_t _b,uint64_t _c, uint64_t _id, triangle_type _type = triangle_type::domain):
		a{_a} ,b{_b}, c{_c}, id{_id},
		type{_type}
	{
	}

};

struct tetrahedron
{
	uint64_t a, b, c, d;
	uint64_t id = 0;
	tetrahedron()
	{
		a = 0;
		b = 0;
		c = 0;
		d = 0;
	}
};

class mesh;
class _2D_;

inline bool operator==(const node &a, const node &b)
{
	return a.id == b.id;
}

inline bool operator!=(const node &a, const node &b)
{
	return a.id != b.id;
}

inline bool operator==(const edge &a, const edge &b)
{
	return a.id == b.id;
}

inline bool operator!=(const edge &a, const edge &b)
{
	return a.id != b.id;
}

inline bool operator==(const mesh_triangle &a, const mesh_triangle &b)
{
	return a.id == b.id;
}

inline bool operator!=(const mesh_triangle &a, const mesh_triangle &b)
{
	return a.id != b.id;
}

inline bool operator==(const tetrahedron &a, const tetrahedron &b)
{
	return a.id == b.id;
}

inline bool operator!=(const tetrahedron &a, const tetrahedron &b)
{
	return a.id != b.id;
}