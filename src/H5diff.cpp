#include "H5diff.h"


void diff (std::vector<std::string> vec_query_ticker, const char* hdf5_file_name) {

    hid_t file_id = H5Fopen(hdf5_file_name, H5F_ACC_RDONLY, H5P_DEFAULT);
    hsize_t dims;

    // only open the first group "FeaturesDB" in the file
    hid_t group_id = H5Gopen(file_id, "FeaturesDB", H5P_DEFAULT);
    hid_t ticker_group_id; 
    hid_t ticker_dataset_id;
    hid_t ticker_dataspace_id;
    hid_t datatype;
    H5G_info_t ginfo;
    herr_t status;

    // bool flag1 = true;
    for(auto ticker : vec_query_ticker) {
        if ((ticker_group_id = H5Gopen(group_id, ticker.c_str(), H5P_DEFAULT)) >= 0) {
            // std::cout << "=============" << "OPEN" << ticker << "SUCCESSFULLY!" << std::endl;
            status = H5Gget_info(ticker_group_id, &ginfo);
            // std::cout << "=============" << ginfo.nlinks << std::endl;
            // Open every dataset in each ticker group
            for (hsize_t i = 0; i < ginfo.nlinks - 1; i++) {
                ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(i).c_str(), H5P_DEFAULT);
                ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                datatype = H5Dget_type(ticker_dataset_id);
                H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                // std::cout << "=============" << "DIMS" << dims << "TYPE" << datatype << std::endl;
                
                // Start read from hdf5       
                if(i == 0 || i == 56){
                    uint32_t* data = (uint32_t* )malloc(sizeof(uint32_t) *dims);
                    H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
                }
                else if((i == 1) || (i >= 17 && i<=26) || (i >= 37 && i <= 48) || (i == 50) || (i == 51)){
                    int32_t* data = (int32_t* )malloc(sizeof(int32_t) *dims);
                    H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
                    // if (flag1) {
                    //     for(int k = 0; k < 10; k++)
                    //         std::cout<<data[k]<<"\t";
                    //     std::cout<<std::endl;
                    //     flag1 = false;
                    // }
                }
                else{
                    double* data = (double* )malloc(sizeof(double) *dims);
                    H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
                }
                
                // Release and close
                H5Sclose(ticker_dataspace_id);
                H5Dclose(ticker_dataset_id);
            }
            H5Gclose(ticker_group_id);
        }
    }
    H5Tclose(datatype);
    H5Gclose(group_id);
    H5Fclose(file_id);    
}