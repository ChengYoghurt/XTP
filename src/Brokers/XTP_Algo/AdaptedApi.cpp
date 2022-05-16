#include "AdaptedApi.h"
#include "KuafuUtils.h"
#include "YAMLGetField.h"
#include <cstring>

namespace wct     {
namespace api     {
    error_id_t map_error_id (int32_t xtp_error_id) {
        error_id_t wctrader_error_id;
        switch(xtp_error_id){
        // 10200000 Login to quote server failed
        // 10200003 Login to quote server failed: invalid parameters
        // 10210000 Login to oms server failed
        // 10210003 Login to oms server failed: invalid parameters
        case 10200000 : 
        case 10200003 : 
        case 10210000 : 
        case 10210003 : wctrader_error_id = error_id_t::not_login;
        break;
        // TODO case ? : wctrader_error_id = error_id_t::login_timeout;
        break;
        case 10200006 : 
        case 10210006 : wctrader_error_id = error_id_t::not_connected_to_server;
        break;
        // 11000030 Authentication Failed! User or password is not correct
        case 11000030 : wctrader_error_id = error_id_t::fail_authentication;
        break;
        // 11000303 Failed to check order
        // 11000316	Failed to check order id
        // 11000317 Failed to check original order id
        case 11000303 : 
        case 11000316 : 
        case 11000317 : wctrader_error_id = error_id_t::wrong_client_order_id;   
        break;
        // 11000010 Failed to get ticker quotes, ticker does not exist or cannot be traded
        // 11000404 Failed to check ticker
        // 11200003 unknown ticker id
        // 11000350 Find none record
        case 11000010 :
        case 11000404 :
        case 11200003 : 
        case 11000350 : wctrader_error_id = error_id_t::wrong_instrument_id;   
        break;
        // 11010562 Invalid market
        // 11000108 Parameter market invalid
        case 11010562 : 
        case 11000108 : wctrader_error_id = error_id_t::wrong_market_id;
        // TODO case ? : wctrader_error_id = error_id_t::wrong_request_id;   
        break;
        // 11000452 User query frequency limited
        case 11000452 : wctrader_error_id = error_id_t::too_freq_query;   
        break;
        // 11000450 Too much order,order frequency limited
        case 11000450 : wctrader_error_id = error_id_t::too_freq_trade;   
        break;
        // 11000343 Target order already finished
        case 11000343 : wctrader_error_id = error_id_t::cancel_after_traded;   
        break;
        default: wctrader_error_id = error_id_t::unknown; 
        break;
        }
        return wctrader_error_id;
    }
    
    void AdaptedSpi::OnDisconnected(uint64_t session_id, int reason) {
        switch(reason){
            case 10200006: p_logger_->error("Disconnected from quote server,session_id={},reason={}", session_id, reason); 
            case 10210006: p_logger_->error("Disconnected from trade server,session_id={},reason={}", session_id, reason); 
        }
        p_spi_->on_disconnected(error_id_t::not_connected_to_server);
    }

    void AdaptedSpi::OnAlgoDisconnected(int reason) {
        p_logger_->error("OnAlgoDisconnected,reason={}",reason);
        //the system will automatically reconnet
    }

    void AdaptedSpi::OnALGOUserEstablishChannel(char* user, XTPRI* error_info, uint64_t session_id) {
        if (error_info == nullptr || error_info->error_id == 0) {
            p_logger_->info("ALGOUserEstablishChannel successfully");
            established_channel_ = true;
            cv_established_.notify_one();
            p_logger_->debug("OnALGOUserEstablishChannel,Successfully");
        }
        else {
            p_logger_->error("OnALGOUserEstablishChannel,Failed,error_id={},msg={}", 
            error_info->error_id, error_info->error_msg);
        }
    }

    void AdaptedSpi::OnInsertAlgoOrder(ApiInsertReport* strategy_info, XTPRI *error_info, uint64_t session_id) {
        if (error_info == nullptr || error_info->error_id == 0) {
            p_logger_->debug("OnInsertAlgoOrder,Successfully");
            if (strategy_info) {
                p_logger_->info("OnInsertAlgoOrder,strategy_type={},strategy_state={},client_strategy_id={},xtp_strategy_id={}", 
                strategy_info->m_strategy_type,
                strategy_info->m_strategy_state,
                strategy_info->m_client_strategy_id,
                strategy_info->m_xtp_strategy_id);

                WCOrderResponse order_rsp;
                order_rsp.client_order_id  = strategy_info->m_client_strategy_id;
                order_rsp.order_status = order_status_t::accepted;
                p_spi_->on_order_event(order_rsp);
            }
        }
        else {
            p_logger_->error("InsertAlgoOrder,Failed,error_id={},error_msg={}",
            error_info->error_id,
            error_info->error_msg);
        }
    }

