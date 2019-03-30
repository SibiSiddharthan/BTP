#ifndef THREE_DIMESION_HPP
#define THREE_DIMESION_HPP

#include "geometry.h"

namespace __3d__
{
	typedef uint64_t node_id, plane_id, tetrahedron_id;
constexpr double area_threshold = 1.0e-3;
constexpr double volume_threshold = 1.0e-3;

enum class node_location
{
	boundary,
	hole,
	inside,
	outside
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

struct node
{
	pos p;
	double value = 0;
	uint64_t id = 0;
	node_location location;
	bool availability;
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

	inline node(pos _p, uint64_t _id, node_location _location, bool _availability = true) : p{_p},
																							id{_id},
																							location{_location},
																							availability{_availability}
	{
	}

	inline const uint64_t tetrahedron_share()
	{
		return TH.size();
	}
};

struct plane
{
	uint64_t a, b, c;
	pos normal;
	uint64_t id;
	plane_location location = plane_location::boundary;
	bool availability = true;
	std::vector<tetrahedron_id> TH;

	inline plane()
	{
		a = 0;
		b = 0;
		c = 0;
		normal = {0, 0, 0};
		id = 0;
	}

	inline plane(uint64_t _a, uint64_t _b, uint64_t _c, pos _normal, uint64_t _id, plane_location _location = plane_location::boundary, bool _availability = true) : a{_a}, b{_b}, c{_c}, normal{_normal}, id{_id},
																																									 availability{_availability}, location{_location}
	{
	}

	inline plane(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _id, plane_location _location = plane_location::boundary, bool _availability = true) : a{_a}, b{_b}, c{_c}, id{_id},
																																						availability{_availability}, location{_location}
	{
		normal = cross(a - c, b - c); //!Have to Check
	}
};

struct tetrahedron
{
	uint64_t a, b, c, d;
	uint64_t id = 0;
	tetrahedron_type type = tetrahedron_type::domain;
	std::vector<plane_id> P;

	inline tetrahedron()
	{
		a = 0;
		b = 0;
		c = 0;
		d = 0;
	}

	inline tetrahedron(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _id, tetrahedron_type _type = tetrahedron_type::domain) : a{_a}, b{_b}, c{_c}, d{_d}, id{_id},
																																			  type{_type}
	{
	}
};

inline bool operator==(const node &a, const node &b)
{
	return a.id == b.id;
}

inline bool operator!=(const node &a, const node &b)
{
	return a.id != b.id;
}

inline bool operator==(const plane &a, const plane &b)
{
	return a.id == b.id;
}

inline bool operator!=(const plane &a, const plane &b)
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
}

#endif