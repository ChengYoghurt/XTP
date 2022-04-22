#include "AdaptedApi.h"
#include "KuafuUtils.h"
#include <cstring>

namespace wct     {
namespace api     {

    void AdaptedSpi::OnDisconnected(uint64_t session_id, int reason) {
        p_logger_->error("Disconnected, session_id = {}, reason = {}", session_id, reason);
        p_spi_->on_disconnected(error_id_t::not_connected_to_server);
    }
    void AdaptedSpi::OnAlgoDisconnected(int reason) {
        p_logger_->error("AlgoDisconnected, reason = {}",reason);
    //    p_spi_->on_disconnected(error_id_t::not_connected_to_server);
    }
    void AdaptedSpi::OnALGOUserEstablishChannel(char* user, XTPRI* error_info, uint64_t session_id) {

    }
    void AdaptedSpi::OnInsertAlgoOrder(XTPStrategyInfoStruct* strategy_info, XTPRI *error_info, uint64_t session_id) {

    }
    void AdaptedSpi::OnStrategyStateReport(ApiOrderReport* strategy_state, uint64_t session_id) {
        WCOrderResponse order_rsp;
        std::memset(&order_rsp, 0, sizeof(order_rsp));
        order_rsp.client_order_id  = strategy_state->m_strategy_info.m_client_strategy_id;
        order_rsp.instrument       =strategy_to_instrument_id[order_rsp.client_order_id];
        order_rsp.volume           = strategy_state->m_strategy_qty;
        order_rsp.price            = strategy_state->m_strategy_execution_price;
        order_rsp.traded           = strategy_state->m_strategy_execution_qty;
        order_rsp.average_price    = strategy_state->m_strategy_market_price;//no average but has market
        //order_rsp.order_status     = simplify_status(strategy_state->order_status);
        order_rsp.error_id         = error_id_t::unknown;
        //order_rsp.transaction_time = strategy_state->update_time % 1'000000'000;  // no transaction time
        order_rsp.host_time        = timestamp_t::now();
        p_spi_->on_order_event(order_rsp);
    };
    void AdaptedSpi::OnCancelAlgoOrder(ApiOrderCancelReport *cancel_info, ApiText *error_info, uint64_t session_id) {
      /*  if(!OrderID(cancel_info->client_order_id).is_from_trader(trade_id_)) {
            return;
        }*///no such
        if(error_info->error_id != 0){
            WCCancelRejectedResponse order_rsp;
            order_rsp.client_order_id  =cancel_info->m_client_strategy_id;
            order_rsp.error_id         = error_id_t::unknown;
            p_spi_->on_cancel_rejected(order_rsp);
        }
    }
    bool AdaptedSpi::setinstrument(order_id_t const&strategy_id,instrument_id_t const&instrument_id){
        strategy_to_instrument_id[strategy_id] = instrument_id;
        return true;
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
        int ret                     = p_broker_api_->Login(ip.c_str(), port, user.c_str(), password.c_str(), sock_type, local_ip.c_str());//oms server
  //    int ret                     = p_broker_api_->ALGOUserEstablishChannel(ip.c_str(), port, user.c_str(), password.c_str(),session_id_) ; //oms server                   
        if(ret == 0) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("Login failed, error_id = {}, error_message = {}",error_info->error_id, error_info->error_msg);
            return error_id_t::not_login;
        }
        ret                         = p_broker_api_->Login(ip.c_str(), port, user.c_str(), password.c_str(), sock_type, local_ip.c_str());//algo server
        return error_id_t::success;
        
    }

    ApiRequestID AdaptedApi::get_request_id() {
        return ++request_id_;
    }

    uint64_t AdaptedApi::get_session_id() {
        return session_id_;
    }

    int AdaptedApi::get_trading_day() {
        std::string trading_day_str = p_broker_api_->GetTradingDay();
        return std::stoi(trading_day_str);
    }

    error_id_t AdaptedApi::place_order(WCOrderRequest const& request) {
/*        ApiSingleOrder single_order;
        std::memset(&single_order, 0, sizeof(single_order));
        //single_order.trade_unit = trade_unit_;
        single_order.order_client_id = request.client_order_id;
        std::snprintf(single_order.ticker, sizeof(single_order.ticker), "%06d", request.instrument);
        switch ((wct::market_t)get_belonged_market(request.instrument)) {
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
        single_order.side = (request.side == side_t::buy) ? XTP_SIDE_BUY : XTP_SIDE_SELL;
        single_order.price_type = ApiPriceType::XTP_PRICE_LIMIT;
        single_order.business_type = ApiBusiness::XTP_BUSINESS_TYPE_CASH; 
     // single_order.algo_parameters = nullptr; // not use algorithms trading

        int xtp_order_id = p_broker_api_->InsertOrder(&single_order,session_id_);
        strategy_id_wctoxtp[request.client_order_id] = xtp_order_id;
        if (xtp_order_id) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("InsertOrder failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
            return error_id_t::unknown;
        }
        else */return error_id_t::success;
    }

    error_id_t AdaptedApi::cancel_order(WCOrderCancelRequest const& request) {

    }

    error_id_t AdaptedApi::query_balance() {

    }

    error_id_t AdaptedApi::query_credit_balance(){

    }

    error_id_t AdaptedApi::place_basket_order(WCBasketOrderRequest const& request){
        std::string strategy_param;
    
        uint32_t strategy_type;
        

        int ret = p_broker_api_->InsertAlgoOrder(strategy_type,request.client_basket_id,"1",session_id_);
        if (ret) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("InsertAlgoOrder of all tickers failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
            return error_id_t::unknown;
        }
         return error_id_t::success;
    }
    error_id_t AdaptedApi::cancel_basket_order(WCBasketOrderCancelRequest const& request){
        
        int ret = p_broker_api_->CancelAlgoOrder(true, request.client_order_id, session_id_);

        if (ret) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("CancelAlgoOrder of all tickers failed, error_id = {}, error_message = {}", error_info->error_id, error_info->error_msg);
            return error_id_t::unknown;
        }
        return error_id_t::success;
    }

} /* namespace api     */
} /* namespace wct     */