    void AdaptedSpi::OnStrategyStateReport(ApiOrderReport* strategy_state, uint64_t session_id) {
        strategy_to_order_mutex.lock();
        if(strategy_to_order_info.find(strategy_state->m_strategy_info.m_client_strategy_id)==strategy_to_order_info.end()){
            p_logger_->warn("OnStrategyStateReport,not in records,perhaps placed by another client_id");
            strategy_to_order_mutex.unlock();
            return;
        }
        WCOrderResponse order_rsp;
        std::memset(&order_rsp, 0, sizeof(order_rsp));
        order_rsp.client_order_id  = strategy_state->m_strategy_info.m_client_strategy_id;
        order_rsp.instrument       = strategy_to_order_info[order_rsp.client_order_id].instrument_id;
        order_rsp.volume           = strategy_state->m_strategy_qty;
        order_rsp.price            = strategy_state->m_strategy_execution_price;
        order_rsp.traded           = strategy_state->m_strategy_execution_qty;
        order_rsp.average_price    = strategy_state->m_strategy_market_price;//no average but has market
        order_rsp.order_status     = simplify_status(strategy_state->m_strategy_info.m_strategy_state);
        order_rsp.error_id         = map_error_id(strategy_state->m_error_info.error_id);
        //// no transaction time
        order_rsp.host_time        = timestamp_t::now();

        strategy_to_order_info[order_rsp.client_order_id].xtp_strategy_id = strategy_state->m_strategy_info.m_xtp_strategy_id;
        strategy_to_order_mutex.unlock();
        p_logger_->debug("OnStrategyStateReport,Successfully");
        p_spi_->on_order_event(order_rsp);
    };

    void AdaptedSpi::OnCancelAlgoOrder(ApiOrderCancelReport *cancel_info, ApiText *error_info, uint64_t session_id) {
        if(error_info == nullptr || error_info->error_id == 0) {
            p_logger_->debug("OnCancelAlgoOrder,Successfully");
            if (cancel_info) {
                p_logger_->info("OnCancelAlgoOrder,strategy_type={},strategy_state={},client_strategy_id={},xtp_strategy_id={}", 
                cancel_info->m_strategy_type,
                cancel_info->m_strategy_state,
                cancel_info->m_client_strategy_id,
                cancel_info->m_xtp_strategy_id);
                WCOrderResponse order_rsp;
                order_rsp.client_order_id  = cancel_info->m_client_strategy_id;
                order_rsp.order_status = order_status_t::canceled;
                p_spi_->on_order_event(order_rsp);
            }
        }
        else {
            if(cancel_info) {
                WCCancelRejectedResponse order_rsp;
                order_rsp.client_order_id  = cancel_info->m_client_strategy_id;
                order_rsp.error_id         = map_error_id(error_info->error_id);
                p_spi_->on_cancel_rejected(order_rsp);
            }
        }
    }

    void AdaptedSpi::on_login(session_t session_id, error_id_t error_id) {
        WCLoginResponse login_rsp;
        login_rsp.session_id = session_id;
        login_rsp.error_id = error_id;
        if(error_id == error_id_t::success) {
            p_logger_->debug("on_login,Successfully");

        }
        else {
            p_logger_->error("on_login,Failed");
        }
        p_spi_->on_login(login_rsp);
    }

    bool AdaptedSpi::setinstrument(order_id_t const&client_order_id,instrument_id_t const&instrument_id){
        strategy_to_order_mutex.lock();
        strategy_to_order_info[client_order_id].instrument_id = instrument_id;
        strategy_to_order_mutex.unlock();
        return true;
    }

