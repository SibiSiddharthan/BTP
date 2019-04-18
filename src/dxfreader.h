#ifndef DXFREADER_H
#define DXFREADER_H

/**
 * @file dxfreader.h
 * @brief Defines the functions necessary to read abd process 
 * a dxf 2007 format file
 * @date 2019-04-17
 * 
 */
#include "geometry.h"


/**
 * @brief Reads the dxf 2007 file and and converts it into a vector of lines
 * 
 * @param in input filestream
 * @param filename 
 * @param line_list the vector of lines
 * @param dx a measure of the length of lines
 */
void read_file(std::ifstream &in, const std::string filename, std::vector<line> &line_list, double dx);

/**
 * @brief Interprets the different entities present in the dxf file
 * 
 * @param l The line list passed by read_file
 * @param d The string data formed by read_file
 * @param dx a measure of the length of lines
 */
void interpret_data(std::vector<line> &l, std::vector<std::string> &d, double dx = 1);

/**
 * @brief Takes in a line segment and divides it into number of line segments based on resolution(dx)
 * of equal length
 * @param l The line list
 * @param p1 start point of given line
 * @param p2 end point of given line
 * @param dx a measure of the length of lines
 */
void refine_line(std::vector<line> &l, pos p1, pos p2, double dx = 1);

/**
 * @brief Takes in a arc of a circle and divides it into number of line segments based on resolution(dx)
 * of equal length
 * @param centre centre of the circle
 * @param radius radius of the circle
 * @param start_angle 
 * @param end_angle 
 */
void refine_arc(std::vector<line> &, pos centre, double radius, double start_angle, double end_angle, double dx = 1);

/**
 * @brief Resizes the centres the data so that it can be drawn on the screen whose
 * coordinates range from [-1,1] in both axes
 * 
 */
void resize_data(std::vector<line> &);

#endif