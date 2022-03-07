/* LogConfig.h
* Configure spdlog::loggers according to config file istream
* 
* Author: Wentao Wu
*/

#pragma once

#include <string>
#include <spdlog/spdlog.h>

void config_log(const std::string& config_file);
