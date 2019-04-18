#ifndef MODEL2D_H
#define MODEL2D_H
/**
 * @file model2d.h
 * @brief Defines functions to create a 2D model from scratch
 * an to import it from a dxf file
 * @date 2019-04-18
 * 
 */

#include "2d.h"
#include "window.h"

namespace __2d__
{

/**
 * @brief Creates a model
 * 
 */
class model
{
  private:
	///@brief The collection of points that the model contains
	std::vector<node> N;
	/**
	 * @brief The edges in the model
	 * 
	 * The edges are oriented counter clock-wise for boundaries
	 * and clockwise for holes
	 * 
	 */
	std::vector<edge> E;
	void intersections(std::vector<line> &, std::vector<std::pair<line, line>> &);
	void branches_and_gaps(std::vector<line> &, std::vector<pos> &);
	bool intersection_pass, branch_pass;

	/**
	 * @brief Returns the number of nodes
	 */
	inline const uint64_t number_of_nodes() const
	{
		return N.size();
	}
	/**
	 * @brief Returns the number of edges
	 */
	inline const uint64_t number_of_edges() const
	{
		return E.size();
	}

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
	 * @return index of all nodes of the edges as unsigned integer
	 */
	std::vector<uint32_t> export_edge_index() const;

  public:
	bool transferable = true;

	/**
	 * @brief Reads a dxf file and converts it to a more usable form
	 * 
	 * @param filepath 
	 * @param dx resolution or a measure of the distance between lines
	 */
	void dxf_read(const std::string filepath, double dx = 1.0);
		
	/**
	 * @brief Saves the model to a file
	 * 
	 * @param filepath 
	 */
	void save(const std::string &filepath);

	/**
	 * @brief Loads the model from a file
	 * 
	 * @param filepath 
	 */
	void load(const std::string &filepath);

	/*Simple boundary constructs for testing purposes*/
	/**
	 * @brief Add a circular boundary at the origin
	 * 
	 * @param radius 
	 * @param dx 
	 */
	void add_boundary_circle(double radius, double dx);

	/**
	 * @brief add a square boundary at the origin
	 * 
	 * @param length 
	 * @param dx 
	 */
	void add_boundary_square(double length, double dx);

	/**
	 * @brief add a circular hole
	 * 
	 * @param centre 
	 * @param radius 
	 * @param dx 
	 */
	void add_hole_circle(pos centre, double radius, double dx);

	/**
	 * @brief add a square hole
	 * 
	 * @param centre 
	 * @param length 
	 * @param dx 
	 */
	void add_hole_square(pos centre, double length, double dx);

	/**
	 * @brief Displays the 2d figure
	 */
	void display();

	//Displays Errors if any when reading from a dxf file
	//void display_import_abnormalities();

	/**
	 * @brief Transfers the data from the model class to the mesh class
	 * 
	 * @return std::tuple<std::vector<node>,std::vector<edge>> 
	 */
	inline std::tuple<std::vector<node>,std::vector<edge>> model_export() const
	{
		return std::make_tuple(N, E);
	}

	/**
	 * @brief Returns some stats about the model
	 * like number of nodes and number of edges
	 * 
	 * @return std::string 
	 */
	std::string stats() const;

};
}

#endif