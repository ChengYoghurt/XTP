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

enum class algo_type_t{ 
    unknown    = 0,
    TWAP       = 3510, 
    VWAP       = 3511, 
    TWAP_PLUS  = 3101, 
    VWAP_PLUS  = 3102,
};

struct AlgoLoginConfig{
    std::string algo_server_ip;
    int algo_server_port;
    std::string algo_username;
    std::string algo_password;
    std::string local_ip;
};

struct AlgoConfig{
    std::string algo_name;
    algo_type_t algo_type;
    bool limit_action;
    bool expire_action;
    double paticipation_rate;
    int style;
};

class AdaptedSpi: public BrokerSpi
{
public:
    AdaptedSpi(std::unique_ptr<WCSpi> p_spi)
        : p_logger_(spdlog::get("AdaptedSpi"))
        , p_spi_(std::move(p_spi))
    {}
    virtual ~AdaptedSpi() = default;
    u_int64_t qurry_xtp_id(order_id_t client_order_id) ;
protected:
    virtual void OnDisconnected(uint64_t session_id, int reason); 
    virtual void OnAlgoDisconnected(int reason); 
//    virtual void OnAlgoConnected(){};
//    virtual void OnQueryPosition(ApiPosition *position, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) ;
//    virtual void OnQueryAsset(ApiBalance *asset, ApiText *error_info, ApiRequestID request_id, bool is_last, uint64_t session_id) ;
    
    virtual void OnALGOUserEstablishChannel(char* user, XTPRI* error_info, uint64_t session_id);
    virtual void OnInsertAlgoOrder(ApiInsertReport* strategy_info, XTPRI *error_info, uint64_t session_id);
    virtual void OnStrategyStateReport(ApiOrderReport* strategy_state, uint64_t session_id);
	virtual void OnCancelAlgoOrder(ApiOrderCancelReport* strategy_info, XTPRI *error_info, uint64_t session_id);
//    virtual void OnOrderEvent(ApiOrderReport *order_info, ApiText error_info, uint64_t session_id);
//    virtual void OnTradeEvent(ApiTradeReport *trade_info, uint64_t session_id);
//    virtual void OnCancelOrderError(ApiOrderCancelReject *cancel_info, ApiText *error_info, uint64_t session_id);
    bool setinstrument(order_id_t const&strategy_id,instrument_id_t const&instrument_id);
protected:
    static order_status_t simplify_status(ApiOrderStatus) ;

protected:
    uint32_t trade_id_;
    std::unique_ptr<WCSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;
    std::unordered_map<order_id_t,instrument_id_t> strategy_to_instrument_id;
    std::unordered_map<order_id_t,u_int64_t> strategy_to_xtp_id;
};

class AdaptedApi : public wct::api::WCApi
{
public:
    AdaptedApi::AdaptedApi(AlgoLoginConfig const& algo_login_config, AlgoConfig const& algo_config)
        :algo_config_(algo_config),
        algo_login_config_(algo_login_config)
    {}
    virtual ~AdaptedApi() ;
    virtual std::string version() const noexcept ;
    virtual error_id_t login(WCLoginRequest const& request);
    int get_trading_day() ;
    ApiRequestID get_request_id() ;
    uint64_t get_session_id();
    error_id_t register_spi(std::unique_ptr<WCSpi> p_spi) ;
    error_id_t place_order(WCOrderRequest const& request){} ;
    error_id_t cancel_order(WCOrderCancelRequest const& request) ;
    error_id_t query_balance() ;
    error_id_t query_position(WCPositionQueryRequest const& request);
    error_id_t query_credit_balance() ;
    error_id_t place_basket_order(WCBasketOrderRequest const& request) ;
    error_id_t cancel_basket_order(WCBasketOrderCancelRequest const& request) ;

protected:
    //ApiRequestID get_request_id();
    //static order_status_t simplify_status(ApiOrderStatus) ;

protected:
    BrokerApi * p_broker_api_;
    uint32_t trade_id_;
    size_t request_id_ = 0;
    uint64_t session_id_;
    std::unique_ptr<AdaptedSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;
    std::unordered_map<order_id_t,uint64_t> strategy_id_wctoxtp;
    AlgoLoginConfig algo_login_config_;
    AlgoConfig algo_config_;
};  /* class AdaptedApi */

} /* namespace wrapper */
} /* namespace wct     */

