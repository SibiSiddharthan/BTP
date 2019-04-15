#ifndef TWO_DIMENSION_HPP
#define TWO_DIMENSION_HPP

/**
 * @file 2d.h
 * @brief This file defines the structs for use for
 * the 2d mesh generation
 * @date 2019-04-14
 */

#include "geometry.h"

namespace __2d__
{

typedef uint64_t node_id, edge_id, triangle_id;

/**
 * @brief The area threshold of how small a triangle should be
 */
constexpr double area_threshold = 1.0e-3;

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
 * location of an edge
 * 
 * boundary = 0 -> on the boundary\n 
 * inside = 1 -> inside the domian\n
 * 
 */

enum class edge_location
{
	boundary,
	inside
};

/**
 * @brief Enum for specifying type of triangle
 * 
 * domain = 0 -> triangle inside the domain\n
 * ghost = 1 -> a ghost triangle
 * 
 */
enum class triangle_type
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
 * @var node::T
 * A vector of triangle id's which have this node as one 
 * of their vertices
 * 
 * @var node::BE
 * A vector of edges that lie on the boundary that
 * have this node as one of their endpoints
 * 
 * @var node::IE
 * A vector of edges that lie inside the domain that
 * have this node as one of their end points
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
 * @fn inline const uint64_t node::triangle_share()
 * @return the number of triangles that have this
 * node as on of their vertices
 * 
 * @fn inline const uint64_t node::edge_share()
 * @return the number of edges inside the domain that have this
 * node as on of their endpoints
 */
struct node
{
	pos p;
	double value = 0;
	uint64_t id = 0;
	node_location location;
	bool availability;
	std::vector<triangle_id> T; //shared triangles
	std::vector<edge_id> BE;	// shared boundary edges
	std::vector<edge_id> IE;	// shared inside edges
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

	inline const uint64_t triangle_share()
	{
		return T.size();
	}

	inline const uint64_t edge_share()
	{
		return IE.size();
	}
};

/**
 * @struct edge
 * @brief A basic structure used to define a line segment
 * in a 2d mesh
 * 
 * The edge is mapped in such a way that
 * the domain is always at the left side of the line segment
 * when viewed from start facing towards the end.
 * 
 * @var edge::start
 * The id of a node corresponding to the start of the edge
 * 
 * @var edge::end
 * The id of a node corresponding to the end of the edge
 * 
 * @var edge::id
 * The id of the edge in an array of edges. This id
 * is used for quick referencing of the edge
 * 
 * @var edge::location
 * The loaction of the edge
 * 
 * @var edge::availability
 * A boolean to help basic mesh generation
 * 
 * @var edge::T
 * A vector of traingle id's which have contain this edge as
 * one of their edges
 * 
 * @fn inline edge::edge()
 * @brief Default constructor for edge
 * 
 * @fn inline edge::edge(uint64_t _start, uint64_t _end, uint64_t _id, edge_location _location, bool _availability = true)
 * @brief Constructor for edge used widely in rest of the code
 * @param _start start of the edge
 * @param _end end of the edge
 * @param _id id of the edge
 * @param _location location of the edge
 * @param _availability availability of the edge used in basic mesh generation
 */
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

	inline edge(uint64_t _start, uint64_t _end, uint64_t _id, edge_location _location, bool _availability = true) : start{_start},
																													end{_end},
																													id{_id},
																													location{_location},
																													availability{_availability}
	{
	}
};


/**
 * @struct triangle
 * @brief A basic structure used to define a triangle
 * in a 2d mesh
 * 
 * The triangle is mapped in such a way that all the vertice are 
 * counter-clockwise
 * 
 * @var triangle::a
 * The id of a node corresponding to one of it's vertices
 * 
 * @var triangle::b
 * The id of a node corresponding to one of it's vertices
 * 
 * @var triangle::c
 * The id of a node corresponding to one of it's vertices
 * 
 * @var triangel::id
 * The id of the triangle in an array of triangles. This id
 * is used for quick referencing of the triangle
 * 
 * @var triangle::type
 * Type of the triangle
 * 
 * @var triangle::E
 * A vector of edge id's of the edges of the triangle
 * 
 * @fn inline triangle::triangle()
 * @brief Default constructor for triangle
 * 
 * @fn inline inline triangle::triangle(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _id, triangle_type _type = triangle_type::domain)
 * @brief Constructor for triangle used widely in rest of the code
 * @param _a One of the vertices of the triangle
 * @param _b One of the vertices of the triangle
 * @param _c One of the vertices of the triangle
 * @param _id id of the triangle
 * @param _type type of the triangle
 */
struct triangle
{
	uint64_t a, b, c; //node id's
	uint64_t id;
	triangle_type type = triangle_type::domain;
	std::vector<edge_id> E; //edge id's

	inline triangle()
	{
		a = 0;
		b = 0;
		c = 0;
		id = 0;
	}

	inline triangle(uint64_t _a, uint64_t _b, uint64_t _c, uint64_t _id, triangle_type _type = triangle_type::domain) : a{_a}, b{_b}, c{_c}, id{_id},
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
 * @brief Equality operator for edge
 * 
 * @return true :if same edge
 * @return false :if different edge
 */
inline bool operator==(const edge &a, const edge &b)
{
	return a.id == b.id;
}

/**
 * @brief Not-equal operator for edge
 * 
 * @return true :if different edge
 * @return false :if same edge
 */
inline bool operator!=(const edge &a, const edge &b)
{
	return a.id != b.id;
}

/**
 * @brief Equality operator for triangle
 * 
 * @return true :if same traingle
 * @return false :if different traingle
 */
inline bool operator==(const triangle &a, const triangle &b)
{
	return a.id == b.id;
}

/**
 * @brief Not-equal operator for triangle
 * 
 * @return true :if different triangle
 * @return false :if same triangle
 */
inline bool operator!=(const triangle &a, const triangle &b)
{
	return a.id != b.id;
}
}
#endif