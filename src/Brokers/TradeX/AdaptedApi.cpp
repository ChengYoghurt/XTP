/* AdaptedApi.cpp
 * 
 * Author: Wentao Wu
*/

#include "TradeX/AdaptedApi.h"
#include "L2XTraderUtils.h"

#include <cstring>

namespace wct     {
namespace api     {

    //=======================================================
    // Adapted SPI
    //=======================================================
    void AdaptedSpi::OnLogin(const ApiLoginResponse *loginResponse) {
        // check response status
        WCLoginResponse login_rsp;
        login_rsp.error_id = loginResponse->is_success 
                           ? error_id_t::success 
                           : error_id_t::fail_authentication;
        p_logger_->info("Login Error: {}", loginResponse->error_message);
        login_rsp.error_id = error_id_t::fail_authentication;
        login_rsp.session_id = loginResponse->session_id; 
        p_spi_->on_login(login_rsp);
    }
    void AdaptedSpi::OnLogout(const bool isLogoutSuccess, const char *error_message) {
        if(isLogoutSuccess) {
            p_logger_->info("User Logout");
        } else {
            p_logger_->error("User Logout Error,message={}", error_message);
        }
        p_spi_->on_logout();
    }
    void AdaptedSpi::OnConnected(const ApiServerType serverType, const char *url) {
        std::string server_type;
        if(serverType == ApiServerType::Authentification) {
            server_type = "Auth";
        } else if(serverType == ApiServerType::Trading) {
            server_type = "Trading";
        } else if(serverType == ApiServerType::Querying) {
            server_type = "Querying";
        }
        p_logger_->info("Connect to {} Server: {}", server_type, url);
        p_spi_->on_connected();
    }
    void AdaptedSpi::OnDisconnected(const ApiServerType serverType, const char *url, const char *error_message) {
        std::string server_type;
        if(serverType == ApiServerType::Authentification) {
            server_type = "Auth";
        } else if(serverType == ApiServerType::Trading) {
            server_type = "Trading";
        } else if(serverType == ApiServerType::Querying) {
            server_type = "Querying";
        }
        p_logger_->error("Disconnect to {} server,message={}", server_type, url, error_message);
        p_spi_->on_disconnected(error_id_t::not_connected_to_server);
    }

