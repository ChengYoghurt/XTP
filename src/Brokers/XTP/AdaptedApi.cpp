#include "XTP/AdaptedApi.h"
#include "L2XTraderUtils.h"

#include <cstring>

namespace wct     {
namespace api     {

    void AdaptedSpi::OnDisconnected(uint64_t session_id, int reason) {
        p_logger_->error("Disconnected, session_id = {}, reason = {}", session_id, reason);
        p_spi_->on_disconnected(error_id_t::not_connected_to_server);
    }

    void AdaptedSpi::OnQueryPosition(ApiPosition *position, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) {
        WCPositionResponse pos_rsp;
        pos_rsp.instrument       = std::atoi(position->ticker)  ;
        pos_rsp.yesterday_volume = position->yesterday_position ;
        pos_rsp.latest_volume    = position->total_qty          ;////////////////////////////
        pos_rsp.available_volume = position->sellable_qty       ;
        pos_rsp.is_last          = is_last                      ; 
        if(error_info == nullptr || error_info->error_id == 0) {
            pos_rsp.error_id = error_id_t::success;
        } else {
            pos_rsp.error_id = error_id_t::unknown;
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

    void AdaptedSpi::OnQueryAsset(ApiBalance *asset, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) {
        WCBalanceResponse balance_rsp;
        balance_rsp.initial_balance   =                         ;
        balance_rsp.initial_balance   =                         ;//////////////////////
        balance_rsp.available_balance = asset->buying_power     ;
        balance_rsp.market_value      = asset->security_asset   ;//////////////////////
        balance_rsp.total_asset       = asset->total_asset      ;
        if(error_info == nullptr || error_info->error_id == 0) {
            pos_rsp.error_id = error_id_t::success;
        } else {
            pos_rsp.error_id = error_id_t::unknown;
        }
        p_spi_->on_query_balance(balance_rsp);
    }

    std::string AdaptedApi::version() const noexcept {
        return "0.0.0";
    }

    error_id_t AdaptedApi::login(WCLoginRequest const& request) {
        std::string ip              = request.server_ip; 
        int port                    = request.server_port;
        std::string user            = request.username;
        std::string password        = request.password;
        XTP_PROTOCOL_TYPE sock_type = XTP_PROTOCOL_TCP;
        std::string local_ip        = request.agent_fingerprint.local_ip;

        uint64_t session_id         = p_broker_api_->Login(ip.c_str(), port, user.c_str(), password.c_str(), sock_type, local_ip.c_str());
        if(session_id == 0) {
            XTPRI *reason = p_broker_api_->GetApiLastError();
            p_logger_->error("Login failed, error_id = {}, error_message = {}",reason->error_id, reason->error_msg);
            return error_id_t::not_login;
        }
        else {
            return error_id_t::success;
        }
    }

    ApiRequestID AdaptedApi::get_request_id() {
        return ++request_id_;
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