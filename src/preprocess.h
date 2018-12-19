#ifndef PREPROCESS_H
#define PREPROCESS_H

//For MSVC compatibility
#ifdef _MSVC_VER
#define and &&
#define or ||
#define not !
#endif

//Acceptable error in comparisons
const double epsilon = 1.0e-10;

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <unordered_map>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#endif  