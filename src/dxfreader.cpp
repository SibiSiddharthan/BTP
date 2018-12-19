#include "dxfreader.h"
using namespace std;

//updated on 29/7/2018
void read_file(ifstream &in, const string filename, vector<line> &line_list, double dx)
{
	vector<string> data;
	int polyline_count;
	in.open(filename, ios_base::in);
	if (in)
	{
		cout << "file has been opened\n";
		string file_string;
		while (getline(in, file_string))
		{

			if (file_string == "AcDbPolyline")
			{

				data.push_back("POLYLINE");

				getline(in, file_string); //90
				getline(in, file_string); //polyline count
				polyline_count = stoi(file_string);

				for (int i = 0; i < 4; i++) //skips to coordinnates
					getline(in, file_string);

				for (int i = 0; i < polyline_count; i++)
				{
					getline(in, file_string); //10
					getline(in, file_string); //x
					data.push_back(file_string);

					getline(in, file_string); //20
					getline(in, file_string); //y
					data.push_back(file_string);
				}

				data.push_back("END");
			}

			else if (file_string == "AcDbLine")
			{
				data.push_back("LINE");

				getline(in, file_string); //10
				getline(in, file_string); //x1
				data.push_back(file_string);

				getline(in, file_string); //20
				getline(in, file_string); //y1
				data.push_back(file_string);

				getline(in, file_string); //11
				getline(in, file_string); //x2
				data.push_back(file_string);

				getline(in, file_string); //21
				getline(in, file_string); //y2
				data.push_back(file_string);

				data.push_back("END");
			}

			else if (file_string == "AcDbPoint")
			{
				data.push_back("POINT");

				getline(in, file_string); //10
				getline(in, file_string); //x
				data.push_back(file_string);

				getline(in, file_string); //20
				getline(in, file_string); //y
				data.push_back(file_string);

				data.push_back("END");
			}

			else if (file_string == "AcDbCircle")
			{
				data.push_back("CIRCLE");

				getline(in, file_string); //10
				getline(in, file_string); //x
				data.push_back(file_string);

				getline(in, file_string); //20
				getline(in, file_string); //y
				data.push_back(file_string);

				getline(in, file_string); //40
				getline(in, file_string); //radius
				data.push_back(file_string);

				getline(in, file_string); //to check whether there is an arc
				getline(in, file_string);
				if (file_string == "AcDbArc")
				{
					data.push_back("ARC");

					getline(in, file_string); //50
					getline(in, file_string); //start angle absolute
					data.push_back(file_string);

					getline(in, file_string); //51
					getline(in, file_string); //end angle absolute
					data.push_back(file_string);
				}
				data.push_back("END");
			}
		}
	}
	else
	{
		cout << "file can't be opened\n";
	}

	in.close();
	//parse_to_domain(pos_list, temp);

	//for (int i = 0; i < data.size(); i++)
	//cout << data[i] << endl;

	//cout << "end of data\n";
	interpret_data(line_list, data, dx);
	resize_data(line_list);
}

//updated on 29/7/2018
void interpret_data(vector<line> &l, vector<string> &d, double dx)
{

	pos p1, p2, centre;
	double radius, start_angle, end_angle;

	for (int i = 0; i < d.size(); i++)
	{
		if (d[i] == "LINE")
		{
			i++;

			p1.x = stod(d[i]);
			p1.y = stod(d[i + 1]);
			p2.x = stod(d[i + 2]);
			p2.y = stod(d[i + 3]);

			refine_line(l, p1, p2, dx);
			i += 4;
		}

		else if (d[i] == "POLYLINE")
		{

			i++;
			while (d[i + 2] != "END")
			{
				p1.x = stod(d[i]);
				p1.y = stod(d[i + 1]);
				p2.x = stod(d[i + 2]);
				p2.y = stod(d[i + 3]);

				refine_line(l, p1, p2, dx);
				i += 2;
			}
		}

		else if (d[i] == "CIRCLE")
		{
			i++;

			centre.x = stod(d[i]);
			centre.y = stod(d[i + 1]);
			radius = stod(d[i + 2]);
			if (d[i + 3] == "ARC")
			{
				start_angle = stod(d[i + 4]);
				end_angle = stod(d[i + 5]);
				refine_arc(l, centre, radius, start_angle, end_angle, dx);
				i += 6;
			}

			else
			{
				refine_arc(l, centre, radius, 0, 360, dx);
				i += 3;
			}
		}
	}

	/*for (int i = 0; i < l.size(); i++)
	{
		cout << l[i].start << l[i].end << endl;
	}*/
	//cout << l.size()<< endl;
}

