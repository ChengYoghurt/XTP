/* UpperLimitHitter/main.cpp
 * 
 * Author: Wentao Wu
*/

#include "Kuafu.h"

#include "KuafuVersion.h"
#include "KuafuUtils.h"
#include "LogConfig.h"
#include "TypeDefs.h"
#include "YAMLGetField.h"

#include <ctime>
#include <errno.h>
#include <exception>
#include <iostream>
#include <iterator>
#include <fstream>
#include <future>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

std::atomic<bool> quit_flag = false;

static void sig_int(int signo) {
    if (signo == SIGINT) {
        quit_flag = true;
    }
}

void usage(const char* call_name) {
    std::cerr << "Kuafu: " << KUAFU_VERSION << std::endl;
    std::cerr << "Usage: " << call_name << " \\ \n"
              << "       [-f configure_file] \\ \n";
}

int main(int argc, char* argv[]) {
    // register signal function 
    struct sigaction act;
    struct sigaction oact;
    act.sa_handler = sig_int;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_RESTART;
    if (sigaction(SIGINT, &act, &oact) < 0) {
        std::cerr << "signal(SIGINT) error" << std::endl;
        return -1;
    }

    std::string levelII_folder;
    std::string config_file {"Config/KuafuConfig.yaml"};
    std::string today_str{ get_today_str() };
    int opt;
    while((opt = getopt(argc, argv, "f:d:h")) != -1) {
        switch(opt) {
        case 'd':
            today_str = optarg;
            break;
        case 'f':
            config_file = optarg;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        case ':':
            std::cerr << "Error: option -" << static_cast<char>(optopt) << " needs a value, use default value." << std::endl;
            break;
        case '?':
            std::cerr << "Error: unknown option -" << static_cast<char>(optopt) << ", ignore option." << std::endl;
            break;
        default:
            // do nothing
            return -1;
        }
    }
    if (optind < argc) {
        // direct pass file name without -f
        config_file = argv[optind];
    }

    //=============================================================//
    //                     +. Load YAML configure                  //
    //=============================================================//
    kf::timestamp_t start_time = kf::timestamp_t::now();

    // Default values
    bool replay_local_market = false;
    std::string tdf_config_file;
    std::string tdf_account_name;
    bool tdf_replay_from_start = false;
    std::string local_market_levelII;
    int replay_idle = 0;
    std::string replay_stock_list_file;
    std::vector<kf::instrument_id_t> replay_stock_list;

    // tdf account config
    std::string tdf_server_ip;
    int tdf_server_port      ;
    std::string tdf_username ;
    std::string tdf_password ;

    // risk config
    kf::price_t account_threshold;
    uint32_t    count_threshold  ;
    uint32_t    count_duration   ;

    check_file_exist(config_file);
    YAML::Node config = YAML::LoadFile(config_file);

    YAML_GET_FIELD(replay_local_market, config, ReplayLocalMarket);
    if(replay_local_market) {
        // Local Market Config
        YAML_GET_FIELD(local_market_levelII  , config, LocalMarketLevelII);
        YAML_GET_FIELD(replay_idle           , config, ReplayIdle        );
        YAML_GET_FIELD(replay_stock_list_file, config, ReplayStockList   );

        if(replay_stock_list_file != "all" && replay_stock_list_file != "All" && !replay_stock_list_file.empty()) {
            check_file_exist(replay_stock_list_file);
        }
    } else {
        // TDF Config
        YAML_GET_FIELD(tdf_config_file      , config, TDFConfig         );
        YAML_GET_FIELD(tdf_account_name     , config, TDFAccount        );
        YAML_GET_FIELD(tdf_replay_from_start, config, TDFReplayFromStart);

        // config TDF account
        check_file_exist(tdf_config_file);
        YAML::Node tdf_config     = YAML::LoadFile(tdf_config_file);
        YAML::Node tdf_account    = tdf_config[tdf_account_name];
        if(!tdf_account) {
            std::cerr << fmt::format("Account {} is not in TdfConfig file {}\n", tdf_account_name, tdf_config_file);
            return -1;
        }
        YAML_GET_FIELD(tdf_server_ip  , tdf_account, server_ip  );
        YAML_GET_FIELD(tdf_server_port, tdf_account, server_port);
        YAML_GET_FIELD(tdf_username   , tdf_account, username   );
        YAML_GET_FIELD(tdf_password   , tdf_account, password   );
    }

    // Read Config
    std::string log_config_file        ; YAML_GET_FIELD(log_config_file        , config, LogConfig       );
    std::string calendar_file          ; YAML_GET_FIELD(calendar_file          , config, Calendar        );
    std::string all_stock_pool_file    ; YAML_GET_FIELD(all_stock_pool_file    , config, StockUniverse   );

    check_file_exist(log_config_file);
    config_log(log_config_file);
    auto p_logger = spdlog::get("main");

    // show all parameters
    p_logger->info("Parameters Summary:");
    p_logger->info("TradingDay           = {}", today_str                 );
    p_logger->info("KuafuVersion         = {}", KUAFU_VERSION             );
    p_logger->info("KuafuUpdate          = {}", KUAFU_VERSION_MESSAGE     );

    if(!replay_local_market) {
    p_logger->info("[[ TDFConfig ]]");
    p_logger->info("TDFAccount           = {}", tdf_username              );
    p_logger->info("TDFServer            = {}", tdf_server_ip             );
    p_logger->info("TDFPort              = {}", tdf_server_port           );
    } else {
    p_logger->info("[[ LocalFakeMarket ]]");
    p_logger->info("LevelDataFolder      = {}", local_market_levelII      );
    p_logger->info("ReplayIdleMS         = {}", replay_idle               );
    p_logger->info("ReplayListLength     = {}", replay_stock_list.size()  );
    }

    p_logger->info("[[ MainConfig ]]");
    p_logger->info("LogConfig            = {}", log_config_file           );
    p_logger->info("StockUniverse        = {}", all_stock_pool_file       );

    //=============================================================//
    //                      +. Init DataBase                       //
    //=============================================================//
    std::vector<kf::instrument_id_t> sub_instruments;
    std::pair<std::size_t, std::size_t> num_stocks_in_exchange {0, 0}; // (num in SZ, num in SH)
    try {
        // num_stocks_in_exchange = load_stock_pool_to_vector(all_stock_pool_file, sub_instruments);
    } catch(std::invalid_argument& e) {
        p_logger->error(e.what());
        p_logger->error("load stock pool file {} failed, please upload pool file with correct format", all_stock_pool_file);
        return -1;
    }
    p_logger->info("Load {} instruments from stock pool", sub_instruments.size());
    p_logger->info("{} instruments in SZ exchange; {} instruments in SH exchange", 
        num_stocks_in_exchange.first, num_stocks_in_exchange.second
    );

    //=============================================================//
    //                      +. Data Segment 
    //=============================================================//

    //=============================================================//
    //                      +. start market                        //
    //=============================================================//

    //=============================================================//
    //                    +. Wait to Quit                          //
    //=============================================================//
    // wait for SIGINT to continue
    p_logger->info("Start Working and wait SIGINT to stop");
    sigset_t zeromask;
    sigemptyset(&zeromask);
    while(quit_flag == 0) {
        sigsuspend(&zeromask);
    }

    std::cout << std::endl;
    std::cerr << std::endl;
    p_logger->warn("Get SIGINT");

    //=============================================================//
    //                    +. Save Stream Data                      //
    //=============================================================//
    p_logger->info("dumping data to disk...");

    p_logger->info("Stop Market spi");

    p_logger->info("All Done!");

    return 0;
}
