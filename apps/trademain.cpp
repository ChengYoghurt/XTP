#include "Kuafu.h"

#include "KuafuVersion.h"
#include "KuafuUtils.h"
#include "LogConfig.h"
#include "TypeDefs.h"
#include "YAMLGetField.h"
#include "WCApi.h"
#include "XTP/AdaptedApi.h"
#include "xtp_trader_api.h"

#include <vector>
#include <cstdio>
#include <string>
#include <iostream>
#include <iterator>
#include <fstream>

int main(int argc,char* argv[]) {

    std::string config_file {"Config/KuafuConfig.yaml"};
    std::string today_str{ get_today_str() };

    //=============================================================//
    //                     +. Load YAML configure                  //
    //=============================================================//
    kf::timestamp_t start_time = kf::timestamp_t::now();

    // Default values
    std::string tdf_config_file;
    std::string tdf_account_name;
    
    // tdf account config
    std::string tdf_server_ip;
    int tdf_server_port      ;
    std::string tdf_username ;
    std::string tdf_password ;
    
    // tdf account config addtional 
    uint32_t client_id;
    uint32_t tdf_exchange_sh;
    uint32_t tdf_exchange_sz;
    std::string query_path_sh;
    std::string query_path_sz;
    uint32_t heat_beat_interval;
    uint32_t trade_protocol;
    uint32_t instrument_count_sh;
    uint32_t instrument_count_sz;
    std::vector<std::string> vec_instruments_sh;
    std::vector<std::string> vec_instruments_sz;

    bool use_yaml = true;

    check_file_exist(config_file);
    YAML::Node config = YAML::LoadFile(config_file);

    // TDF Config
    YAML_GET_FIELD(tdf_config_file      , config, TDFConfig         );
    YAML_GET_FIELD(tdf_account_name     , config, TDFAccount        );
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
    // yaml 初始化 additional
    YAML_GET_FIELD(client_id         , tdf_account, client_id        );
        
    YAML_GET_FIELD(heat_beat_interval, tdf_account, hb_interval      );
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

    p_logger->info("[[ TDFConfig ]]");
    p_logger->info("TDFAccount           = {}", tdf_username              );
    p_logger->info("TDFServer            = {}", tdf_server_ip             );
    p_logger->info("TDFPort              = {}", tdf_server_port           );
    p_logger->info("LogConfig            = {}", log_config_file           );


    //wct::api::WCApi* ptradeapi = wct::api::WCApi
    //XTP::API::TraderApi* ptradeapi = XTP::API::TraderApi::CreateTraderApi(client_id, filepath.c_str(), XTP_LOG_LEVEL_DEBUG);
    wct::api::AdaptedApi* ptradeapi = new wct::api::AdaptedApi();
    wct::api::WCSpi* ptradespi = std::make_unique<wct::api::WCSpi>();

    ptradeapi->register_spi(ptradespi);
    ptradeapi->SetHeartBeatInterval(heat_beat_interval);
    
    uint64_t login_result_trade = -1;
    login_result_trade = ptradeapi->login(tdf_server_ip.c_str(), tdf_server_port, tdf_username.c_str(), tdf_password.c_str(), (XTP_PROTOCOL_TYPE)trade_protocol);
    if(login_result_trade != 0)
    {
        std::cout << "--------------Login successfully----------------" << std::endl;

        ptradeapi->place_order()




    }


}
