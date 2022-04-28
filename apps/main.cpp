#include "Kuafu.h"

#include "KuafuVersion.h"
#include "KuafuUtils.h"
#include "LogConfig.h"
#include "TraderTypeDefs.h"
#include "YAMLGetField.h"
#include "WCTrader/WCApi.h"
#include "AdaptedApi.h"
#include "AdaptedTypes.h"
#include "WCTrader/TraderTypes.h"
#include "WCTrader/WCTrader.h"

#include <vector>
#include <cstdio>
#include <string>
#include <iostream>
#include <iterator>
#include <fstream>
#include <signal.h>
#include <thread>



std::atomic<bool> quit_flag = false;

static void sig_int(int signo) {
    if (signo == SIGINT) {
        quit_flag = true;
    }
}


void usage(const char* call_name) {
    std::cerr << "KUAFU Version: " << KUAFU_VERSION << std::endl;
    std::cerr << "Usage: " << call_name << " \\ \n"
              << "       [-f configure_file] \\ \n";
}

int main(int argc,char* argv[]) {

    std::string config_file { "Config/KuafuConfig.yaml"};
    std::string today_str { get_today_str() };
    int opt;
    while((opt = getopt(argc, argv, "f:h")) != -1) {
        switch(opt) {
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
    std::string trade_config_file;
    std::string trade_account_name;
    
    // trade account config
    std::string trade_server_ip;
    int trade_server_port      ;
    std::string trade_username ;
    std::string trade_password ;
    std::string filepath; 
    int log_level;
    // trade account config addtional 
    uint32_t client_id;
    uint32_t heat_beat_interval;
    uint32_t trade_protocol;

    bool use_yaml = true;

    check_file_exist(config_file);
    YAML::Node config = YAML::LoadFile(config_file);

    // trade Config
    YAML_GET_FIELD(trade_config_file      , config, TradeConfig         );
    YAML_GET_FIELD(trade_account_name     , config, TradeAccount        );
    // config trade account
    check_file_exist(trade_config_file);
    YAML::Node trade_config     = YAML::LoadFile(trade_config_file);
    YAML::Node trade_account    = trade_config[trade_account_name];
    if(!trade_account) {
        std::cerr << fmt::format("Account {} is not in tradeConfig file {}\n", trade_account_name, trade_config_file);
        return -1;
    }
    YAML_GET_FIELD(trade_server_ip  , trade_account, server_ip  );
    YAML_GET_FIELD(trade_server_port, trade_account, server_port);
    YAML_GET_FIELD(trade_username   , trade_account, username   );
    YAML_GET_FIELD(trade_password   , trade_account, password   );
    YAML_GET_FIELD(filepath         , trade_account, path       );
    // yaml 初始化 additional
    YAML_GET_FIELD(client_id         , trade_account, client_id        );   
    YAML_GET_FIELD(heat_beat_interval, trade_account, hb_interval      );

 //   YAML_GET_FIELD(trade_exchange_sh      , trade_account, exchange_id_sh       );
 //   YAML_GET_FIELD(trade_exchange_sz      , trade_account, exchange_id_sz       );

/*
    if (use_yaml) {
        YAML::Node node_instruments   = trade_account["instrument_sh"];
        instrument_count_sh         = node_instruments.size();

        for (int i = 0 ; i < instrument_count_sh ; i++ ) {
            std::string temp_instrument   = node_instruments[i].as<std::string>();
            vec_instruments_sh.push_back(temp_instrument);
        }
            
        node_instruments   = trade_account["instrument_sz"];
        instrument_count_sz         = node_instruments.size();
        
        for (int i = 0 ; i < instrument_count_sz ; i++ ) {
            std::string temp_instrument   = node_instruments[i].as<std::string>();
            vec_instruments_sz.push_back(temp_instrument);
        }

    } 
*/
    //not clear if yamlgetfield can convert node to other types like side_t
    //order config
    YAML::Node node_orders  ;// = trade_account["order"];        
    uint32_t order_count     = node_orders.size();
    std::vector<wct::WCOrderRequest> vec_wcorderrequest;

    /*for (uint32_t i = 0 ; i < order_count ; i++) {
        wct::WCOrderRequest wcorderrequest;
        wcorderrequest.instrument       = node_orders[i]["instrument_id"].as<wct::instrument_id_t>();
        wcorderrequest.client_order_id  = wct::order_id_t(node_orders[i]["client_order_id"].as<uint32_t>());
        wcorderrequest.market           = (wct::market_t)get_belonged_market(wcorderrequest.instrument);
        wcorderrequest.price            = node_orders[i]["price"].as<wct::price_t>();
        wcorderrequest.volume           = node_orders[i]["volume"].as<wct::volume_t>();
        wcorderrequest.side             = (wct::side_t)node_orders[i]["side"].as<uint32_t>();
        wcorderrequest.price_type       = (wct::price_type_t)node_orders[i]["price_type"].as<uint32_t>();
        vec_wcorderrequest.emplace_back(wcorderrequest);
    }*/
    //concel order config
    YAML::Node node_cancel_orders  ;// = trade_account["cancel_order_id"];        ;
    uint32_t cancel_order_count     = node_cancel_orders.size();;
    std::vector<wct::WCOrderCancelRequest> vec_wccancelreq;

    for (uint32_t i = 0 ; i < cancel_order_count ; i++) {
        wct::WCOrderCancelRequest wccancelreq;
        wccancelreq.client_order_id = wct::order_id_t(node_cancel_orders[i].as<uint32_t>());//? not certain
        vec_wccancelreq.emplace_back(wccancelreq);
    }
    //qurry config
    bool query_balance_is_true_account;
    bool query_balance_is_true_broker;
    bool query_position_is_true;
    bool query_position_is_all;
    wct::instrument_id_t query_position_instrument; 

    YAML_GET_FIELD(query_balance_is_true_account,     trade_account, query_balance_is_true_account    );
    YAML_GET_FIELD(query_balance_is_true_broker,      trade_account, query_balance_is_true_broker     );
    YAML_GET_FIELD(query_position_is_true,            trade_account, query_position_is_true           );
    YAML_GET_FIELD(query_position_is_all,             trade_account, query_position_is_all            );
    YAML_GET_FIELD(query_position_instrument,         trade_account, query_position_instrument        );
    
    // Read Config
    std::string log_config_file        ; YAML_GET_FIELD(log_config_file        , config, LogConfig       );
    std::string calendar_file          ; YAML_GET_FIELD(calendar_file          , config, Calendar        );
    std::string all_stock_pool_file    ; YAML_GET_FIELD(all_stock_pool_file    , config, StockUniverse   );
    std::string query_data             ; YAML_GET_FIELD(query_data             , config, Query_data      );

    check_file_exist(log_config_file);
    config_log(log_config_file);
    auto p_logger = spdlog::get("main");

    // show all parameters
    p_logger->info("Parameters Summary:");
    p_logger->info("TradingDay           = {}", today_str                 );
    p_logger->info("KuafuVersion         = {}", KUAFU_VERSION             );
    p_logger->info("KuafuUpdate          = {}", KUAFU_VERSION_MESSAGE     );

    p_logger->info("[[ tradeConfig ]]");
    p_logger->info("tradeAccount           = {}", trade_username              );
    p_logger->info("tradeServer            = {}", trade_server_ip             );
    p_logger->info("tradePort              = {}", trade_server_port           );
    p_logger->info("LogConfig              = {}", log_config_file             );

    auto p_adapted_api = std::make_unique<wct::api::AdaptedApi>(client_id, filepath);
    auto p_wc_trader_config = std::make_unique<wct::WCTraderConfig>();
    
    wct::WCTrader wc_trader(
        std::move(p_wc_trader_config), 
        std::move(p_adapted_api)
    ); 

    std::thread wc_trader_th = std::thread(&wct::WCTrader::run, &wc_trader);

    wct::WCLoginRequest wcloginrequest     ;
    wct::HoldingInfo holdingofgiveninstr   ;
    wct::BalanceInfo balanceinfo           ;
    wct::PositionInfo positioninfo         ;

    wcloginrequest.username                     = trade_username      ;
    wcloginrequest.password                     = trade_password      ;
    wcloginrequest.server_ip                    = trade_server_ip     ;
    wcloginrequest.server_port                  = trade_server_port   ;
    wcloginrequest.agent_fingerprint.local_ip   = "192.168.0.204"     ; //? not certain
    //std::cout<<trade_username<<std::endl<<trade_password<<std::endl<<trade_server_ip<<std::endl<<trade_server_port<<std::endl;
    wcloginrequest.agent_fingerprint.token      = "b8aa7173bba3470e390d787219b2112e";
    wc_trader.login(wcloginrequest);
    std::cout << "--------------Login successfully----------------" << std::endl;
    //wct::WCLoginResponse response; 
    //response.session_id = p_adapted_api->get_session_id();
    //response.error_id = wct::error_id_t::success;
    //wc_trader.on_login(response);////////////////////////////////
    wct::price_t account_avail = 5000000.0; 
    wc_trader.init_account_avail(account_avail); 
    std::ofstream querylog;
    querylog.open(query_data, std::ios::app);

    std::vector<wct::order_id_t> vec_orderid;

    for (uint32_t i = 0 ; i < order_count ; i++) {
        wct::order_id_t local_order_id;
        wct::instrument_id_t stock  = vec_wcorderrequest[i].instrument  ;
        wct::side_t side            = vec_wcorderrequest[i].side        ;
        wct::volume_t vol           = vec_wcorderrequest[i].volume      ;
        wct::price_t limit_price    = vec_wcorderrequest[i].price       ;
        wct::millisec_t expire_ms   = 100                               ;
        local_order_id = wc_trader.place_order(stock, side, vol, limit_price, expire_ms);
        vec_orderid.push_back(local_order_id);
    }

        /*for (uint32_t i = 0 ; i < order_count ; i++) {
            wct::instrument_id_t stock     = vec_wcorderrequest[i].instrument  ;
            wct::side_t side            = vec_wcorderrequest[i].side        ;
            wct::volume_t vol           = vec_wcorderrequest[i].volume      ;
            wct::price_t limit_price    = vec_wcorderrequest[i].price       ;
            wc_trader.execute_place_order(local_order_id, stock, side, vol, limit_price);
        }*/

    for (size_t i = 0 ; i < vec_orderid.size() ; i++) {
        wct::order_id_t last_order_id = vec_orderid[i];
        wc_trader.cancel_order(last_order_id);
    }

    for (size_t i = 0 ; i < vec_orderid.size() ; i++) {
        wct::order_id_t last_order_id = vec_orderid[i];
        wc_trader.execute_cancel_order(last_order_id);
    }

    /*if (query_position_is_true) {
        holdingofgiveninstr = wc_trader.query_holdings(query_position_instrument);
        querylog << "query_position: " << query_position_instrument << std::endl;
        querylog << "holding: "        << holdingofgiveninstr.holding
                 << "available: "      << holdingofgiveninstr.available
                 << std::endl;
    }*/

    /*if (query_position_is_all) {
        positioninfo = wc_trader.query_holdings();
    }*/

    if (query_balance_is_true_account) {
        balanceinfo = wc_trader.query_balance_from_account();
        querylog << "query_balance_account" <<std::endl; 
        querylog << "initial_balance: "     << balanceinfo.initial_balance
                 << "available_balance: "   << balanceinfo.available_balance
                 << "market_value: "        << balanceinfo.market_value
                 << "total_asset: "         << balanceinfo.total_asset
                 << std::endl;
    }

    if (query_balance_is_true_broker) {
        balanceinfo = wc_trader.query_balance_from_broker();
        querylog << "query_balance_account" <<std::endl; 
        querylog << "initial_balance: "     << balanceinfo.initial_balance
                 << "available_balance: "   << balanceinfo.available_balance
                 << "market_value: "        << balanceinfo.market_value
                 << "total_asset: "         << balanceinfo.total_asset
                 << std::endl;
    }
        
    querylog.close();


    sigset_t zeromask;
    while(quit_flag == 0) {
        sigsuspend(&zeromask);
    }
    wc_trader.stop();
    wc_trader_th.join();

    std::string dumplogpath;
    YAML_GET_FIELD(dumplogpath, config, Dump_log_output);
    dumplogpath = dumplogpath + get_time_str() + ".log";
    std::ofstream dumplogfile;
    dumplogfile.open(dumplogpath, std::ios::app);
    wc_trader.dump_log(dumplogfile);
    dumplogfile.close();

    return 0;
}