    u_int64_t AdaptedSpi::qurry_xtp_id(order_id_t client_order_id){
        strategy_to_order_mutex.lock();
        auto order_info_item = strategy_to_order_info.find(client_order_id);
        if(order_info_item !=strategy_to_order_info.end()){
            strategy_to_order_mutex.unlock();
            if(order_info_item->second.xtp_strategy_id == 0){
                p_logger_->debug("place wait insert or callback");
                return 0;
            }
            return order_info_item->second.xtp_strategy_id;
        }
        strategy_to_order_mutex.unlock();
        p_logger_->error("not in allrecords");
        return 0;
    }
    order_status_t AdaptedSpi::simplify_status(ApiOrderStatus const& order_status){
        order_status_t local_order_status;
        switch(order_status){
        case XTP_STRATEGY_STATE_CREATING  : 
        case XTP_STRATEGY_STATE_CREATED   :
        case XTP_STRATEGY_STATE_STARTING  : local_order_status = order_status_t::created;
        break;
        case XTP_STRATEGY_STATE_STARTED   : local_order_status = order_status_t::accepted;
        break;
        case XTP_STRATEGY_STATE_STOPPING  :
        case XTP_STRATEGY_STATE_STOPPED   : 
        case XTP_STRATEGY_STATE_DESTROYING:
        case XTP_STRATEGY_STATE_DESTROYED : local_order_status = order_status_t::completed;
        break;
        case XTP_STRATEGY_STATE_ERROR     : local_order_status = order_status_t::rejected;   
        break;
        default: local_order_status = order_status_t::unknown; 
        break;
        }
        return local_order_status;
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
        p_broker_api_->SetSoftwareKey(request.agent_fingerprint.token.c_str());
        int ret                     = p_broker_api_->Login(ip.c_str(), port, user.c_str(), password.c_str(), sock_type, local_ip.c_str());//oms server
        if(ret == 0) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("Login, Failed,error_id={},error_message={}",error_info->error_id, error_info->error_msg);
            p_spi_->on_login(ret, error_id_t::not_login);
            return error_id_t::not_login;
        }
        session_id_ = ret;
        p_logger_->debug("Login oms Successfully");
        XTP_PROTOCOL_TYPE algo_sock_type = XTP_PROTOCOL_TCP                  ;
        //this is for test,need to modified
        p_logger_->info("ip={},port={},name={},pw={}",
                        algo_login_config_.algo_server_ip,
                        algo_login_config_.algo_server_port,
                        algo_login_config_.algo_username,
                        algo_login_config_.algo_password);

        ret = p_broker_api_->LoginALGO(
              algo_login_config_.algo_server_ip.c_str(), 
              algo_login_config_.algo_server_port,
              algo_login_config_.algo_username.c_str(), 
              algo_login_config_.algo_password.c_str(), 
              algo_sock_type);//algo server

        if(ret != 0) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("LoginAlgo,Failed,error_id={},error_message={}", error_info->error_id, error_info->error_msg);
            p_spi_->on_login(session_id_, error_id_t::not_login);
            return error_id_t::not_login;
        }else{
            p_logger_->debug("LoginAlgo,Successfully");
        }
    
