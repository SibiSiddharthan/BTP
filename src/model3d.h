#ifndef MODEL3D_H
#define MODEL3D_H
#include "3d.h"

namespace __3d__
{

class model
{
  private:
	std::vector<node> N;
	std::vector<plane> P;

  public:
	
	void display();

	void stl_read(const std::string filepath);

	inline const uint64_t number_of_nodes()
	{
		return N.size();
	}

	inline const uint64_t number_of_planes()
	{
		return P.size();
	}

	std::tuple<std::vector<node>,std::vector<plane>> model_export() const;
};
} // namespace __3d__

#endif