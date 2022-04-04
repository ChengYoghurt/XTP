#include "XTP/AdaptedApi.h"
#include "L2XTraderUtils.h"

#include <cstring>

namespace wct     {
namespace api     {

    void AdaptedSpi::OnDisconnected(uint64_t session_id, int reason) {
        p_logger_->error("Disconnected, session_id = {}, reason = {}", session_id, reason);
        p_spi_->on_disconnected(error_id_t::not_connected_to_server);
    }

    void OnOrderEvent(ApiOrderReport *order_info, ApiText error_info, ApiTradeUnitID session_id) {
       /* if(order_info->is_child_order) {
            return; // skip all child order //?
        }*/
        if(!OrderID(order_info->order_client_id).is_from_trader(trade_id_)) {
            return;
        }
        WCOrderResponse order_rsp;
        std::memset(&order_rsp, 0, sizeof(order_rsp));
        order_rsp.client_order_id  = order_info->order_xtp_id;
        order_rsp.instrument       = std::atoi(order_info->ticker);
        order_rsp.volume           = order_info->quantity;
        order_rsp.price            = order_info->price;
        order_rsp.traded           = order_info->qty_traded;
        order_rsp.average_price    = order_info->trade_amount;//no average but has total
        order_rsp.order_status     = simplify_status(order_info->order_status);
        order_rsp.error_id         = error_id_t::unknown;
        order_rsp.transaction_time = order_info->update_time % 1'000000'000;  // drop YYYYMMDD and only keep HHMMSSsss
        order_rsp.host_time        = timestamp_t::now();
        p_spi_->on_order_event(order_rsp);
    }

    void AdaptedSpi::OnTradeEvent(ApiTradeReport *trade_info, ApiTradeUnitID session_id) {
        if(!OrderID(trade_info->order_client_id).is_from_trader(trade_id_)) {
            return;
        }
        WCTradeResponse trade_rsp;
        std::memset(&trade_rsp, 0, sizeof(trade_rsp));
        trade_rsp.client_order_id  = trade_info->order_xtp_id;
        trade_rsp.instrument       = std::atoi(trade_info->ticker);
        trade_rsp.trade_volume     = trade_info->trade_quantity;
        trade_rsp.trade_price      = trade_info->trade_price;
        trade_rsp.transaction_time = trade_info->trade_time % 1'000000'000;
        trade_rsp.host_time        = timestamp_t::now();
        p_spi_->on_trade_event(trade_rsp);
    }

    void AdaptedSpi::OnCancelOrderError(ApiOrderCancelReject *cancel_info, ApiText *error_info, ApiTradeUnitID session_id) {
      /*  if(!OrderID(cancel_info->client_order_id).is_from_trader(trade_id_)) {
            return;
        }*///no such
        WCCancelRejectedResponse order_rsp;
        order_rsp.client_order_id  = cancel_info->order_xtp_id;
        order_rsp.error_id         = error_id_t::unknown;
        p_spi_->on_cancel_rejected(std::move(order_rsp));
    }

    void AdaptedSpi::OnQueryPosition(ApiPosition *position, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) {
        WCPositionResponse pos_rsp;
        pos_rsp.instrument       = std::atoi(position->ticker)  ;
        pos_rsp.yesterday_volume = position->yesterday_position ;
        pos_rsp.latest_volume    = position->total_qty          ;
        pos_rsp.available_volume = position->sellable_qty       ;
        pos_rsp.is_last          = is_last                      ; 
        if(error_info == nullptr || error_info->error_id == 0) {
            pos_rsp.error_id = error_id_t::success;
        } else {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("OnQueryPosition failed, error_id = {}, error_message = {}",error_info->error_id, error_info->error_msg);
            pos_rsp.error_id = error_id_t::unknown;//////////////////////
        }
        p_spi_->on_query_position(pos_rsp);
    }

    void AdaptedSpi::OnQueryAsset(ApiBalance *asset, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) {
        WCBalanceResponse balance_rsp;
        balance_rsp.initial_balance   = 0                       ;
        balance_rsp.initial_balance   = 0                       ;
        balance_rsp.available_balance = asset->buying_power     ;
        balance_rsp.market_value      = asset->security_asset   ;
        balance_rsp.total_asset       = asset->total_asset      ;
        if(error_info == nullptr || error_info->error_id == 0) {
            pos_rsp.error_id = error_id_t::success;
        } else {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("OnQueryPosition failed, error_id = {}, error_message = {}",error_info->error_id, error_info->error_msg);
            pos_rsp.error_id = error_id_t::unknown;
        }
        p_spi_->on_query_balance(balance_rsp);
    }

    error_id_t AdaptedApi::register_spi(std::unique_ptr<WCSpi> p_spi) {
        p_spi_ = std::make_unique<AdaptedSpi>(std::move(p_spi));
        // RegisterSpi return value is void
        p_broker_api_->RegisterSpi(p_spi_.get());
        return error_id_t::success;
    }
    
