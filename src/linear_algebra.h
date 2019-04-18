#ifndef LINAL_H
#define LINAL_H
/**
 * @file linear_algebra.h
 * @brief Implements a simple
 * matrix operation framework
 * @date 2019-04-17
 * 
 */

#include "preprocess.h"

/**
 * @brief A square matrix
 * 
 */
struct square_matrix
{
	std::vector<double> mat;
	int n;
	square_matrix(int _n) : n(_n)
	{
		mat.reserve(n * n);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				mat.push_back(0);
	}

	~square_matrix()
	{
		mat.~vector();
	}

	inline double &operator()(int i, int j)
	{
		return mat[j + (n * i)];
	}
};

/**
 * @brief Add operator overload
 * 
 * @return square_matrix 
 */
square_matrix operator+(square_matrix, square_matrix);

/**
 * @brief Subract operator overload
 * 
 * @return square_matrix 
 */
square_matrix operator-(square_matrix, square_matrix);

/**
 * @brief Multiply operator overload
 * 
 * @return square_matrix 
 */
square_matrix operator*(square_matrix, square_matrix);

/**
 * @brief Ostream operator overload
 * 
 * @return ostream 
 */
std::ostream &operator<<(std::ostream &, const square_matrix &);

/**
 * @brief Multiply with constant operator overload
 * 
 * @return square_matrix 
 */
square_matrix operator*(double, square_matrix);

/**
 * @brief Divide with constant operator overload
 * 
 * @return square_matrix 
 */
square_matrix operator/(square_matrix, double);

void make_identity(square_matrix &);

/**
 * @brief A column vector
 * 
 */
struct column_vector
{
	std::vector<double> vec;
	int n;

	column_vector(int _n) : n(_n)
	{
		vec.reserve(n);
		for (int i = 0; i < n; i++)
			vec.push_back(0);
	}

	~column_vector()
	{
		vec.~vector();
	}

	inline double &operator()(int i)
	{
		return vec[i];
	}
};

/**
 * @brief Multiply with constant operator overload
 * 
 * @return square_matrix 
 */
column_vector operator*(square_matrix, column_vector);

/**
 * @brief Ostream operator overload
 * 
 * @return ostream 
 */
std::ostream &operator<<(std::ostream &, const column_vector &);

square_matrix &rotation_xy(double);
square_matrix &rotation_yz(double);
square_matrix &rotation_zx(double);

#endif