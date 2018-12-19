#include"linear_algebra.h"
#include<iostream>
#include<cmath>

using namespace std;


square_matrix operator+(square_matrix S1, square_matrix S2)
{
	
	if (S1.n == S2.n)
	{
		for (int i = 0; i < S1.n; i++)
			for (int j = 0; j < S1.n; j++)
				S1(i,j) = S1(i,j) + S2(i, j);
	}

	return S1;
}


square_matrix operator-(square_matrix S1, square_matrix S2)
{
	if (S1.n == S2.n)
	{
		for (int i = 0; i < S1.n; i++)
			for (int j = 0; j < S1.n; j++)
				S1(i, j) = S1(i, j) - S2(i, j);
	}

	return S1;

}

square_matrix operator*(square_matrix S1, square_matrix S2)
{
	square_matrix result = square_matrix(S1.n);
	if (S1.n == S2.n)
	{
		for (int i = 0; i < S1.n; i++)
			for (int j = 0; j < S1.n; j++)
			{
				result(i,j) = 0;
				for (int k = 0; k < S1.n; k++)
				{
					result(i, j) += S1(i,k) * S2(k,j);
				}
			}
	}
	S1 = result;
	return S1;

}


square_matrix operator*(double a,square_matrix S)
{
		
	for (int i = 0; i < S.n; i++)
		for (int j = 0; j < S.n; j++)
			S(i,j) = S(i, j) * a;
	
	return S;

}



square_matrix operator/(square_matrix S, double a)
{
	for (int i = 0; i < S.n; i++)
		for (int j = 0; j < S.n; j++)
			S(i, j) = S(i, j) / a;

	return S;

}



ostream& operator<<(ostream &out, const square_matrix &S)
{
	for (int i = 0; i < S.n; i++)
	{
		for (int j = 0; j < S.n; j++)
			out << S.mat[j + (i*S.n)] << " ";
		out << endl;
	}
	return out;
}

void make_identity(square_matrix &S)
{
	for (int i = 0; i < S.n; i++)
		for (int j = 0; j < S.n; j++)
			if (i == j)
				S(i, j) = 1;
}



column_vector operator*(square_matrix S, column_vector x)
{
	column_vector result = column_vector(x.n);
	if (x.n == S.n)
	{
		for (int i = 0; i < x.n;i++)
			for (int j = 0; j < x.n; j++)
			{
				result(i) += S(i, j)*x(j);
			}
	}
	x = result;
	return x;
}



ostream& operator<<(ostream &out, const column_vector &x)
{
	for (int i = 0; i < x.n; i++)
		out << x.vec[i] << " ";
	out << endl;
	return out;
}

square_matrix& rotation_xy(double angle)
{
	static square_matrix S(3);

	S(0, 0) = cos(angle);
	S(0, 1) = -sin(angle);
	S(0, 2) = 0;

	S(1, 0) = sin(angle);
	S(1, 1) = cos(angle);
	S(1, 2) = 0;

	S(2, 0) = 0;
	S(2, 1) = 0;
	S(2, 2) = 1;
	
	return S;
}

square_matrix& rotation_yz(double angle)
{
	static square_matrix S(3);

	S(0, 0) = 1;
	S(0, 1) = 0;
	S(0, 2) = 0;

	S(1, 0) = 0;
	S(1, 1) = cos(angle);
	S(1, 2) = -sin(angle);

	S(2, 0) = 0;
	S(2, 1) = sin(angle);
	S(2, 2) = cos(angle);

	return S;
}

square_matrix& rotation_zx(double angle)
{
	static square_matrix S(3);

	S(0, 0) = cos(angle);
	S(0, 1) = 0;
	S(0, 2) = sin(angle);

	S(1, 0) = 0;
	S(1, 1) = 1;
	S(1, 2) = 0;

	S(2, 0) = -sin(angle);
	S(2, 1) = 0;
	S(2, 2) = cos(angle);

	return S;
}