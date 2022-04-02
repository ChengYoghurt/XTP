#pragma once

#include "AdaptedTypes.h"
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
    
    virtual void OnDisconnected(uint64_t session_id, int reason) ; 
    virtual void OnQueryPosition(ApiPosition *position, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) ;
    virtual void OnQueryAsset(ApiBalance *asset, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) ;

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
    
    
    virtual error_id_t  login(WCLoginRequest const& request) ;
    
protected:
    ApiRequestID get_request_id();
    static order_status_t simplify_status(ApiOrderStatus) ;
protected:
    BrokerApi * p_trader_api_;
    uint32_t trade_id_;
    size_t request_id_ = 0;
    uint64_t session_id_;
    ApiTradeUnitID trade_unit_;
    std::unique_ptr<AdaptedSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;

};  /* class AdaptedApi */

} /* namespace wrapper */
} /* namespace wct     */

