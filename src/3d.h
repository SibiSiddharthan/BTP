#ifndef THREE_DIMESION_HPP
#define THREE_DIMESION_HPP

/**
 * @file 3d.h
 * @brief This file defines the structs for use for
 * the 3d mesh generation
 * @date 2019-04-15
 * 
 * 
 */
#include "geometry.h"

namespace __3d__
{

typedef uint64_t node_id, plane_id, tetrahedron_id;
constexpr double area_threshold = 1.0e-3;

/**
 * @brief The volume threshold of how small a tetrahedron should be
 */
constexpr double volume_threshold = 1.0e-3;

/**
 * @brief Enum for specifying 
 * location of a node
 * 
 * boundary = 0 -> on the outside boundary \n
 * hole = 1 -> on the hole inside a boundary\n
 * inside = 2 -> inside the domain\n
 * outside = 3 -> outside the domain\n
 * 
 */
enum class node_location
{
	boundary,
	hole,
	inside,
	outside
};

/**
 * @brief Enum for specifying
 * location of a plane
 * 
 * boundary = 0 -> on the boundary\n 
 * inside = 1 -> inside the domian\n
 * 
 */
enum class plane_location
{
	boundary,
	inside
};

/**
 * @brief Enum for specifying type of tetrahedron
 * 
 * domain = 0 -> tetrahedron inside the domain\n
 * ghost = 1 -> a ghost tetrahedron
 * 
 */
enum class tetrahedron_type
{
	domain,
	ghost
};


/**
 * @struct node
 * @brief A basic structure used to define a
 * point in 2d space with some extra functionality
 * 
 * @var node::p
 * Position of the point in 2d space
 * 
 * @var node::id
 * The id of the node in an array of nodes. This id
 * is used for quick referencing of the node
 * 
 * @var node::location
 * The location of the node
 * @see node_location
 * 
 * @var node::availablility
 * A boolean to help basic mesh generation
 * 
 * @var node::TH
 * A vector of tetrahedron id's which have this node as one 
 * of their vertices
 * 
 * @var node::BP
 * A vector of planes that lie on the boundary that
 * have this node as one of their vertices
 * 
 * @var node::IP
 * A vector of planes that lie inside the domain that
 * have this node as one of their vertices
 * 
 * @fn inline node::node()
 * @brief Default Constructor for node
 * 
 * @fn inline node::node(pos _p, uint64_t _id, node_location _location, bool _availability = true)
 * @brief Constructor for node used widely in rest of the code
 * @param _p postion of the node
 * @param id The index of the node in the array in which it is stored
 * @param _location location of the node
 * @param _availability If the node is availble during basic mesh generation
 * 
 * @fn inline const uint64_t node::tetrahderon_share()
 * @return the number of tetrahedrons that have this
 * node as on of their vertices
 *  
 */
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


/**
 * @struct plane
 * @brief A basic structure used to define a triangle with a normal
 * in a 3d mesh
 * 
 * The plane is mapped in such a way that
 * the domain is always in the opposite direction of it's normal
 * 
 * @var plane::a
 * The id of a node corresponding to one of it's vertices
 * 
 * @var plane::b
 * The id of a node corresponding to one of it's vertices
 * 
 * @var plane::c
 * The id of a node corresponding to one of it's vertices
 * 
 * @var plane::id
 * The id of the plane in an array of planes. This id
 * is used for quick referencing of the plane
 * 
 * @var plane::location
 * The loaction of the plane
 * 
 * @var plane::availability
 * A boolean to help basic mesh generation
 * 
 * @var plane::TH
 * A vector of tetrahedron id's which have contain this plane as
 * one of their faces
 * 
 * @fn inline plane::plane()
 * @brief Default constructor for plane
 * 
 * @fn inline plane::plane(uint64_t _a, uint64_t _b, uint64_t _c, pos _normal, uint64_t _id, plane_location _location = plane_location::boundary, bool _availability = true)
 * @brief Constructor for plane used widely in rest of the code
 * @param _a One of the vertices of the plane
 * @param _b One of the vertices of the plane
 * @param _c One of the vertices of the plane
 * @param _normal Normal of the plane
 * @param _id id of the plane
 * @param _location location of the plane
 * @param _availability availability of the edge used in basic mesh generation
 * 
 * @fn inline plane::plane(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _id, plane_location _location = plane_location::boundary, bool _availability = true)
 * @brief Constructor for plane used widely in rest of the code
 * @param _a One of the vertices of the plane
 * @param _b One of the vertices of the plane
 * @param _c One of the vertices of the plane
 * @param _id id of the plane
 * @param _location location of the plane
 * @param _availability availability of the edge used in basic mesh generation
 */
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


/**
 * @struct tetrahedron	
 * @brief A basic structure used to define a tetrahedron
 * in a 3d mesh
 * 
 * @var tetrahedron::a
 * The id of a node corresponding to one of it's vertices
 * 
 * @var tetrahedron::b
 * The id of a node corresponding to one of it's vertices
 * 
 * @var tetrahedron::c
 * The id of a node corresponding to one of it's vertices
 * 
 * @var tetrahedron::d
 * The id of a node corresponding to one of it's vertices
 * 
 * @var tetrahedron::id
 * The id of a tetrahedron in an array of tetrahedrons.
 * This id is used for quick referencing of the tetrahedron
 * 
 * @var tetrahedron::type
 * Tpye of the tetrahedron
 * 
 * @fn inline tetrahedron::tetrahedron()
 * @brief Default constructor for tetrahedron
 * 
 * @fn inline tetrahedron::tetrahedron(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _d, uint64_t _id, tetrahedron_type _type = tetrahedron_type::domain)
 * @brief Constructor for triangle used widely in rest of the code
 * @param _a One of the vertices of the tetrahedron
 * @param _b One of the vertices of the tetrahedron
 * @param _c One of the vertices of the tetrahedron
 * @param _d One of the vertices of the tetrahedron
 * @param _id id of the tetrahedron
 * @param _type type of the tetrahedron
 */
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

/**
 * @brief Equality operator for node
 * 
 * @return true :if same node
 * @return false :if different node
 */
inline bool operator==(const node &a, const node &b)
{
	return a.id == b.id;
}

/**
 * @brief Not-equal operator for node
 * 
 * @return true :if different node
 * @return false :if same node
 */
inline bool operator!=(const node &a, const node &b)
{
	return a.id != b.id;
}

/**
 * @brief Equality operator for plane
 * 
 * @return true :if same plane
 * @return false :if different plane
 */
inline bool operator==(const plane &a, const plane &b)
{
	return a.id == b.id;
}

/**
 * @brief Not-equal operator for plane
 * 
 * @return true :if different plane
 * @return false :if same plane
 */
inline bool operator!=(const plane &a, const plane &b)
{
	return a.id != b.id;
}

/**
 * @brief Equality operator for tetrahedron
 * 
 * @return true :if same tetrahedron
 * @return false :if different tetrahedron
 */
inline bool operator==(const tetrahedron &a, const tetrahedron &b)
{
	return a.id == b.id;
}

/**
 * @brief Not-equal operator for tetrahedron
 * 
 * @return true :if different tetrahedron
 * @return false :if same tetrahedron
 */
inline bool operator!=(const tetrahedron &a, const tetrahedron &b)
{
	return a.id != b.id;
}
}

#endif