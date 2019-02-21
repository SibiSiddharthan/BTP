#include "stl_reader.h"

using namespace std;

void read_stl(ifstream &in, const string filename, vector<plane> &planes)
{
	vector<string> data;
	in.open(filename, ios_base::in);
	if (in)
	{
		cout << "file has been opened\n";
		string file_string;
		int i = 0;
		plane p;
		while (getline(in, file_string))
		{
			if (file_string.find("normal") != string::npos)
			{
				string temp;

				uint64_t k = 0;
				for (k = 0; k < file_string.size(); ++k)
				{
					if (file_string[k] == 'l')
					{
						break;
					}
				}

				for (k = k + 2; k < file_string.size(); ++k)
				{
					if (file_string[k] != ' ')
						temp.push_back(file_string[k]);
					else
						break;
				}
				p.normal.x = stod(temp);
				temp.clear();

				for (k = k + 1; k < file_string.size(); ++k)
				{
					if (file_string[k] != ' ')
						temp.push_back(file_string[k]);
					else
						break;
				}
				p.normal.y = stod(temp);
				temp.clear();

				for (k = k + 1; k < file_string.size(); ++k)
				{
					if (file_string[k] != ' ')
						temp.push_back(file_string[k]);
					else
						break;
				}
				p.normal.z = stod(temp);
				temp.clear();
			}

			if (file_string.find("vertex") != string::npos)
			{
				string temp;

				if (i == 0)
				{
					uint64_t k = 0;

					for (k = 0; k < file_string.size(); ++k)
					{
						if (file_string[k] == 'x')
							break;
					}

					for (k = k + 2; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}
					p.a.x = stod(temp);
					temp.clear();

					for (k = k + 1; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}
					p.a.y = stod(temp);
					temp.clear();

					for (k = k + 1; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}
					p.a.z = stod(temp);
					temp.clear();

					++i;
				}

				else if (i == 1)
				{
					uint64_t k = 0;

					for (k = 0; k < file_string.size(); ++k)
					{
						if (file_string[k] == 'x')
							break;
					}

					for (k = k + 2; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}

					p.b.x = stod(temp);
					temp.clear();

					for (k = k + 1; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}

					p.b.y = stod(temp);
					temp.clear();

					for (k = k + 1; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}

					p.b.z = stod(temp);
					temp.clear();

					++i;
				}

				else //if(i==2)
				{
					uint64_t k = 0;

					for (k = 0; k < file_string.size(); ++k)
					{
						if (file_string[k] == 'x')
							break;
					}

					for (k = k + 2; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}

					p.c.x = stod(temp);
					temp.clear();

					for (k = k + 1; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}

					p.c.y = stod(temp);
					temp.clear();

					for (k = k + 1; k < file_string.size(); ++k)
					{
						if (file_string[k] != ' ')
							temp.push_back(file_string[k]);
						else
							break;
					}

					p.c.z = stod(temp);
					temp.clear();
					++i;
				}
			}

			if (i == 3)
			{
				planes.push_back(p);
				i = 0;
				//cout << p.a << p.b << p.c << p.normal;
			}
		}
	}

	else
	{
		cout << "file not found\n";
	}
}
