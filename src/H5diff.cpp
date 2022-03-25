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
#include "H5diff.h"
#include "Typedefs.h"

#define MAX_DATA_ATTR 43
#define TICKER_NO -1

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::find;

extern struct DepthMarketField dmfeild;
static bool ticker_first_record = true;

// Files to record differences
const char* wrong_file_path = "wrong.txt";
const char* repeat_file_path = "repeat.txt";
const char* absent_file_path = "absent.txt";

void eliminate_wrong_time (vector <l2agg::timestamp_t>& data_vec, vector <l2agg::timestamp_t>& std_data_vec, string& ticker);
void create_index (vector <l2agg::timestamp_t>& data_vec, std::map <l2agg::timestamp_t, int>& time_index_m, vector <int> &repeatime_index_vec);
template <typename T>
void record_wrong (string& wrong_ticker, const char* wrong_attr, T const& wrong_data, const char* wrong_path);

void prepare_diff (const char* market_data_path, const char* hdf5_file_path) {

    hid_t file_id = H5Fopen(hdf5_file_path, H5F_ACC_RDONLY, H5P_DEFAULT);
    hsize_t dims;

    // Only open the first group "FeaturesDB" in the file
    hid_t group_id = H5Gopen(file_id, "FeaturesDB", H5P_DEFAULT);
    hid_t ticker_group_id; 
    hid_t ticker_dataset_id;
    hid_t ticker_dataspace_id;
    hid_t datatype;
    H5G_info_t ginfo;
    herr_t status;

    string ticker_id;

    int data_attrno = TICKER_NO;
    unsigned long long lino_no = 0; 
    std::map <l2agg::timestamp_t, int> time_index_map;
    vector <int> repeatime_index_vec;
    
    // Open market_data file
    std::ifstream market_data_infile;
    string data_line;
    market_data_infile.open(market_data_path, std::ios::in);
    if (!market_data_infile)  
        cout << "Failed to open market data file" << endl;
    else {
        //TODO:Read from .csv 
        // Assume tickers id increase by each line 
        // 1.read ticker and compare with the group name, if the same
        // 2.push the depthmarket_time into a vector
        // 3.compare the time_vec with standard_time_vec using A FUNCTION, and return a int index[]
        // Use the index[] and compare other data from .csv
        //market_data_infile
        bool jump_flag = false;
        while (getline(market_data_infile, data_line)) {
            ++lino_no;
            if (jump_flag == true) {
                if (data_line!= "#") continue;
                else {
                data_attrno = -1;
                jump_flag = false;
                continue;
                }
            } 
            // Read ticker_id from the second column
            if (data_attrno == TICKER_NO) {
                // cout << data_line << endl;
                std::stringstream ss(data_line);
                string data;
                vector<string> data_vec;

                while (getline(ss, data, ',')) 
                    data_vec.push_back(data);

                ticker_id = data_vec.back();
                cout << ticker_id << endl;

                if ((ticker_group_id = H5Gopen(group_id, ticker_id.c_str(), H5P_DEFAULT)) >= 0) {
                    status = H5Gget_info(ticker_group_id, &ginfo);
                }
                else {
                    jump_flag = true;
                    continue;
                }
            }
            else {  
                // Only open ticker_group once per ticker
                if (data_attrno < MAX_DATA_ATTR) {
                    std::stringstream ss(data_line);
                    string data;

                    if (data_attrno == l2agg::dmfeild.DepthMarketTime) {
                        vector<l2agg::timestamp_t> data_vec;
                        while (getline(ss, data, ',')) 
                            data_vec.push_back(strtoul(data.c_str(), NULL, 0));

                        // Start read from hdf5 
                        ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(l2agg::dmfeild.DepthMarketTime).c_str(), H5P_DEFAULT);
                        ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                        datatype = H5Dget_type(ticker_dataset_id);
                        H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                        vector <l2agg::timestamp_t> depth_market_time_vec;
                        depth_market_time_vec.resize(dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, depth_market_time_vec.data());

                        // Eliminate wrong time data
                        eliminate_wrong_time(data_vec, depth_market_time_vec, ticker_id);
                        cout << "ELIMINATE WRONG TIME successfully-------" << endl;
                        
                        // Create time-index map and repeattime index vec
                        create_index(data_vec, time_index_map, repeatime_index_vec);
                        cout << "CREATE INDEX successfully-------" << endl;
                        H5Tclose(datatype);
                        H5Sclose(ticker_dataspace_id);
                        H5Dclose(ticker_dataset_id);
                        
                    } 
                    /*    
                    else if(i == 56){
                        uint32_t* data = (uint32_t* )malloc(sizeof(uint32_t) *dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

                        //TODO:ticker_first_record SET TO TRUE
                    }
                    else if((i == 1) || (i >= 17 && i<=26) || (i >= 37 && i <= 48) || (i == 50) || (i == 51)){
                        int32_t* data = (int32_t* )malloc(sizeof(int32_t) *dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);


                    }
                    else{
                        double* data = (double* )malloc(sizeof(double) *dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

                        
                    }
                    */
  
                }               
            }
            data_attrno += 1;
            // After read every data attr, reset data_attrno
            // Be careful with uint vs int
            if ((data_attrno != -1) && (data_attrno > MAX_DATA_ATTR)) {
                // cout << "I M BIGGER-------------------------" << endl;
                data_attrno = -1;
                H5Gclose(ticker_group_id);
            }
        }
        // Release and close
        H5Gclose(group_id);
        H5Fclose(file_id);
    }
        
    market_data_infile.close();  
    
    
}

