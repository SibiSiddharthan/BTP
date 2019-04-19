#ifndef MODEL3D_H
#define MODEL3D_H
/**
 * @file model3d.h
 * @brief This file declares function that are necessary for
 * creating a 3d model
 * @date 2019-04-18
 * 
 */

#include "3d.h"
#include "window.h"
namespace __3d__
{
/**
 * @brief This class defines a 3d model
 */
class model
{
  private:
	///@brief The collection of points that the model contains
	std::vector<node> N;

	///@brief The planes in the model
	std::vector<plane> P;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return vector of all node positions as 3 floats
	 */
	std::vector<float> export_vertex_data() const;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes as unsigned integer
	 */
	std::vector<uint32_t> export_node_index() const;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes of the planes as unsigned integer
	 */
	std::vector<uint32_t> export_plane_index() const;

	/**
	 * @brief This is a helper function used for displaying the model
	 * 
	 * @return index of all nodes of the edges of the planes as unsigned integer
	 */
	std::vector<uint32_t> export_plane_edge_index() const;

	/**
	 * @brief This is a helper function used for displaying the model.
	 * Exports normal coordinates and index data
	 * 
	 * The normals are drawn from the centre of each plane to diatance
	 * of 0.1 units in the direction of the normal
	 */
	std::tuple<std::vector<float>,std::vector<uint32_t>> export_normals() const;

  public:
	/**
	 * @brief Function that displays the 3d object to the screen
	 */
	void display();

	/**
	 * @brief Reads from an stl file to create a 3d model
	 * 
	 * @param filepath 
	 */
	void stl_read(const std::string filepath);

	/**
	 * @brief Returns the number of nodes
	 */
	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	/**
	 * @brief Returns the number of planes
	 */
	inline const uint64_t number_of_planes()
	{
		return P.size();
	}

	/**
	 * @brief Transfers the data from the model class to the mesh class
	 * 
	 * @return std::tuple<std::vector<node>,std::vector<plane>> 
	 */
	inline std::tuple<std::vector<node>,std::vector<plane>> model_export() const
	{
		return std::make_tuple(N, P);
	}
};
} // namespace __3d__

#endif