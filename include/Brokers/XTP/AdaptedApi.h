#pragma once

#include "AdaptedTypes.h"
#include "WCTrader/TraderTypes.h"
#include "WCTrader/WCApi.h"
#include "spdlog/spdlog.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace wct     {
namespace api     {

class AdaptedSpi: public BrokerSpi
{
public:
    AdaptedSpi(std::unique_ptr<WCSpi> p_spi)
        : p_logger_(spdlog::get("AdaptedSpi"))
        , p_spi_(std::move(p_spi))
    {}
    void onLogin(session_t session_id, error_id_t error_id);
    virtual ~AdaptedSpi() = default;
protected:
    virtual void OnDisconnected(uint64_t session_id, int reason); 
    virtual void OnQueryPosition(ApiPosition *position, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) ;
    virtual void OnQueryAsset(ApiBalance *asset, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) ;
    virtual void OnOrderEvent(ApiOrderReport *order_info, ApiText error_info, uint64_t session_id);
    virtual void OnTradeEvent(ApiTradeReport *trade_info, uint64_t session_id);
    virtual void OnCancelOrderError(ApiOrderCancelReject *cancel_info, ApiText *error_info, uint64_t session_id);

protected:
    uint32_t trade_id_;
    std::unique_ptr<WCSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;
    std::unordered_map<uint64_t,order_id_t> order_id_xtptowc;
};

class AdaptedApi : public wct::api::WCApi
{
public:
    AdaptedApi() ;
    AdaptedApi(const uint32_t client_id, const std::string filepath, XTP_LOG_LEVEL log_level = XTP_LOG_LEVEL_INFO);
    virtual ~AdaptedApi() ;
    virtual std::string version() const noexcept ;
    virtual error_id_t login(WCLoginRequest const& request);
    int get_trading_day() ;
    ApiRequestID get_request_id() ;
    uint64_t get_session_id();
    error_id_t register_spi(std::unique_ptr<WCSpi> p_spi) ;
    error_id_t place_order(WCOrderRequest const& request) ;
    error_id_t cancel_order(WCOrderCancelRequest const& request) ;
    error_id_t query_balance() ;
    error_id_t query_position(WCPositionQueryRequest const& request) ;
    error_id_t query_credit_balance() ;
    error_id_t place_basket_order(WCBasketOrderRequest const& request) ;
    error_id_t cancel_basket_order(WCBasketOrderCancelRequest const& request) ;
    
protected:
    //ApiRequestID get_request_id();
    static order_status_t simplify_status(ApiOrderStatus) ;
protected:
    BrokerApi * p_broker_api_;
    uint32_t trade_id_;
    size_t request_id_ = 0;
    uint64_t session_id_;
    std::unique_ptr<AdaptedSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_; 

    std::unordered_map<order_id_t,uint64_t> order_id_wctoxtp;
};  /* class AdaptedApi */

} /* namespace wrapper */
} /* namespace wct     */