    std::string AdaptedApi::version() const noexcept {
        std::string version_no =  p_broker_api_->GetApiVersion();
        return version_no;
    }

    error_id_t AdaptedApi::login(WCLoginRequest const& request) {
        std::string ip              = request.server_ip                 ; 
        int port                    = request.server_port               ;
        std::string user            = request.username                  ;
        std::string password        = request.password                  ;
        XTP_PROTOCOL_TYPE sock_type = XTP_PROTOCOL_TCP                  ;
        std::string local_ip        = request.agent_fingerprint.local_ip;

        session_id_                 = p_broker_api_->Login(ip.c_str(), port, user.c_str(), password.c_str(), sock_type, local_ip.c_str());
        if(session_id_ == 0) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("Login failed, error_id = {}, error_message = {}",error_info->error_id, error_info->error_msg);
            return error_id_t::not_login;
        }
        else {
            WCLoginResponse response; 
            response.session_id = session_id_;
            response.error_id   = 0          ;
            p_spi_->p_spi_->on_login(response);
            return error_id_t::success;
        }
    }

    ApiRequestID AdaptedApi::get_request_id() {
        return ++request_id_;
    }

    int AdaptedApi::get_trading_day() {
        std::string trading_day_str = p_broker_api_->GetTradingDay();
        return std::stoi(trading_day_str);
    }

    error_id_t AdaptedApi::place_order(WCOrderRequest const& request) {
        ApiSingleOrder single_order;
        std::memset(&single_order, 0, sizeof(single_order));
        //single_order.trade_unit = trade_unit_;
        single_order.order_client_id = request.client_order_id;
        std::snprintf(single_order.ticker, sizeof(single_order.ticker), "%06d", request.instrument);
        switch (get_belonged_market(request.instrument)) {
            case market_t::sh: 
            case market_t::shsecond: 
                single_order.market = ApiMarket::XTP_MKT_SH_A; 
                break;
            case market_t::sz: 
            case market_t::szsecond: 
                single_order.market = ApiMarket::XTP_MKT_SZ_A; 
                break;
            default: single_order.market = ApiMarket::XTP_MKT_UNKNOWN; break;
        }
        single_order.price = request.price;
        single_order.quantity = request.volume;
        single_order.side = (request.side == side_t::buy) ? ApiSide::XTP_SIDE_BUY : ApiSide::XTP_SIDE_SELL;
        single_order.price_type = ApiPriceType::XTP_PRICE_LIMIT;
        single_order.business_type = Apibusiness::XTP_BUSINESS_TYPE_CASH; 
     // single_order.algo_parameters = nullptr; // not use algorithms trading

        int ret = p_broker_api_->InsertOrder(&single_order,trade_unit_);
        return error_id_t::success;
    }

    error_id_t AdaptedApi::cancel_order(WCOrderCancelRequest const& request) {
        int ret = p_broker_api_->CancelOrder(request.client_order_id,trade_unit_);
        return error_id_t::success;
    }

    error_id_t AdaptedApi::query_balance() {
        int ret = p_broker_api_->QueryAsset(session_id_, get_request_id());
        if (ret) {
            unique_ptr<ApiText> error_info(new ApiText(p_broker_api->GetApiLastError()));
            p_logger_->error("QueryAsset failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
            return error_id_t::unknown;
        }
        else return error_id_t::success;
    }

    error_id_t AdaptedApi::query_position(WCPositionQueryRequest const& request) {
        if (request.query_all) {
            // Ticker being NULL means querying all
            // QueryPos only use MARKET_TYPE when ticker isn't NULL
            int ret = p_broker_api_->QueryPosition(NULL, session_id_, get_request_id());
            if (ret) {
                unique_ptr<ApiText> error_info(new ApiText(p_broker_api->GetApiLastError()));
                p_logger_->error("QueryPosition of all tickers failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
                return error_id_t::unknown;
            }
        }
        else {
            string ticker = std::to_string(request.instrument);
            if (ticker[0] == '6') {
                int ret = p_broker_api_->QueryPosition(ticker.c_str(), session_id_, get_request_id(), ApiMarket::XTP_MKT_SH_A);
                if (ret) {
                unique_ptr<ApiText> error_info(new ApiText(p_broker_api->GetApiLastError()));
                p_logger_->error("QueryPosition of sh market failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
                return error_id_t::unknown;
                }
            }
            else if (ticker[0] == '0' || ticker[0] == '3') {
                int ret = p_broker_api_->QueryPosition(ticker.c_str(), session_id_, get_request_id(), ApiMarket::XTP_MKT_SZ_A);
                if (ret) {
                unique_ptr<ApiText> error_info(new ApiText(p_broker_api->GetApiLastError()));
                p_logger_->error("QueryPosition of sz market failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
                return error_id_t::unknown;
                }
            }
        }
        return error_id_t::success;
    }
} /* namespace api     */
} /* namespace wct     */