        ret = p_broker_api_->ALGOUserEstablishChannel(ip.c_str(), port, user.c_str(), password.c_str(),session_id_) ; //oms server                   
        if(ret != 0) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("EstablishChannel Failed,error_id={},error_message={}",error_info->error_id, error_info->error_msg);
            
        }
        else {
            p_logger_->debug("Waiting to establish channel...");
            std::mutex mutex;
            if(!p_spi_->check_established()){
                p_spi_->set_established(false);
            }

            {
                std::unique_lock lk(mutex);
                bool established_ret = p_spi_->cv_established_.wait_for(lk, std::chrono::seconds(3), [this]{return p_spi_->check_established();});
                if(established_ret) {
                    p_logger_->debug("CV return,Channel established!");
                    p_spi_->on_login(session_id_, error_id_t::success);
                    return error_id_t::success;
                }
                p_logger_->error("CV timed out,Channel establish Failed!");
            }
        } 
        // Call on_login even if Establish Failed
        p_spi_->on_login(session_id_, error_id_t::not_login);
        return error_id_t::not_login;
    }

    ApiRequestID AdaptedApi::get_request_id() {
        return ++request_id_;
    }

    uint64_t AdaptedApi::get_session_id() {
        return session_id_;
    }

    int AdaptedApi::get_trading_day() {
        std::string trading_day_str = p_broker_api_->GetTradingDay();
        return 0;//the trading day type is not clear
    }

    error_id_t AdaptedApi::place_order(WCOrderRequest const& request) {

    }

    error_id_t AdaptedApi::cancel_order(WCOrderCancelRequest const& request) {

    }

    error_id_t AdaptedApi::query_balance() {

    }

    error_id_t AdaptedApi::query_credit_balance(){

    }
    error_id_t AdaptedApi::query_position(WCPositionQueryRequest const& request){

    }

    error_id_t AdaptedApi::place_basket_order(WCBasketOrderRequest const& request){
        uint32_t strategy_type = static_cast<uint32_t>(algo_config_.algo_type);
        std::string start_time = std::string(request.start_time.str(),0,8);
        std::string end_time   = std::string(request.end_time.str(),0,8);
        std::string limit_action;
        std::string expire_action;  

        if(algo_config_.limit_action) limit_action = "true";   
        else limit_action = "false";
        if(algo_config_.expire_action) expire_action = "true";   
        else expire_action = "false";

        for(auto& basket_leg: request.basket_legs){
            std::string strategy_param;
            std::string market;
            std::string side;
            
            p_spi_->setinstrument(basket_leg.client_order_id,basket_leg.instrument);
            
            switch ((wct::market_t)get_belonged_market(basket_leg.instrument)) {
                case market_t::sh: 
                case market_t::shsecond: 
                    market = "SH"; 
                    break;
                case market_t::sz: 
                case market_t::szsecond: 
                    market = "SZ"; 
                    break;
                default: break;
            }
            switch (basket_leg.side)
            {
            case side_t::buy: side = "BUY";
                break;
            case side_t::sell: side = "SELL";
                break;
            default:
                break;
            }
            switch(algo_config_.algo_type){
                case algo_type_t::unknown: return error_id_t::unknown;
                break;
                case algo_type_t::TWAP_PLUS:
                case algo_type_t::VWAP_PLUS:
                strategy_param += "{\"start_time\": \"" ;
                strategy_param += start_time ;
                strategy_param += "\", \"end_time\": \"" ;
                strategy_param += end_time   ;
                strategy_param += "\", \"ticker\": \"" ;
                strategy_param += std::to_string(basket_leg.instrument) ;
                strategy_param += "\", \"market\": \"" ;
                strategy_param += market     ;
                strategy_param += "\", \"side\": \"" ;
                strategy_param += side       ;
                strategy_param += "\", \"quantity\": " ;
                strategy_param += std::to_string(basket_leg.volume) ;
                strategy_param += "\", \"limit_action\": " ;
                strategy_param += limit_action ;
                strategy_param += ", \"expire_action\": " ;
                strategy_param +=expire_action ;
                strategy_param += ", \"price\": " ;
                strategy_param += std::to_string(basket_leg.price) ;
                strategy_param +=" }";
                break;
                case algo_type_t::TWAP:
                case algo_type_t::VWAP:
                strategy_param += "{\"start_time\": \""  ;
                strategy_param += start_time ;
                strategy_param += "\", \"end_time\": \"" ;
                strategy_param += end_time ;  
                strategy_param += "\", \"market\": \""   ;
                strategy_param += market ;
                strategy_param += "\", \"ticker\": \""   ;
                strategy_param += std::to_string(basket_leg.instrument);
                strategy_param += "\", \"side\": \""     ;
                strategy_param += side ;       
                strategy_param += "\", \"quantity\": "   ;
                strategy_param +=std::to_string(basket_leg.volume);
                strategy_param += ", \"price\": "        ; 
                strategy_param += std::to_string(basket_leg.price);
                strategy_param += ", \"business_type\": \"";
                strategy_param += "CASH" ;
                strategy_param += "\", \"participation_rate\": ";
                strategy_param += std::to_string(algo_config_.paticipation_rate);
                strategy_param +=", \"style\": ";
                strategy_param += std::to_string(algo_config_.style);
                strategy_param += " }";
                break;
                default:break;
            }

            int constexpr k_max = 1024;
            char strategy_param_c[k_max];
            snprintf(strategy_param_c,k_max,strategy_param.c_str());
            int ret = p_broker_api_->InsertAlgoOrder(strategy_type,basket_leg.client_order_id,strategy_param_c,session_id_);        
            if (ret != 0) {
                const  ApiText* error_info = p_broker_api_->GetApiLastError();
                p_logger_->error("InsertAlgoOrder failed,error_id={},error_message={}", error_info->error_id, error_info->error_msg);
            }else{
                p_logger_->debug("InsertAlgoOrder success,basket_id={},param={},",basket_leg.client_order_id,strategy_param);
            }
        }
        return error_id_t::success;
    }
    error_id_t AdaptedApi::cancel_basket_order(WCBasketOrderCancelRequest const& request){
        uint32_t strategy_xtp_id = p_spi_->qurry_xtp_id(request.client_order_id);
        if(strategy_xtp_id == 0) return error_id_t::unknown;
        int ret = p_broker_api_->CancelAlgoOrder(true, strategy_xtp_id, session_id_);

        if (ret) {
            const  ApiText* error_info = p_broker_api_->GetApiLastError();
            p_logger_->error("CancelAlgoOrder of all tickers Failed,error_id={},error_message={}", error_info->error_id, error_info->error_msg);
            return error_id_t::unknown;
        }
        p_logger_->debug("CancelAlgoOrder success,cliet_id = {}",request.client_order_id);
        return error_id_t::success;
    }

} /* namespace api     */
} /* namespace wct     */