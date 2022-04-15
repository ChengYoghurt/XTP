/* WCRspHandler.h
 * Define callback dervied from TraderX SPI
 * 
 * Author: Wentao Wu
*/

#pragma once

#include "TraderTypes.h"

#include <variant>
#include <spdlog/spdlog.h>
#include <memory>

namespace wct {

class OrderKeeper   ;
class ControlSignals;
class WCTrader      ;

class WCResponseHandler {
public:
    WCResponseHandler(
        std::shared_ptr<ControlSignals> p_control_signals, 
        std::shared_ptr<OrderKeeper   > p_order_keeper    
    );

    void operator()(WCEmptyResponse    && no_response      ) ;
    void operator()(WCLoginResponse    && login_response   ) ;
    void operator()(WCTradeResponse    && trade_response   ) ;
    void operator()(WCOrderResponse    && order_response   ) ;
    void operator()(WCPositionResponse && position_response) ;
    void operator()(WCBalanceResponse  && balance_response ) ;

private:
    std::shared_ptr<ControlSignals> p_control_signals_ ;
    std::shared_ptr<OrderKeeper   > p_order_keeper_    ;
    std::shared_ptr<spdlog::logger> p_logger_          ;
};

} /* namespace wct */
