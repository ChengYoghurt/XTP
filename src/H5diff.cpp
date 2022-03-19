#include "H5diff.h"


void diff (std::vector<std::string> vec_query_ticker, const char* hdf5_file_name) {

    hid_t file_id = H5Fopen(hdf5_file_name, H5F_ACC_RDONLY, H5P_DEFAULT);
    hsize_t dims;

    // only open the first group "FeaturesDB" in the file
    hid_t group_id = H5Gopen(file_id, "FeaturesDB", H5P_DEFAULT);
    hid_t ticker_group_id; 

    for(auto ticker : vec_query_ticker) {
        if ((ticker_group_id = H5Gopen(group_id, ticker.c_str(), H5P_DEFAULT)) >= 0) {
            std::cout << "OPEN" << ticker << "SUCCESSFULLY!===========================" << std::endl;

            H5Gclose(ticker_group_id);
        }
    }

    H5Gclose(group_id);
    H5Fclose(file_id);
    
}