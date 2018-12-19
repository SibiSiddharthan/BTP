#pragma once
#include<iostream>
#include<vector>

struct square_matrix
{
	std::vector<double> mat;
	int n;
	square_matrix(int _n) :n(_n)
	{
		mat.reserve(n*n);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				mat.push_back(0);
	}

	~square_matrix()
	{
		mat.~vector();
		
	}
		
	
	inline double& operator()(int i, int j)
	{
		return mat[j+(n*i)];
	}
	

};

square_matrix operator+(square_matrix, square_matrix);
square_matrix operator-(square_matrix, square_matrix);
square_matrix operator*(square_matrix, square_matrix);

std::ostream& operator<<(std::ostream&, const square_matrix&);
square_matrix operator*(double,square_matrix);
square_matrix operator/(square_matrix, double);

void make_identity(square_matrix&);


struct column_vector
{
	std::vector<double> vec;
	int n;

	column_vector(int _n) :n(_n)
	{
		vec.reserve(n);
		for (int i = 0; i < n; i++)
			vec.push_back(0);
	}

	~column_vector()
	{
		vec.~vector();
	}

	inline double& operator()(int i)
	{
		return vec[i];
	}


};

column_vector operator*(square_matrix, column_vector);

std::ostream& operator<<(std::ostream&, const column_vector&);

square_matrix& rotation_xy(double);
square_matrix& rotation_yz(double);
square_matrix& rotation_zx(double);