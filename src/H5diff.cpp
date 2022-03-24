#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "hdf5.h"
#include "quote_spi.h"
#include "H5diff.h"
#include "Typedefs.h"

#define MAX_DATA_ATTR 43
#define TICKER_NO -1

using std::cout;
using std::endl;
using std::string;

extern struct DepthMarketField dmfeild;
void prepare_diff (const char* market_data_path, const char* hdf5_file_path){

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

    // Open market_data file
    std::ifstream market_data_infile;
    string data_line;
    string ticker_id;
    int data_attrno = TICKER_NO; 
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
                std::vector<string> data_vec;

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
                for (size_t i = 0; i < MAX_DATA_ATTR; i++) {
                    // Start read from hdf5  
                    if(i == l2agg::dmfeild.DepthMarketTime){
                        ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(l2agg::dmfeild.DepthMarketTime).c_str(), H5P_DEFAULT);
                        
                        
                        ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                        datatype = H5Dget_type(ticker_dataset_id);
                        H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                        std::vector <l2agg::timestamp_t> vec_depth_market_time;
                        vec_depth_market_time.resize(dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, vec_depth_market_time.data());
                        H5Tclose(datatype);
                        H5Sclose(ticker_dataspace_id);
                        H5Dclose(ticker_dataset_id);
                        
                    } 
                    /*    
                    else if(i == 56){
                        uint32_t* data = (uint32_t* )malloc(sizeof(uint32_t) *dims);
                        H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

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