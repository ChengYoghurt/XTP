#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "hdf5.h"
#include "quote_spi.h"

void compare_time(uint32_t* data_h5, std::string& ticker, const MyQuoteSpi* spi, 
                    std::map<uint32_t,uint32_t> &map_index, hid_t dims);
template<class T>
void compare_data(T* data_h5, std::string& ticker, const MyQuoteSpi* spi, 
                    hsize_t dims, std::map<uint32_t,uint32_t> &map_index, std::vector<T>& vec);