    void AdaptedSpi::OnOrderEvent(const ApiOrderReport *orderReport) {//quanshangfangfa
        if(orderReport->is_child_order) {
            return; // skip all child order
        }
        if(!OrderID(orderReport->client_order_id).is_from_trader(trade_id_)) {
            return;
        }
        WCOrderResponse order_rsp;
        std::memset(&order_rsp, 0, sizeof(order_rsp));
        order_rsp.client_order_id  = orderReport->client_order_id;
        order_rsp.instrument       = std::atoi(orderReport->symbol);
        order_rsp.volume           = orderReport->quantity;
        order_rsp.traded           = orderReport->cum_qty;
        order_rsp.average_price    = orderReport->avg_price;
        order_rsp.order_status     = simplify_status(orderReport->order_status);
        order_rsp.error_id         = error_id_t::unknown;
        order_rsp.transaction_time = orderReport->transact_time % 1'000000'000;  // drop YYYYMMDD and only keep HHMMSSsss
        order_rsp.host_time        = timestamp_t::now();
        p_spi_->on_order_event(order_rsp);
    }
    void AdaptedSpi::OnTradeEvent(const ApiTradeReport *tradeReport) {
        if(!OrderID(tradeReport->client_order_id).is_from_trader(trade_id_)) {
            return;
        }
        WCTradeResponse trade_rsp;
        std::memset(&trade_rsp, 0, sizeof(trade_rsp));
        trade_rsp.client_order_id  = tradeReport->client_order_id;
        trade_rsp.instrument       = std::atoi(tradeReport->symbol);
        trade_rsp.trade_volume     = tradeReport->trade_quantity;
        trade_rsp.trade_price      = tradeReport->trade_price;
        trade_rsp.transaction_time = tradeReport->trade_time % 1'000000'000;
        trade_rsp.host_time        = timestamp_t::now();
        p_spi_->on_trade_event(trade_rsp);
    }
    void AdaptedSpi::OnCancelReject(const ApiOrderCancelReject *cancelReject) {
        if(!OrderID(cancelReject->client_order_id).is_from_trader(trade_id_)) {
            return;
        }
        WCCancelRejectedResponse order_rsp;
        order_rsp.client_order_id  = cancelReject->client_order_id;
        order_rsp.error_id         = error_id_t::unknown;
        p_spi_->on_cancel_rejected(std::move(order_rsp));
    }
    void AdaptedSpi::OnQueryPosition(const ApiPosition *position, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        WCPositionResponse pos_rsp;
        pos_rsp.instrument       = std::atoi(position->symbol);
        pos_rsp.yesterday_volume = position->yesterday_qty;
        pos_rsp.latest_volume    = position->latest_qty;
        pos_rsp.available_volume = position->available_qty;
        pos_rsp.is_last          = is_last; 
        if((position == nullptr) || (is_success == false)) {
            pos_rsp.error_id = error_id_t::unknown;
        } else {
            pos_rsp.error_id = error_id_t::success;
        }
        p_spi_->on_query_position(pos_rsp);
    }
    void AdaptedSpi::OnQueryBalance(const ApiBalance *balance, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        WCBalanceResponse balance_rsp;
        balance_rsp.initial_balance   = balance->initial_balance   ;
        balance_rsp.initial_balance   = balance->initial_balance   ;
        balance_rsp.available_balance = balance->available_balance ;
        balance_rsp.market_value      = balance->market_value      ;
        balance_rsp.total_asset       = balance->total_asset       ;
        if((balance == nullptr) || (is_success == false)) {
            balance_rsp.error_id = error_id_t::unknown;
        } else {
            balance_rsp.error_id = error_id_t::success;
        }
        p_spi_->on_query_balance(balance_rsp);
    }
    void AdaptedSpi::OnQueryOrder(const ApiOrderReport *order, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnQueryTrade(const ApiTradeReport *tradeReport, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnQueryStandardCouponPosition(const ApiPosition *position, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnQueryBorrowingSecurity(const ApiBorrowingSecurity *borrowingSecurity, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnQuerySecurityLiability(const ApiSecurityLiability *securityLiability, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnQueryFinancingLiability(const ApiFinancingLiability *financingLiability, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnQueryCreditAsset(const ApiCreditAsset *creditAsset, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) {
        // not implemented
    }
    void AdaptedSpi::OnTradeUnitStatus(const ApiTradeUnitConnStatusNotice *notice) {
        // not implemented
    }

    //=======================================================
    // Adapted API
    //=======================================================
    AdaptedApi::AdaptedApi() 
        : p_logger_(spdlog::get("AdaptedApi"))
    {
        p_broker_api_ = BrokerApi::CreateTraderApi(".");
    }
    AdaptedApi::~AdaptedApi() {
        p_broker_api_->Release();
    }
    error_id_t AdaptedApi::register_spi(std::unique_ptr<WCSpi> p_spi) {
        p_spi_ = std::make_unique<AdaptedSpi>(std::move(p_spi));
        p_broker_api_->RegisterSpi(p_spi_.get());
        return error_id_t::success;
    }
    std::string AdaptedApi::version() const noexcept {
        return "0.0.0";
    }

    ApiRequestID AdaptedApi::get_request_id() {
        return 0;
    }

    error_id_t AdaptedApi::login(WCLoginRequest const& request) {
        ApiLoginRequest login_req;
        std::memset(&login_req, 0, sizeof(login_req));
        login_req.client_id = std::stoi(request.username);
        std::strncpy(login_req.password  , request.password.c_str()     , sizeof(login_req.password  ));
        std::string login_url = request.server_ip + ":" + std::to_string(request.server_port);
        std::strncpy(login_req.login_url , login_url.c_str(), sizeof(login_req.login_url ));
        std::strncpy(login_req.ip, 
            request.agent_fingerprint.local_ip.c_str(), 
            sizeof(login_req.ip));
        std::strncpy(login_req.mac, 
            request.agent_fingerprint.mac.c_str(), 
            sizeof(login_req.mac));
        std::strncpy(login_req.hard_drive, 
            request.agent_fingerprint.disk.c_str(), 
            sizeof(login_req.hard_drive));
        int ret = p_broker_api_->Login(&login_req);
        return error_id_t::success;
    }
    int AdaptedApi::get_trading_day() {
        return 0;
    }
    error_id_t AdaptedApi::place_order(WCOrderRequest const& request) {
        ApiSingleOrder single_order;
        std::memset(&single_order, 0, sizeof(single_order));
        single_order.trade_unit = trade_unit_;
        single_order.client_order_id = request.client_order_id;
        std::snprintf(single_order.symbol, sizeof(single_order.symbol), "%06d", request.instrument);
        switch (get_belonged_market(request.instrument)) {
            case market_t::sh: 
            case market_t::shsecond: 
                single_order.market = ApiMarket::SH_A; 
                break;
            case market_t::sz: 
            case market_t::szsecond: 
                single_order.market = ApiMarket::SZ_A; 
                break;
            default: single_order.market = ApiMarket::UNKNOWN_MARKET; break;
        }
        single_order.price = request.price;
        single_order.quantity = request.volume;
        single_order.side = (request.side == side_t::buy) ? ApiSide::Buy : ApiSide::Sell;
        single_order.price_type = ApiPriceType::LIMIT; 
        single_order.algo_parameters = nullptr; // not use algorithms trading

        int ret = p_broker_api_->PlaceOrder(&single_order);
        return error_id_t::success;//panduan
    }

    error_id_t AdaptedApi::cancel_order(WCOrderCancelRequest const& request) {
        ApiOrderCancelRequest cancel_order;
        std::memset(&cancel_order, 0, sizeof(cancel_order));
        cancel_order.trade_unit = trade_unit_;
        cancel_order.order_id = request.client_order_id;
        int ret = p_broker_api_->CancelOrder(&cancel_order);
        return error_id_t::success;
    }
    error_id_t AdaptedApi::query_balance() {
        ApiBalanceQueryRequest balance_request;
        std::memset(&balance_request, 0, sizeof(balance_request));
        balance_request.request_id = get_request_id();
        balance_request.trade_unit = trade_unit_;
        int ret = p_broker_api_->QueryBalance(&balance_request);
        return error_id_t::success;
    }
    error_id_t AdaptedApi::query_position(WCPositionQueryRequest const& request) {
        ApiPositionQueryRequest position_request;
        std::memset(&position_request, 0, sizeof(position_request));
        position_request.request_id = get_request_id();
        position_request.trade_unit = trade_unit_;
        if (request.query_all) {
        } else {
            std::snprintf(position_request.symbol, std::strlen(position_request.symbol), "%06d", request.instrument);
        }
        int ret = p_broker_api_->QueryPosition(&position_request);
        return error_id_t::success;
    }
    error_id_t AdaptedApi::query_credit_balance() {
        // not implemented
        return error_id_t::not_implemented;
    }

    error_id_t AdaptedApi::place_basket_order(WCBasketOrderRequest const& request) {
        if(request.basket_legs.empty()) {
            return error_id_t::success;
        }
        ApiBasketOrder basket_order;
        std::memset(&basket_order, 0, sizeof(basket_order));
        basket_order.trade_unit = trade_unit_;
        basket_order.client_basket_id = request.client_basket_id;  // basket_order_ trunc int64_t to int32_t
        basket_order.leg_count = request.basket_legs.size();

        for(int i = 0; i < request.basket_legs.size(); ++i) {
            const WCOrderRequest& item = request.basket_legs[i];
            ApiBasketLeg&    leg  = basket_order.legs[i];
            std::snprintf(leg.symbol, sizeof(leg.symbol), "%06d", item.instrument);
            switch (get_belonged_market(item.instrument))
            {
                case market_t::sh: 
                case market_t::shsecond: 
                    leg.market = ApiMarket::SH_A; 
                    break;
                case market_t::sz: 
                case market_t::szsecond: 
                    leg.market = ApiMarket::SZ_A; 
                    break;
                default: 
                    leg.market = ApiMarket::UNKNOWN_MARKET; 
                    break;
            }
            leg.quantity = item.volume;
            leg.side = item.side == side_t::buy ? ApiSide::Buy : ApiSide::Sell;
            leg.price_type = ApiPriceType::BEST5_OR_CANCEL; // BEST5_OR_CANCEL implies market order for vwap algo
        }
        int ret = p_broker_api_->PlaceBasketOrder(&basket_order);
        return error_id_t::success;
    }
    error_id_t AdaptedApi::cancel_basket_order(WCBasketOrderCancelRequest const& request) {
        WCOrderCancelRequest order_req;
        order_req.client_order_id = request.client_order_id;
        return cancel_order(order_req);
    }

} /* namespace api     */
} /* namespace wct     */