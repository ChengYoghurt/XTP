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

// algo type 
enum class algo_type_t{ 
    unknown    = 0,
    TWAP       = 3510, 
    VWAP       = 3511, 
    TWAP_PLUS  = 3101, 
    VWAP_PLUS  = 3102,
};

// algo login info
struct AlgoLoginConfig{
    std::string algo_server_ip;
    int algo_server_port;
    std::string algo_username;
    std::string algo_password;
    std::string local_ip;
};
// algo order static info 
struct AlgoConfig{
    std::string algo_name;
    algo_type_t algo_type;
    bool limit_action;
    bool expire_action;
    double paticipation_rate;
    int style;
};

// user order info 
struct UserOrderInfo{
    uint64_t xtp_strategy_id;
    instrument_id_t instrument_id;
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
    void on_login(session_t session_);
    bool setinstrument(order_id_t const& strategy_id,instrument_id_t const& instrument_id);
    std::condition_variable cv_established_;
    bool established_channel_;
protected:
    virtual void OnDisconnected(uint64_t session_id, int reason); 
    virtual void OnAlgoDisconnected(int reason); 

    virtual void OnALGOUserEstablishChannel(char* user, XTPRI* error_info, uint64_t session_id);
    virtual void OnInsertAlgoOrder(ApiInsertReport* strategy_info, XTPRI *error_info, uint64_t session_id);

    virtual void OnStrategyStateReport(ApiOrderReport* strategy_state, uint64_t session_id);
	virtual void OnCancelAlgoOrder(ApiOrderCancelReport* strategy_info, XTPRI *error_info, uint64_t session_id);
    
   

protected:
    static order_status_t simplify_status(ApiOrderStatus) ;

protected:
    uint32_t trade_id_;
    std::unique_ptr<WCSpi> p_spi_;
    std::shared_ptr<spdlog::logger> p_logger_;
    std::unordered_map<order_id_t,UserOrderInfo> strategy_to_order_info;
    
};/* class AdaptedSpi */

class AdaptedApi : public wct::api::WCApi
{
public:
    AdaptedApi::AdaptedApi(AlgoLoginConfig const& algo_login_config, AlgoConfig const& algo_config,uint32_t client_id, std::string filepath)
        :algo_config_(algo_config),
        algo_login_config_(algo_login_config),
        p_logger_(spdlog::get("AdaptedApi"))
    {
        p_broker_api_ = BrokerApi::CreateTraderApi(client_id,filepath.c_str());
        p_spi_ = nullptr;
    }
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
    AlgoLoginConfig algo_login_config_;
    AlgoConfig algo_config_;
};  /* class AdaptedApi */

} /* namespace wrapper */
} /* namespace wct     */

