#ifndef STL_READER_H
#define STL_READER_H
/**
 * @file stl_reader.h
 * @brief This file defines functions neccessary for reading
 * stl files
 * @date 2019-04-17
 * 
 */

#include "geometry.h"

/**
 * @brief Reads a stl file and forms a vector of planes
 * 
 * @param in 
 * @param filename 
 * @param planes The vector of planes formed
 */
void read_stl(std::ifstream &in, const std::string filename,std::vector<plane> &planes);

#endif