//updated on 29/7/2018
void refine_line(std::vector<line> &l, pos p1, pos p2, double dx)
{

	double length = distance(p1, p2);
	int number_of_divisions = length / dx;
	int n = number_of_divisions; // just an alias
	//cout << length << endl;
	if (number_of_divisions > 1)
	{
		for (int j = 0; j < number_of_divisions; j++)
		{
			l.push_back({(p2 * (j * 1.0 / n)) + (p1 * ((n - j) * 1.0 / n)), (p2 * ((j + 1) * 1.0 / n)) + (p1 * ((n - j - 1) * 1.0 / n))});
			//cout << (p2*(j*1.0 / n)) + (p1*((n - j)*1.0 / n)) << (p2*((j + 1)*1.0 / n)) + (p1*((n - j - 1)*1.0 / n));
		}
	}

	else
	{
		l.push_back({p1, p2});
	}

	/*for (int i = 0; i < l.size(); i++)
	{
		cout << l[i].start << l[i].end << endl;
	}*/
}

//updated on 29/7/2018
void refine_arc(std::vector<line> &l, pos centre, double radius, double start_angle, double end_angle, double dx)
{
	double length, dtheta;
	int number_of_divisions;
	int n; // just an alias
	pos temp1, temp2;

	if (start_angle == 0 && end_angle == 360)
	{
		length = 2 * pi_ * radius;
		number_of_divisions = length / dx;
		n = number_of_divisions;
		dtheta = 2 * pi_ / n;

		if (number_of_divisions > 1)
		{
			for (int i = 0; i <= number_of_divisions; i++)
			{
				temp1.x = centre.x + radius * cos(i * dtheta);
				temp1.y = centre.y + radius * sin(i * dtheta);

				temp2.x = centre.x + radius * cos((i + 1) * dtheta);
				temp2.y = centre.y + radius * sin((i + 1) * dtheta);

				l.push_back({temp1, temp2});
			}
		}
	}

	else
	{
		if (start_angle == end_angle)
			return;

		else if (start_angle < end_angle)
		{
			length = radius * (end_angle - start_angle) * pi_ / 180;
			number_of_divisions = length / dx;
			n = number_of_divisions;
			//cout << n;
			dtheta = (end_angle - start_angle) * pi_ / 180 / n;
			//cout << start_angle << " " << end_angle << endl;
			if (number_of_divisions > 1)
			{
				for (int i = 0; i < number_of_divisions; i++)
				{
					temp1.x = centre.x + radius * cos(start_angle * pi_ / 180 + i * dtheta);
					temp1.y = centre.y + radius * sin(start_angle * pi_ / 180 + i * dtheta);

					temp2.x = centre.x + radius * cos((i + 1) * dtheta + start_angle * pi_ / 180);
					temp2.y = centre.y + radius * sin((i + 1) * dtheta + start_angle * pi_ / 180);

					l.push_back({temp1, temp2});

					//cout << (start_angle  + i * dtheta*180/pi) << endl;
				}
			}
		}

		else if (start_angle > end_angle)
		{
			end_angle += 360;
			length = radius * (end_angle - start_angle) * pi_ / 180;
			number_of_divisions = length / dx;
			n = number_of_divisions;
			//cout << n;
			dtheta = (end_angle - start_angle) * pi_ / 180 / n;

			if (number_of_divisions > 1)
			{
				for (int i = 0; i < number_of_divisions; i++)
				{
					temp1.x = centre.x + radius * cos(start_angle * pi_ / 180 + i * dtheta);
					temp1.y = centre.y + radius * sin(start_angle * pi_ / 180 + i * dtheta);

					temp2.x = centre.x + radius * cos((i + 1) * dtheta + start_angle * pi_ / 180);
					temp2.y = centre.y + radius * sin((i + 1) * dtheta + start_angle * pi_ / 180);

					l.push_back({temp1, temp2});
				}
			}
		}
	}
	/*for (int i = 0; i < l.size(); i++)
	{
		cout << l[i].start << l[i].end << endl;
	}
	cout << endl << endl << endl;*/
}

//updated on 29/7/2018
void resize_data(vector<line> &l)
{
	double max_dist = 0, temp1, temp2;

	/*Shift the origin to the centroid of the figure*/
	pos centroid = {};
	for (int i = 0; i < l.size(); i++)
	{
		centroid = centroid + ((l[i].start + l[i].end) * 0.5);
	}

	centroid = centroid / l.size();

	for (int i = 0; i < l.size(); i++)
	{
		l[i].start = l[i].start - centroid;
		l[i].end = l[i].end - centroid;
	}

	/*Resize the figure to fit in the window*/
	for (int i = 0; i < l.size(); i++)
	{
		temp1 = distance(l[i].start, {0, 0, 0});
		temp2 = distance(l[i].end, {0, 0, 0});
		if (max(temp1, temp2) > max_dist)
		{
			max_dist = max(temp1, temp2);
		}
	}

	if (max_dist > 1)
	{
		for (int i = 0; i < l.size(); i++)
		{
			l[i].start = l[i].start * 0.8 / max_dist;
			l[i].end = l[i].end * 0.8 / max_dist;
		}
	}

	/*for (int i = 0; i < l.size(); i++)
	{
		cout << l[i].start << l[i].end << endl;
	}*/
}