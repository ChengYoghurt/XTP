#pragma once

/*
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
*/

void prepare_diff (const char* market_data_path, const char* hdf5_file_path);