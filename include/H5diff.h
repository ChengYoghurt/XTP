#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "hdf5.h"

void diff (std::vector<std::string> vec_query_ticker, const char* hdf5_file_name);
