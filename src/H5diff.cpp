#include "H5diff.h"

#define MAX_DATA_ATTR 43
#define TICKER_NO -1

// define offsets between my data's index and standard tuple
#define TOV_OFF_SET 47
#define QTY_OFF_SET 46
#define BID_OFF_SET 23
#define ASK_OFF_SET -17

struct l2agg::DepthMarketField dmfeild;

//extern struct DepthMarketField dmfeild;
static bool ticker_first_record = true;

// Files to record differences
const char* wrong_file_path = "wrong.txt";
const char* repeat_file_path = "repeat.txt";
const char* absent_file_path = "absent.txt";

/*
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
*/
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
    string attr_name;
    string bid_or_ask;
    string bid_ask_no;

    int data_attrno = TICKER_NO;
    unsigned long long lino_no = 0; 
    vector <l2agg::timestamp_t> depth_market_time_vec;
    std::map <l2agg::timestamp_t, size_t> time_index_map;
    vector <size_t> time_index_vec;
    vector <size_t> repeatime_index_vec;
    vector <int> isrepeat_correct_vec;
    std::set <l2agg::timestamp_t> repeatime_s;
    
    // Open market_data file
    std::ifstream market_data_infile;
    string data_line;
    market_data_infile.open(market_data_path, std::ios::in);
    if (!market_data_infile)  
        cout << "Failed to open market data file" << endl;
    else {
        // Assume tickers id increase by each line 
        // 1.read ticker and compare with the group name, if the same
        // 2.push the depthmarket_time into a vector
        // 3.compare the time_vec with standard_time_vec using A FUNCTION, and return a int index[]
        // Use the index[] and compare other data from .csv
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

                    if (data_attrno == dmfeild.DepthMarketTime) {
                        vector<l2agg::timestamp_t> data_vec;
                        while (getline(ss, data, ',')) 
                            data_vec.push_back(strtoul(data.c_str(), NULL, 0));

                        // Start read from hdf5 
                        ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(dmfeild.DepthMarketTime).c_str(), H5P_DEFAULT);
                        ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                        datatype = H5Dget_type(ticker_dataset_id);
                        H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                        depth_market_time_vec.resize(dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, depth_market_time_vec.data());

                        // Eliminate wrong time data
                        eliminate_wrong_time(data_vec, depth_market_time_vec, ticker_id);
                        // cout << "ELIMINATE WRONG TIME successfully-------" << endl;
                        
                        // Create time-index map and repeattime index vec
                        create_index(data_vec, time_index_map, repeatime_index_vec);
                        // cout << "CREATE INDEX successfully-------" << endl;
                        
                        // Find absent
                        eliminate_absent (data_vec, depth_market_time_vec, ticker_id);

                        // Initiate time_index_vec
                        for (auto time_index_pair : time_index_map) {
                            time_index_vec.push_back(time_index_pair.second);
                            // cout << time_index_pair.first << endl;
                        }
                        time_index_map.clear();

                        // Initiate isrepeat_correct_vec
                        for (auto repeatime_index : repeatime_index_vec) {
                            isrepeat_correct_vec.push_back(1);
                        }

                        H5Tclose(datatype);
                        H5Sclose(ticker_dataspace_id);
                        H5Dclose(ticker_dataset_id);
                        
                    } 
                    else if ((data_attrno + TOV_OFF_SET == dmfeild.Turnover)
                    || ((data_attrno + BID_OFF_SET) >= dmfeild.BidPrice1 && (data_attrno + BID_OFF_SET) <= dmfeild.BidPrice10)
                    || ((data_attrno + ASK_OFF_SET) >= dmfeild.AskPrice1 && (data_attrno + ASK_OFF_SET) < dmfeild.AskPrice10)) {
                        vector<l2agg::price_t> data_vec;
                        while (getline(ss, data, ',')) 
                            data_vec.push_back(atof(data.c_str()));
                        
                        // Start read from hdf5 
                        ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(data_attrno + TOV_OFF_SET).c_str(), H5P_DEFAULT);
                        ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                        datatype = H5Dget_type(ticker_dataset_id);
                        H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                        vector <l2agg::price_t> price_vec;
                        price_vec.resize(dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, price_vec.data());

                        // Before compare, set attr_name
                        if (data_attrno + TOV_OFF_SET == dmfeild.Turnover) attr_name = "Turnover";
                        else if ((data_attrno + BID_OFF_SET) >= dmfeild.BidPrice1 && (data_attrno + BID_OFF_SET) <= dmfeild.BidPrice10) {
                            bid_or_ask = "Bid";
                            bid_ask_no = std::to_string((data_attrno + BID_OFF_SET) - dmfeild.BidPrice1);
                            attr_name = bid_or_ask + "Price" + bid_ask_no;
                        }
                        else {
                            bid_or_ask = "Ask";
                            bid_ask_no = std::to_string((data_attrno + BID_OFF_SET) - dmfeild.BidPrice1);
                            attr_name = bid_or_ask + "Price" + bid_ask_no;
                        }
                        // Use COMPARE on data_vec and price_vec
                        eliminate_wrong_attr (ticker_id, attr_name, data_vec, price_vec, time_index_vec);

                        // Compare repeat data with standard data
                        for (auto repeat_index : repeatime_index_vec) {
                            if (data_vec[repeat_index] != price_vec[repeat_index])
                                isrepeat_correct_vec[repeat_index] = 0;
                        }
                    }
                    else if ((data_attrno + QTY_OFF_SET == dmfeild.Volume)
                    || ((data_attrno + BID_OFF_SET) >= dmfeild.BidVol1 && (data_attrno + BID_OFF_SET) <= dmfeild.BidVol10)
                    || ((data_attrno + ASK_OFF_SET) >= dmfeild.AskVol1 && (data_attrno + ASK_OFF_SET) < dmfeild.AskVol10)) {
                        vector<l2agg::volume_t> data_vec;
                        while (getline(ss, data, ',')) 
                            data_vec.push_back(atoi(data.c_str()));

                        // Start read from hdf5 
                        ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(data_attrno + BID_OFF_SET).c_str(), H5P_DEFAULT);
                        ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                        datatype = H5Dget_type(ticker_dataset_id);
                        H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                        vector <l2agg::volume_t> volume_vec;
                        volume_vec.resize(dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, volume_vec.data());

                        // Before compare, set attr_name
                        if (data_attrno + QTY_OFF_SET == dmfeild.Volume) attr_name = "Volume";
                        else if ((data_attrno + BID_OFF_SET) >= dmfeild.BidVol1 && (data_attrno + BID_OFF_SET) <= dmfeild.BidVol10) {
                            bid_or_ask = "Bid";
                            bid_ask_no = std::to_string((data_attrno + BID_OFF_SET) - dmfeild.BidVol1);
                            attr_name = bid_or_ask + "Vol" + bid_ask_no;
                        }
                        else {
                            bid_or_ask = "Ask";
                            bid_ask_no = std::to_string((data_attrno + BID_OFF_SET) - dmfeild.BidVol1);
                            attr_name = bid_or_ask + "Vol" + bid_ask_no;
                        }
                        // Use COMPARE on data_vec and volume_vec
                        eliminate_wrong_attr (ticker_id, attr_name, data_vec, volume_vec, time_index_vec);

                        // Compare repeat data with standard data
                        for (auto repeat_index : repeatime_index_vec) {
                            if (data_vec[repeat_index] != volume_vec[repeat_index])
                                isrepeat_correct_vec[repeat_index] = 0;
                        }
                    }
                    // If is last attr
                    else if(data_attrno + ASK_OFF_SET == dmfeild.AskVol10){
                        vector<l2agg::volume_t> data_vec;
                        while (getline(ss, data, ',')) 
                            data_vec.push_back(atoi(data.c_str()));

                        // Start read from hdf5 
                        ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(dmfeild.AskVol10).c_str(), H5P_DEFAULT);
                        ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                        datatype = H5Dget_type(ticker_dataset_id);
                        H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                        vector <l2agg::volume_t> ask_volume10_vec;
                        ask_volume10_vec.resize(dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, ask_volume10_vec.data());

                        // Use COMPARE on data_vec and ask_volume10_vec
                        attr_name = "AskVol10";
                        eliminate_wrong_attr (ticker_id, attr_name, data_vec, ask_volume10_vec, time_index_vec);

                        // Compare repeat data with standard data
                        for (auto repeat_index : repeatime_index_vec) {
                            if (data_vec[repeat_index] != ask_volume10_vec[repeat_index])
                                isrepeat_correct_vec[repeat_index] = 0;
                        }

                        // Use set to record data with correct repeat time
                        std::set <l2agg::timestamp_t> correct_repeat_s;
                        size_t is_correct_data_index = 0;
                        string memo;
                        for (auto is_correct : isrepeat_correct_vec) {
                            size_t repeat_index = repeatime_index_vec[is_correct_data_index];
                            l2agg::timestamp_t& repeat_time = depth_market_time_vec[repeat_index];
                            // If is_correct is 0, then the corresponding record indexed by is_correct_data_index is wrong
                            if (!is_correct) {
                                // Record this piece of data to wrong data
                                memo = "repeat T wrong data";
                                record_repeat (ticker_id, repeat_time, memo, repeat_index, repeat_file_path);
                            }
                            // If is_correct is 1, then the corresponding record indexed by is_correct_data_index is repeated
                            else {
                                
                                auto ret = correct_repeat_s.insert(repeat_time);
                                if (!ret.second) {
                                    // Record this piece of data to repeat data
                                    memo = "";
                                    record_repeat (ticker_id, repeat_time, memo, repeat_index, repeat_file_path);
                                }
                            }
                            ++is_correct_data_index;
                        }
                    }
                }               
            }
            data_attrno += 1;
            // After comparing all attr...
            // Be careful with uint vs int
            if ((data_attrno != -1) && (data_attrno > MAX_DATA_ATTR)) {
                // cout << "I M BIGGER-------------------------" << endl;
                // Reset data_attrno
                data_attrno = -1;
                // Reset ticker_first_record
                ticker_first_record = true;
                // Clear used vectors
                depth_market_time_vec.clear();
                time_index_vec.clear();
                repeatime_index_vec.clear();
                isrepeat_correct_vec.clear();
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

// Use the index_vec to compare other attr
template <typename T>
void eliminate_wrong_attr (string& ticker_id, string& wrong_attr, vector <T>& data_vec, vector <T>& std_data_vec, vector <size_t>& index) {
    for(size_t i_index = 0; i_index < index.size() ;i_index++){
        if(index[i_index] != -1){
            if(std_data_vec[i_index] != data_vec[index[i_index]]){
                record_wrong(ticker_id, wrong_attr.c_str(), std::make_pair(std_data_vec[i_index], data_vec[index[i_index]]), 
                                std::make_pair(i_index, index[i_index]), wrong_file_path);
            }
        }
    }

}

void eliminate_absent (vector <l2agg::timestamp_t>& data_vec, vector <l2agg::timestamp_t>& std_data_vec, string& ticker) {
    for(vector<l2agg::timestamp_t>::iterator it_std_vec = std_data_vec.begin(); it_std_vec != std_data_vec.end(); it_std_vec++) {
        vector<l2agg::timestamp_t>::iterator it_find = find(data_vec.begin(), data_vec.end(), *it_std_vec);
        if(it_find == data_vec.end()) {
            record_absent(ticker, *it_std_vec, absent_file_path);
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

template <typename T>
void record_wrong (string& wrong_ticker, const char* wrong_attr, pair<T, T> const& wrong_data, pair<size_t, size_t> const& wrong_index, const char* wrong_path) { 
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
        wrong_out_file << wrong_attr << "\tright_index: " << wrong_index.first  << "\tright_data: " << wrong_data.first 
                        << "\twrong_index: " << wrong_index.second << "\twrong_data: " << wrong_data.second << "\n";
        wrong_out_file.close();
    }
    
}

void record_repeat (string& ticker_id, l2agg::timestamp_t& repeat_time, string& memo, size_t& repeat_index, const char* repeat_path) {
    std::ofstream repeat_out_file;
    repeat_out_file.open(repeat_path, std::ios::app);
    if(repeat_out_file) {
        if (ticker_first_record) {
            ticker_first_record = false; // Reset it to true after examing all attrs in tickers
            repeat_out_file << "Ticker_id: " << ticker_id << endl;
        }
        repeat_out_file 
        << "\trepeat time: " << repeat_time 
        << "\tmemo:" << memo 
        << "\trepeat index: " << repeat_index << endl; 
    }
    repeat_out_file.close();
}

void record_absent (string ticker_id, l2agg::timestamp_t std_time, const char* absent_path) {
    std::ofstream absent_out_file;
    absent_out_file.open(absent_path, std::ios::app);
    if(absent_out_file) {
        if (ticker_first_record) {
            ticker_first_record = false; // Reset it to true after examing all attrs in tickers
            absent_out_file << "Ticker_id: " << ticker_id << endl;
        }
        absent_out_file << "absent time: " << std_time << endl; 
    }
    absent_out_file.close();
}

// time_index_map indicates the index of the records that we will conduct further comparison on
// repeatime_index_vec indicates the index of the records whose data_time occurs more than once
void create_index (vector <l2agg::timestamp_t>& data_vec, std::map <l2agg::timestamp_t, size_t>& time_index_m, vector <size_t> &repeatime_index_vec) {
    size_t index = 0;
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
