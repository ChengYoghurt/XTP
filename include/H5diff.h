#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "hdf5.h"
#include "quote_spi.h"
#include "TraderTypeDefs.h"
//FIX: have defined a struct in .h, may cause multipul defination

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::find;
using std::pair;

void eliminate_wrong_time (vector <l2agg::timestamp_t>& data_vec, vector <l2agg::timestamp_t>& std_data_vec, string& ticker);
template <typename T>
void eliminate_wrong_attr (string& ticker_id, string& wrong_attr, vector <T>& data_vec, vector <T>& std_data_vec, vector <size_t>& index);
void eliminate_absent (vector <l2agg::timestamp_t>& data_vec, vector <l2agg::timestamp_t>& std_data_vec, string& ticker);

template <typename T>
void record_wrong (string& wrong_ticker, const char* wrong_attr, T const& wrong_data, const char* wrong_path);
template <typename T>
void record_wrong (string& wrong_ticker, const char* wrong_attr, pair<T, T> const& wrong_data, pair<size_t, size_t> const& wrong_index, const char* wrong_path);
void record_repeat (string& ticker_id, l2agg::timestamp_t& repeat_time, string& memo, size_t& repeat_index, const char* repeat_path);
void record_absent (string ticker_id, l2agg::timestamp_t std_time, const char* absent_path);

void create_index (vector <l2agg::timestamp_t>& data_vec, std::map <l2agg::timestamp_t, size_t>& time_index_m, vector <size_t> &repeatime_index_vec);

void prepare_diff (const char* market_data_path, const char* hdf5_file_path);