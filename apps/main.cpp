/* UpperLimitHitter/main.cpp
 * 
 * Author: Wentao Wu
*/

#include "Kuafu.h"

#include "KuafuVersion.h"
#include "KuafuUtils.h"
#include "LogConfig.h"
#include "TraderTypeDefs.h"
#include "YAMLGetField.h"
#include "xtp_quote_api.h"
#include "quote_spi.h"
#include "xquote_api_struct.h"
#include "xtp_trader_api.h"
#include "H5diff.h"

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
#include <vector>
#include <cstdio>



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

int main(int argc, char* argv[]){
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
    std::string today_str{ kf::get_today_str() };
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

    // tdf account config addtional 
    std::string filepath; 
    uint32_t client_id;
    uint32_t tdf_exchange_sh;
    uint32_t tdf_exchange_sz;
    std::string query_path_sh;
    std::string query_path_sz;
    uint32_t heat_beat_interval;
    uint32_t quote_buffer_size;
    uint32_t quote_protocol;
    uint32_t instrument_count_sh;
    uint32_t instrument_count_sz;
    std::vector<std::string> vec_instruments_sh;
    std::vector<std::string> vec_instruments_sz;
    // risk config
    kf::price_t account_threshold;
    uint32_t    count_threshold  ;
    uint32_t    count_duration   ;

    bool use_yaml = true;

    kf::check_file_exist(config_file);
    YAML::Node config = YAML::LoadFile(config_file);

    YAML_GET_FIELD(replay_local_market, config, ReplayLocalMarket);
    if(replay_local_market) {
        // Local Market Config
        YAML_GET_FIELD(local_market_levelII  , config, LocalMarketLevelII);
        YAML_GET_FIELD(replay_idle           , config, ReplayIdle        );
        YAML_GET_FIELD(replay_stock_list_file, config, ReplayStockList   );

        if(replay_stock_list_file != "all" && replay_stock_list_file != "All" && !replay_stock_list_file.empty()) {
            kf::check_file_exist(replay_stock_list_file);
        }
    } else {
        // TDF Config
        YAML_GET_FIELD(tdf_config_file      , config, TDFConfig         );
        YAML_GET_FIELD(tdf_account_name     , config, TDFAccount        );
        YAML_GET_FIELD(tdf_replay_from_start, config, TDFReplayFromStart);

        // config TDF account
        kf::check_file_exist(tdf_config_file);
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
        // yaml 初始化 additional
        YAML_GET_FIELD(client_id         , tdf_account, client_id        );
        YAML_GET_FIELD(filepath          , tdf_account, path             );
        
        YAML_GET_FIELD(heat_beat_interval, tdf_account, hb_interval      );
        YAML_GET_FIELD(quote_buffer_size , tdf_account, quote_buffer_size);
        YAML_GET_FIELD(quote_protocol    , tdf_account, quote_protocol   );  

        YAML_GET_FIELD(tdf_exchange_sh      , tdf_account, exchange_id_sh       );
        YAML_GET_FIELD(tdf_exchange_sz      , tdf_account, exchange_id_sz       );
        YAML_GET_FIELD(query_path_sh     , tdf_account, query_tickers_path_sh);
        YAML_GET_FIELD(query_path_sz     , tdf_account, query_tickers_path_sz);

        if (use_yaml) {
            YAML::Node node_instruments   = tdf_account["instrument_sh"];
            instrument_count_sh         = node_instruments.size();
            node_instruments   = tdf_account["instrument_sz"];
            instrument_count_sz         = node_instruments.size();
        
            for (int i = 0 ; i < instrument_count_sh ; i++ ) {
                std::string temp_instrument   = node_instruments[i].as<std::string>();
                vec_instruments_sh.push_back(temp_instrument);
            }
            
            for (int i = 0 ; i < instrument_count_sz ; i++ ) {
                std::string temp_instrument   = node_instruments[i].as<std::string>();
                vec_instruments_sz.push_back(temp_instrument);
            }

        } 
    }

    // Read Config
    std::string log_config_file        ; YAML_GET_FIELD(log_config_file        , config, LogConfig       );
    std::string calendar_file          ; YAML_GET_FIELD(calendar_file          , config, Calendar        );
    std::string all_stock_pool_file    ; YAML_GET_FIELD(all_stock_pool_file    , config, StockUniverse   );

    kf::check_file_exist(log_config_file);
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
    
    p_logger->info("filepath             = {}", filepath);
    // p_logger->info("instrument_count     = {}", instrument_count_sh + instrument_count_sz);

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

    // Initiate quote api
	XTP::API::QuoteApi* pquoteapi = XTP::API::QuoteApi::CreateQuoteApi(client_id, filepath.c_str(), XTP_LOG_LEVEL_DEBUG);//log日志级别可以调整
    MyQuoteSpi* pquotespi = new MyQuoteSpi();
	pquoteapi->RegisterSpi(pquotespi);
	// Set timeout (second)
	pquoteapi->SetHeartBeatInterval(heat_beat_interval);
	// Set local quote cache size (MB)
	pquoteapi->SetUDPBufferSize(quote_buffer_size);

	int login_result_quote = -1;
	login_result_quote = pquoteapi->Login(tdf_server_ip.c_str(), tdf_server_port, tdf_username.c_str(), tdf_password.c_str(), (XTP_PROTOCOL_TYPE)quote_protocol); 
	if (login_result_quote == 0)
	{
        std::cout << "--------------Login successfully----------------" << std::endl;
        std::mutex mutex;
        pquotespi->processed_sh = false;
	    pquotespi->processed_sz = false;

		std::remove(query_path_sh.c_str());
        std::remove(query_path_sz.c_str());
        
        pquoteapi->QueryAllTickers(XTP_EXCHANGE_SH);
        std::cout << "Querying_SH" << std::endl;
        //Main thread waits here 
        {
            std::unique_lock lk(mutex);
            if(pquotespi->cv_last.wait_for(lk, std::chrono::seconds(3), [&pquotespi]{return pquotespi->processed_sh;})) {
                pquotespi->print_ticker_info(XTP_EXCHANGE_SH, query_path_sh);
            }

        }
        pquoteapi->QueryAllTickers(XTP_EXCHANGE_SZ);
        std::cout << "Querying_SZ" << std::endl;
        //Main thread waits here 
        {
            std::unique_lock lk(mutex);
            if(pquotespi->cv_last.wait_for(lk, std::chrono::seconds(3), [&pquotespi]{return pquotespi->processed_sz;})) {
                pquotespi->print_ticker_info(XTP_EXCHANGE_SZ, query_path_sz);
            }

        }
    
        //Store tickers to vec
        if (!use_yaml) {
            std::ifstream query_ticker_sh_infile(query_path_sh);
            std::ifstream query_ticker_sz_infile(query_path_sz);
            bool has_ticker_input = false;
            if (query_ticker_sh_infile) {
                // std::cout << "SH FINE" << std::endl;
                has_ticker_input = true;
                std::string ticker_sh;
                while (getline(query_ticker_sh_infile, ticker_sh)) {
                    vec_instruments_sh.push_back(ticker_sh);
                }
                query_ticker_sh_infile.close();
                instrument_count_sh = vec_instruments_sh.size();
                std::cout << "SH SIZE" << instrument_count_sh << std::endl;
            }

            if (query_ticker_sz_infile) {
                // std::cout << "SZ FINE" << std::endl;
                has_ticker_input = true;
                std::string ticker_sz;
                while (getline(query_ticker_sz_infile, ticker_sz)) {
                    vec_instruments_sz.push_back(ticker_sz);
                }
                query_ticker_sz_infile.close();
                instrument_count_sz = vec_instruments_sz.size();
                std::cout << "SZ SIZE" << instrument_count_sz << std::endl;
            }

            if (has_ticker_input == false) {
                std::cout << "--------------No ticker input--------------" << std::endl;
                return 0;
            }

        }

		int quote_exchange = tdf_exchange_sh;

		// Read from yaml
		char* *allInstruments = new char*[instrument_count_sh];
		for (int i = 0; i < instrument_count_sh; i++) {
			allInstruments[i] = new char[7];
			std::string instrument =vec_instruments_sh[i] ;
			strcpy(allInstruments[i], instrument.c_str());
		}

		pquoteapi->SubscribeMarketData(allInstruments, instrument_count_sh, (XTP_EXCHANGE_TYPE)quote_exchange);
		pquoteapi->SubscribeTickByTick(allInstruments, instrument_count_sh, (XTP_EXCHANGE_TYPE)quote_exchange);

        for (int i = 0; i < instrument_count_sh; i++) {
		    delete[] allInstruments[i];
			allInstruments[i] = NULL;
		}

        allInstruments = new char*[instrument_count_sz];
		for (int i = 0; i < instrument_count_sz; i++) {
			allInstruments[i] = new char[7];
			std::string instrument =vec_instruments_sz[i] ;
			strcpy(allInstruments[i], instrument.c_str());
		}
		
        pquoteapi->SubscribeMarketData(allInstruments, instrument_count_sz, (XTP_EXCHANGE_TYPE)tdf_exchange_sh);
		pquoteapi->SubscribeTickByTick(allInstruments, instrument_count_sz, (XTP_EXCHANGE_TYPE)tdf_exchange_sz);

        for (int i = 0; i < instrument_count_sz; i++) {
			delete[] allInstruments[i];
			allInstruments[i] = NULL;
		}

		delete[] allInstruments;
		allInstruments = NULL;

    }

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
    
    //TEST HDF5
    //prepare_diff("depth_market_data.csv", "depth_market.h5");
    //=============================================================//
    //                    +. Save Stream Data                      //
    //=============================================================//

    p_logger->info("dumping data to disk...");
    pquotespi->print_vec_xtpdmet(all_stock_pool_file.c_str());
    p_logger->info("Stop Market spi");
    p_logger->info("All Done!");

    return 0;
}
