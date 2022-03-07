/* L2XTraderUtils.h
 * 
 * Author: Wentao Wu
*/

#pragma once

#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


inline std::string get_today_str() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream today_oss;
    today_oss << std::put_time(&tm, "%Y%m%d");
    std::string today_str = today_oss.str();
    return today_str;
}

inline std::string get_time_str() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream time_oss;
    time_oss << std::put_time(&tm, "%H%M%S");
    std::string time_str = time_oss.str();
    return time_str;
}

// create folder if the folder not exist
// returns:
//   0: if the folder has already been exist
//   1: if the folder is created
// throw:
//   runtime_error: if not have permission or need recursive create
inline int mkdir_if_not_exist(const std::string& folder) {
    mode_t folder_mode =  S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH; // 0773
    if(mkdir(folder.c_str(), folder_mode) == -1) { 
        if(errno == EEXIST) {
            return 0;
        } else {
            throw std::runtime_error("Create folder " + folder + " failed: " + std::string(strerror(errno)));
        }
    } 
    return 1;
}

inline void check_file_exist(std::string const& name) {
    struct stat buffer;
    bool is_exist = stat(name.c_str(), &buffer) == 0 ;
    if(!is_exist) {
        throw std::invalid_argument("File " + name + " does not exist");
    }
}
