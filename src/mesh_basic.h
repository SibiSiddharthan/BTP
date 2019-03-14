#pragma once
#include "geometry.h"

typedef uint64_t node_id, edge_id, triangle_id, plane_id ,tetrahedron_id;
constexpr double area_threshold = 1.0e-3;
constexpr double volume_threshold = 1.0e-3;

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

enum class plane_location
{
	boundary,
	inside
};

enum class tetrahedron_type
{
	domain,
	ghost
};

struct node;
struct edge;
struct mesh_triangle;
struct mesh_plane;
struct tetrahedron;

struct node
{
	pos p;
	double value = 0;
	uint64_t id = 0;
	node_location location;
	bool availability;
	std::vector<triangle_id> T; //shared triangles
	std::vector<edge_id> BE; // shared boundary edges
	std::vector<edge_id> IE; // shared inside edges
	std::vector<plane_id> BP;
	std::vector<plane_id> IP;
	std::vector<tetrahedron_id> TH;
	inline node()
	{
		p = {};
		availability = false;
		location = node_location::inside;
		id = 0;
	}

	inline node(pos _p, uint64_t _id, node_location _location, bool _availability = true) :
		p{_p},
		id{_id},
		location{_location},
		availability{_availability}
	{
	}

	inline const uint64_t triangle_share()
	{
		return T.size();
	}

	inline const uint64_t edge_share()
	{
		return IE.size();
	}

	inline const uint64_t tetrahedron_share()
	{
		return TH.size();
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
	std::vector<triangle_id> T;

	inline edge()
	{
		start = 0;
		end = 0;
		id = 0;
	}

	inline edge(uint64_t _start, uint64_t _end, uint64_t _id, edge_location _location , bool _availability = true) :
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
	uint64_t a, b, c;//node id's
	uint64_t id ;
	triangle_type type = triangle_type::domain;
	std::vector<edge_id> E;//edge id's

	inline mesh_triangle()
	{
		a = 0;
		b = 0;
		c = 0;
		id = 0;
	}

	inline mesh_triangle(uint64_t _a,uint64_t _b,uint64_t _c, uint64_t _id, triangle_type _type = triangle_type::domain):
		a{_a} ,b{_b}, c{_c}, id{_id},
		type{_type}
	{
	}

};

struct mesh_plane
{
	uint64_t a,b,c;
	pos normal;
	uint64_t id;
	plane_location location = plane_location::boundary;
	bool availability = true;

	inline mesh_plane()
	{
		a = 0;
		b = 0;
		c = 0;
		normal = {0,0,0};
		id = 0;
	}

	inline mesh_plane(uint64_t _a,uint64_t _b,uint64_t _c,pos _normal, uint64_t _id, plane_location _location = plane_location::boundary, bool _availability = true):
		a{_a} ,b{_b}, c{_c},normal{_normal} ,id{_id},
		availability{_availability}, location{_location}
	{
	}

	inline mesh_plane(uint64_t _a,uint64_t _b,uint64_t _c, uint64_t _id, plane_location _location = plane_location::boundary, bool _availability = true):
		a{_a} ,b{_b}, c{_c},id{_id},
		availability{_availability}, location{_location}
	{
		normal = cross(c-a,c-b);//!Have to Check
	}

	
};

struct tetrahedron
{
	uint64_t a, b, c, d;
	uint64_t id = 0;
	tetrahedron_type type = tetrahedron_type::domain;
	inline tetrahedron()
	{
		a = 0;
		b = 0;
		c = 0;
		d = 0;
	}

	inline tetrahedron(uint64_t _a,uint64_t _b,uint64_t _c, uint64_t _d,uint64_t _id, tetrahedron_type _type = tetrahedron_type::domain):
		a{_a} ,b{_b}, c{_c},d{_d}, id{_id},
		type{_type}
	{
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