void eliminate_wrong_time (vector <l2agg::timestamp_t>& data_vec, vector <l2agg::timestamp_t>& std_data_vec, string& ticker) {
    l2agg::timestamp_t data_time;
    for (vector<l2agg::timestamp_t>::iterator it_data_vec = data_vec.begin(); it_data_vec != data_vec.end(); ) { 
        // Find my data in standard data 
        vector<l2agg::timestamp_t>::iterator it_find = find(std_data_vec.begin(), std_data_vec.end(), *it_data_vec);
        // Record wrong data and erase it
        if (it_find == std_data_vec.end()) {    
            record_wrong(ticker, "Market_depth_time", *it_data_vec, wrong_file_path);
            // it_data_vec = data_vec.erase(it_data_vec);
            *it_data_vec++ = 0;
        } else {
            it_data_vec++;  
        }  
    }  
}

// Record wrong data
template <typename T>
void record_wrong (string& wrong_ticker, const char* wrong_attr, T const& wrong_data, const char* wrong_path) { 
    std::ofstream wrong_out_file;
    wrong_out_file.open(wrong_path, std::ios::app);
    if (wrong_out_file) {
        if (ticker_first_record) {
            ticker_first_record = false; // Reset it to true after examing all attrs in tickers
            wrong_out_file << "Ticker_id: " << wrong_ticker << endl;
        }
        // Assume there's just few mistakes
        // So might be ok to write the same attr_name for multiple times
        // Also ok for write one data at a time
        wrong_out_file << wrong_attr << "\t" << wrong_data << "\n";
        wrong_out_file.close();
    }
    
} 

// time_index_map indicates the index of the records that we will conduct further comparison on
// repeatime_index_vec indicates the index of the records whose data_time occurs more than once
void create_index (vector <l2agg::timestamp_t>& data_vec, std::map <l2agg::timestamp_t, int>& time_index_m, 
vector <int> &repeatime_index_vec) {
    int index = 0;
    l2agg::timestamp_t data_time;
    std::set <l2agg::timestamp_t> repeat_key_s;
    for (auto data_time : data_vec) {
        // If datatime is zero, its wrong time data
        data_time = data_vec[index];
        if (data_time) {
            auto ret = time_index_m.insert({data_time, index});
        
            // Insert fail
            // ret.first refers to iter to repeat pair
            // ret.second is a bool: false when insertion fail
            if (!ret.second) {
                // Should not delete the repeat pair now, leave it before loop finishes
                repeat_key_s.insert(ret.first->first);
                repeatime_index_vec.push_back(index);
            }
            ++index;
        }    
    }
    
    for (auto repeat_key : repeat_key_s) {
        auto iter_m = time_index_m.find(repeat_key);
        repeatime_index_vec.push_back(iter_m->second);
        time_index_m.erase(iter_m);
    }
    
}
