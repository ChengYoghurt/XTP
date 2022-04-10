/* WCTrader.h
 * Implement target position with TraderX API/SPI
 * 
 * Author: Wentao Wu
*/
#pragma once

#include "WCApi.h"

#include "OrderPack.h"
#include "TraderTypes.h"
#include "WCResponseHandler.h"

#include <iostream>
#include <atomic>
#include <string>
#include <memory>
#include <variant>
#include "concurrentqueue/concurrentqueue.h"
#include "spdlog/spdlog.h"

namespace wct {

class ControlSignals        ;
class OrderKeeper           ;
class RiskController        ;
class RiskControllerCreator ;
class WCCallback            ;
class WCTrader              ;
class WCTraderConfig        ;

using TraderMessage = std::variant<
    WCEmptyResponse    ,
    WCOrderResponse    , 
    WCTradeResponse    , 
    WCPositionResponse ,
    WCBalanceResponse  ,
    WCLoginResponse    
>;
using ResponseQueue = moodycamel::ConcurrentQueue<TraderMessage>;

class WCCallback: public api::WCSpi {
public:
    WCCallback(uint16_t trader_id, wct::ResponseQueue& queue);
    void on_login(WCLoginResponse const& response) ;
    void on_logout() ;
    void on_connected() ;
    void on_disconnected(error_id_t error_id) ;
    void on_order_event(WCOrderResponse const& response) ;
    void on_trade_event(WCTradeResponse const& response) ;
    void on_cancel_rejected(WCCancelRejectedResponse const& response) ;
    void on_query_position(WCPositionResponse const& response) ;
    void on_query_balance(WCBalanceResponse const& response) ;
    void on_query_credit_balance(WCCreditBalanceResponse const& response) ;
private:
    uint16_t trade_id_;
    wct::ResponseQueue& response_queue_;
    std::shared_ptr<spdlog::logger> p_logger_;
};

struct WCTraderConfig {
    uint32_t    trade_id           ;  // self assigned global unique id for each trader
    int32_t     trade_unit         ;
    std::string server_socket      ;
    std::string tradex_logger_name ;
    std::string tradex_algo_params ;
};

class WCTrader {
public:
    WCTrader(
        std::unique_ptr<WCTraderConfig> p_config, 
        std::unique_ptr<api::WCApi> p_trader_api,
        std::unique_ptr<RiskControllerCreator> p_risk_controller_creator = nullptr
    );
    ~WCTrader() ;

    // account init
    void init_account_avail(price_t account_avail) ;

    // run() runs on seperated thread
    void run()  ; 
    void stop() ; 

    // login() must be called before order operations
    // block until login reponse received 
    void login(WCLoginRequest const& request) ;

    // API: place orders:
    // 1. place order with risk control and delay service, only last order id is returned if splitted
    order_id_t place_order(
        instrument_id_t stock, 
        side_t side, 
        volume_t vol, 
        price_t limit_price, 
        millisec_t expire_ms = 0
    );
    // 2. directly place order without risk and delay service
    void execute_place_order(
        order_id_t local_order_id, 
        instrument_id_t id, 
        side_t side, 
        volume_t vol, 
        price_t limit_price
    );

    // API: place basket order
    // 1. add legs to basket before place_algo_basket
    void add_algo_leg(instrument_id_t instrument, volume_t volume, side_t side) ;
    // 2. place basket, returns when all order are completed
    order_id_t place_algo_basket(
        timestamp_t end_time,
        timestamp_t start_time = timestamp_t::now()
    );
    // 3. if some order is stuck, call stop_wait_order to stop waiting and clear all cached legs
    void manual_stop_waiting_orders() ;
    // 4. print incomplete order
    void print_incomplete_orders() const ;

    // API: cancel orders
    // 1. cancel all sibling order of last_order_id
    void cancel_order(order_id_t last_order_id);
    // 2. directly only cancel given order_id
    void execute_cancel_order(order_id_t local_order_id);

    // query current holding of given instrument
    const HoldingInfo& query_holdings(instrument_id_t instrument) ;
    // query positions of all current holdings 
    const PositionInfo& query_holdings() ;
    // query balance
    const BalanceInfo& query_balance_from_account();
    const BalanceInfo& query_balance_from_broker();

    std::ostream& dump_log(std::ostream& os) ;

private:

    void user_login(WCLoginRequest const& request) ;

    // place OrderPack
    order_id_t place_order(OrderPack const& order_pack, OrderID parent_order_id = 0);
    order_id_t place_order(OrderAction const& order_action, OrderID child_order_id = 0);
    void execute_order(OrderPack const& order_pack, OrderID parent_order_id = 0);

    void create_order_log(
        order_id_t local_order_id, 
        instrument_id_t id, 
        side_t side, 
        volume_t vol, 
        price_t limit_price
    );

    // cancel OrderPack
    void cancel_order(OrderPack const& order_pack);
    void cancel_order(OrderAction const& order_action);

    void post_risk_service() ;
    void delay_service() ;

    request_id_t get_request_id() {
        // request encoding
        static request_id_t s_request_id_ = 1;
        // increasing from 1
        return s_request_id_++;
    }

private:
    std::atomic_bool stop_;
    ResponseQueue response_queue_;

    std::unique_ptr<WCTraderConfig   > p_config_          ;
    std::shared_ptr<ControlSignals   > p_control_signals_ ;
    std::shared_ptr<OrderKeeper      > p_order_keeper_    ;
    std::unique_ptr<RiskController   > p_risk_controller_ ;

    std::unique_ptr<api::WCApi       > p_trader_api_      ;
    std::unique_ptr<WCResponseHandler> p_response_handler_;

    std::shared_ptr<spdlog::logger   > p_logger_          ;
};

} /* namespace wct */
