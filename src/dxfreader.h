#pragma once
#include "geometry.h"


//Reads the dxf 2007 file and and converts it into a vector of lines
void read_file(std::ifstream &, const std::string, std::vector<line> &, double dx);

//Interprets the different entities present in the dxf file
//This function is only used by read_file
void interpret_data(std::vector<line> &, std::vector<std::string> &, double = 1);

//Takes in a line segment and divides it into number of line segments based on resolution(dx)
//of equal length
//This function is only used by interpret data
void refine_line(std::vector<line> &, pos, pos, double = 1);

//Takes in a arc of a circle and divides it into number of line segments based on resolution(dx)
//of equal length
//This function is only used by interpret data
void refine_arc(std::vector<line> &, pos centre, double radius, double start_angle, double end_angle, double = 1);

//Resizes the centres the data so that it can be drawn on the screen whose
//coordinates range from [-1,1] in both axes
//This function is only used by read_file
void resize_data(std::vector<line> &);