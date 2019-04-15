#ifndef PREPROCESS_H
#define PREPROCESS_H

/**
 * @file preprocess.h
 * @brief Defines the C++ Standard library
 * files that are used throughout the code
 * 
 */

//For MSVC compatibility
#ifdef _MSVC_VER
#define and &&
#define or ||
#define not !
#define in :
#endif

//* Acceptable error in comparisons is sqrt(epsilon)
/**
 * @brief Tolerance for distinguishing two points
 */
constexpr double epsilon = 1.0e-14;

/**
 * @brief Constant pi
 */
constexpr double pi = 3.14159265358979323;

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <tuple>
#include <utility>
#include <vector>
#include <functional>

#endif  