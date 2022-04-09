#pragma once

#include "xtp_trader_api.h"

namespace wct     {
namespace api     {

using BrokerApi                      = XTP::API::TraderApi                     ;
using BrokerSpi                      = XTP::API::TraderSpi                     ;

// Export Spi Types
using ApiBalance                     = XTPQueryAssetRsp                        ;   
using ApiOrderCancelReject           = XTPOrderCancelInfo                      ;
using ApiOrderReport                 = XTPOrderInfo                            ;
using ApiOrderStatus                 = XTP_ORDER_STATUS_TYPE                   ;
using ApiPosition                    = XTPQueryStkPositionRsp                  ;
using ApiTradeReport                 = XTPTradeReport                          ;
using ApiText                        = XTPRI                                   ;
using ApiRequestID                   = int                                     ;
using ApiPriceType                   = XTP_PRICE_TYPE                          ;
using ApiSide                        = XTP_SIDE_TYPE                           ;
using ApiMarket                      = XTP_MARKET_TYPE                         ;
using Apiexchange                    = XTP_EXCHANGE_TYPE                       ;
using ApiBusiness                    = XTP_BUSINESS_TYPE                       ;
using ApiSingleOrder                 = XTPOrderInsertInfo                      ;

} /* namespace api     */
} /* namespace wct     */
