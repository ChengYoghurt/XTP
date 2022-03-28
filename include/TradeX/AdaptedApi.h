/* WCTrader.h
 * Implement target position with TraderX API/SPI
 * 
 * Author: Wentao Wu
*/
#pragma once

#include "TradeX/AdaptedTypes.h"
#include "TraderTypes.h"
#include "WCApi.h"
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
    virtual ~AdaptedSpi() = default;
protected:
    virtual void OnLogin(const ApiLoginResponse *loginResponse) ;
    virtual void OnLogout(const bool isLogoutSuccess, const char *error_message) ;
    virtual void OnConnected(const ApiServerType serverType, const char *url) ;
    virtual void OnDisconnected(const ApiServerType serverType, const char *url, const char *error_message) ;
    virtual void OnOrderEvent(const ApiOrderReport *orderReport) ;
    virtual void OnTradeEvent(const ApiTradeReport *tradeReport) ;
    virtual void OnCancelReject(const ApiOrderCancelReject *cancelReject) ;
    virtual void OnQueryPosition(const ApiPosition *position, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) ;
    virtual void OnQueryBalance(const ApiBalance *balance, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success) ;
    virtual void OnQueryOrder(const ApiOrderReport *order, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;
    virtual void OnQueryTrade(const ApiTradeReport *tradeReport, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;

    // Credit Operation
    virtual void OnQueryStandardCouponPosition(const ApiPosition *position, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;
    virtual void OnQueryBorrowingSecurity(const ApiBorrowingSecurity *borrowingSecurity, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;
    virtual void OnQuerySecurityLiability(const ApiSecurityLiability *securityLiability, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;
    virtual void OnQueryFinancingLiability(const ApiFinancingLiability *financingLiability, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;
    virtual void OnQueryCreditAsset(const ApiCreditAsset *creditAsset, const ApiText error_message, const ApiRequestID request_id, const bool is_last, const bool is_success = true) ;

    virtual void OnTradeUnitStatus(const ApiTradeUnitConnStatusNotice *notice) ;

protected:
    uint32_t trade_id_;
    std::unique_ptr<WCSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;
};

class AdaptedApi : public wct::api::WCApi
{
public:
    AdaptedApi() ;
    virtual ~AdaptedApi() ;
    virtual std::string version() const noexcept ;
    virtual int         get_trading_day() ;
    virtual error_id_t  register_spi(std::unique_ptr<WCSpi> p_spi) ;
    virtual error_id_t  login(WCLoginRequest const& request) ;
    virtual error_id_t  place_order(WCOrderRequest const& request) ;
    virtual error_id_t  cancel_order(WCOrderCancelRequest const& request) ;
    virtual error_id_t  query_balance() ;
    virtual error_id_t  query_position(WCPositionQueryRequest const& request) ;
    virtual error_id_t  query_credit_balance() ;

    virtual error_id_t  place_basket_order(WCBasketOrderRequest const& request) ;
    virtual error_id_t  cancel_basket_order(WCBasketOrderCancelRequest const& request) ;
protected:
    ApiRequestID get_request_id();
    static order_status_t simplify_status(ApiOrderStatus) ;
protected:
    BrokerApi* p_broker_api_;
    uint32_t trade_id_;
    ApiTradeUnitID trade_unit_;
    std::unique_ptr<AdaptedSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;

};  /* class AdaptedApi */

} /* namespace wrapper */
} /* namespace wct     */
