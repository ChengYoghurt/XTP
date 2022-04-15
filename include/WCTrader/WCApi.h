#pragma once

#include "TraderTypes.h"

namespace wct {
namespace api {

class WCSpi {
public:
    virtual ~WCSpi() = default;
    virtual void on_login(WCLoginResponse const& response) = 0;
    virtual void on_logout() = 0;
    virtual void on_connected() = 0;
    virtual void on_disconnected(error_id_t error_id) = 0;
    virtual void on_order_event(WCOrderResponse const& response) = 0;
    virtual void on_trade_event(WCTradeResponse const& response) = 0;
    virtual void on_cancel_rejected(WCCancelRejectedResponse const& response) = 0;
<<<<<<< HEAD
    virtual void on_query_position(WCPositionResponse const& response) = 0; 
    virtual void on_query_balance(WCBalanceResponse const& response) = 0; 
    virtual void on_query_credit_balance(WCCreditBalanceResponse const& response) = 0; 
=======
    virtual void on_query_position(WCPositionResponse const& response) = 0;
    virtual void on_query_balance(WCBalanceResponse const& response) = 0;
    virtual void on_query_credit_balance(WCCreditBalanceResponse const& response) = 0;
>>>>>>> 8f6e9e8c5afc702d7ea3e5fbb31a2a450222ea5a
};  /* class WCSpi */

class WCApi {
public:
    virtual ~WCApi() = default;
    virtual error_id_t  register_spi(std::unique_ptr<WCSpi> p_spi) = 0;
    virtual std::string version() const noexcept = 0;
    virtual int         get_trading_day() = 0;
    virtual error_id_t  login(WCLoginRequest const& request) = 0;
    virtual error_id_t  place_order(WCOrderRequest const& request) = 0;
    virtual error_id_t  cancel_order(WCOrderCancelRequest const& request) = 0;
    virtual error_id_t  query_balance() = 0;
    virtual error_id_t  query_position(WCPositionQueryRequest const& request) = 0;
    virtual error_id_t  query_credit_balance() = 0;

<<<<<<< HEAD
    virtual error_id_t place_basket_order(WCBasketOrderRequest const& request) = 0; //下一篮子单
=======
    virtual error_id_t place_basket_order(WCBasketOrderRequest const& request) = 0;
>>>>>>> 8f6e9e8c5afc702d7ea3e5fbb31a2a450222ea5a
    virtual error_id_t cancel_basket_order(WCBasketOrderCancelRequest const& request) = 0;
};  /* class WCApi */

} /* namespace api */ 
} /* namespace wct */
