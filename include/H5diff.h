#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "hdf5.h"
#include "quote_spi.h"
using namespace std;

void compare_time(uint32_t* data_h5, string& ticker, MyQuoteSpi* spi, 
                    map<uint32_t,uint32_t> &map_index, hid_t dims);

template<class T>
void compare_uint(T* data_h5, string& ticker, MyQuoteSpi* spi, 
                    hsize_t dims, map<uint32_t,uint32_t> &map_index, vector<uint32_t>& vec);

template<class T>
void compare_int(T* data_h5, string& ticker, MyQuoteSpi* spi, 
                    hsize_t dims, map<uint32_t,uint32_t> &map_index, vector<int32_t>& vec);

template<class T>
void compare_double(T* data_h5, string& ticker, MyQuoteSpi* spi, 
                    hsize_t dims, map<uint32_t,uint32_t> &map_index, vector<double>& vec);

void diff (std::vector<std::string> vec_query_ticker, const char* hdf5_file_name, MyQuoteSpi* spi);