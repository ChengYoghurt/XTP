#include "H5diff.h"


using namespace std;

void compare_time(uint32_t* data_h5, string& ticker, MyQuoteSpi* spi, 
                    map<uint32_t,uint32_t> &map_index, hid_t dims){
    freopen("timewrong.txt","w",stdout);
    map<string, XTPDMET> map_xtpdmd = spi->get_xtpdmet();
    size_t numoftime= map_xtpdmd[ticker].vec_depthtime.size();
    for(hsize_t i = 0, j=0; i < dims, j < numoftime; i++, j++){
        while(map_xtpdmd[ticker].vec_depthtime[j] != data_h5[i]){
            j++;
        }
        if(j < numoftime){
            map_index[i] = j;
        }
    }

}

template<class T>
void compare_uint(T* data_h5, string& ticker, MyQuoteSpi* spi, 
                    hsize_t dims, map<uint32_t,uint32_t> &map_index, vector<uint32_t>& vec){ 
    freopen("wrongdata.txt","w",stdout);
    map<string, XTPDMET> map_xtpdmd = spi->get_xtpdmet();

    for(hsize_t i = 0, j = 0; i < map_index.size(); i++){
        j = map_index[i];
        if(data_h5[i] != vec[j]){
            cout <<"Time: " << map_xtpdmd[ticker].vec_depthtime[i] << ": index: (" 
            << i << "," << j << ") Right: " << data_h5[i] << "Wrong: " << vec[j] <<endl;  
        }
    }

}

template<class T>
void compare_int(T* data_h5, string& ticker, MyQuoteSpi* spi, 
                    hsize_t dims, map<uint32_t,uint32_t> &map_index, vector<int32_t>& vec){ 

    freopen("wrongdata.txt","w",stdout);
    map<string, XTPDMET> map_xtpdmd = spi->get_xtpdmet();

    for(hsize_t i = 0, j = 0; i < map_index.size(); i++){
        j = map_index[i];
        if(data_h5[i] != vec[j]){
            cout <<"Time: " << map_xtpdmd[ticker].vec_depthtime[i] << ": index: (" 
            << i << "," << j << ") Right: " << data_h5[i] << "Wrong: " << vec[j] <<endl;  
        }
    }

}

template<class T>
void compare_double(T* data_h5, string& ticker, MyQuoteSpi* spi, 
                    hsize_t dims, map<uint32_t,uint32_t> &map_index, vector<double>& vec){ 

    freopen("wrongdata.txt","w",stdout);
    map<string, XTPDMET> map_xtpdmd = spi->get_xtpdmet();

    for(hsize_t i = 0, j = 0; i < map_index.size(); i++){
        j = map_index[i];
        if(data_h5[i] != vec[j]){
            cout <<"Time: " << map_xtpdmd[ticker].vec_depthtime[i] << ": index: (" 
            << i << "," << j << ") Right: " << data_h5[i] << "Wrong: " << vec[j] <<endl;  
        }
    }

}

void diff (std::vector<std::string> vec_query_ticker, const char* hdf5_file_name, MyQuoteSpi* spi) {

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
    map<uint32_t,uint32_t> map_index;
    map<string, XTPDMET> map_xtpdmd = spi->get_xtpdmet();

    // bool flag1 = true;
    for(auto ticker : vec_query_ticker) {
        if ((ticker_group_id = H5Gopen(group_id, ticker.c_str(), H5P_DEFAULT)) >= 0) {
            status = H5Gget_info(ticker_group_id, &ginfo);

            for (hsize_t i = 0; i < ginfo.nlinks - 1; i++) {
                ticker_dataset_id = H5Dopen(ticker_group_id, std::to_string(i).c_str(), H5P_DEFAULT);
                ticker_dataspace_id = H5Dget_space(ticker_dataset_id);
                datatype = H5Dget_type(ticker_dataset_id);
                H5Sget_simple_extent_dims(ticker_dataspace_id, &dims, NULL);
                
                // Start read from hdf5  
                if(i == 0){
                    uint32_t* data = (uint32_t* )malloc(sizeof(uint32_t) *dims);
                    H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
                    compare_time(data, ticker, spi, map_index, dims);
                }     
                else if(i == 56){
                    uint32_t* data = (uint32_t* )malloc(sizeof(uint32_t) *dims);
                    H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
                    compare_uint(data, ticker, spi, dims, map_index, map_xtpdmd[ticker].vec_localtime);
                }
                else if((i == 1) || (i >= 17 && i<=26) || (i >= 37 && i <= 48) || (i == 50) || (i == 51)){
                    int32_t* data = (int32_t* )malloc(sizeof(int32_t) *dims);
                    H5Dread(ticker_dataset_id, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

                    if(i==1)
                        compare_int(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_status);
                    else if(i >= 17 && i <= 26){
                        compare_int(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_askvolume[i-17]);
                    }
                    else if(i >= 37 && i <= 48){
                        compare_int(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_bidvolume[i-37]);
                    }
                    else if(i == 50){
                        compare_int(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_totalbidvol);
                    }
                    else if(i == 51){
                        compare_int(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_totalaskvol);
                    }

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

                    if(i >= 2 && i <= 6)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_overall_price[i-2]);
                    else if(i >= 7 && i <= 16){
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_askprice[i-7]);
                    }
                    else if(i >= 27 && i <= 36)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_bidprice[i-27]);
                    else if(i == 49)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].turnover);
                    else if(i == 52)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_WeightedAvgBidPrice);
                    else if(i == 53)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_WeightedAvgAskPrice);
                    else if(i == 54)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_HighLimit);
                    else if(i == 55)
                        compare_double(data, ticker, spi, dims, map_index,map_xtpdmd[ticker].vec_LowLimit);
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