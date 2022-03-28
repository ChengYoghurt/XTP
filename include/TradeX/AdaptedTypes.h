#pragma once

#include "TradeX/tradex_trader_api.h"
#include "TradeX/tradex_messages.h"

namespace wct     {
namespace api     {

using BrokerApi                      = com::tradex::api::TradeXApi                     ;
using BrokerSpi                      = com::tradex::api::TradeXSpi                     ;

// Export Spi Types
using ApiBalance                     = com::tradex::api::TRXBalance                    ;
using ApiBorrowingSecurity           = com::tradex::api::TRXBorrowingSecurity          ;
using ApiCreditAsset                 = com::tradex::api::TRXCreditAsset                ;
using ApiFinancingLiability          = com::tradex::api::TRXFinancingLiability         ;
using ApiLoginResponse               = com::tradex::api::TRXLoginResponse              ;
using ApiOrderCancelReject           = com::tradex::api::TRXOrderCancelReject          ;
using ApiOrderReport                 = com::tradex::api::TRXOrderReport                ;
using ApiOrderStatus                 = com::tradex::api::TRXOrderStatus                ;
using ApiPosition                    = com::tradex::api::TRXPosition                   ;
using ApiSecurityLiability           = com::tradex::api::TRXSecurityLiability          ;
using ApiServerType                  = com::tradex::api::TRXServerType                 ;
using ApiSide                        = com::tradex::api::TRXSide                       ;
using ApiTradeReport                 = com::tradex::api::TRXTradeReport                ;
using ApiTradeUnitConnStatusNotice   = com::tradex::api::TRXTradeUnitConnStatusNotice  ;
using ApiText                        = com::tradex::api::text_t                        ;

// Export Api Types
using ApiTradeUnitID                 =  com::tradex::api::trade_unit_t                 ;
using ApiRequestID                   = com::tradex::api::request_id_t                  ;
using ApiPriceType                   = com::tradex::api::TRXPriceType                  ;
using ApiSide                        = com::tradex::api::TRXSide                       ;
using ApiMarket                      = com::tradex::api::TRXMarket                     ;
using ApiLoginRequest                = com::tradex::api::TRXLoginRequest               ;
using ApiAlgoParameters              = com::tradex::api::TRXAlgoParameters             ;
using ApiSingleOrder                 = com::tradex::api::TRXSingleOrder                ;
using ApiBasketOrder                 = com::tradex::api::TRXBasketOrder                ;
using ApiBasketLeg                   = com::tradex::api::TRXBasketLeg                  ;
using ApiBalanceQueryRequest         = com::tradex::api::TRXBalanceQueryRequest        ;
using ApiPositionQueryRequest        = com::tradex::api::TRXPositionQueryRequest       ;
using ApiOrderCancelRequest          = com::tradex::api::TRXOrderCancelRequest         ;
using ApiBasketCancelRequest         = com::tradex::api::TRXBasketCancelRequest        ;
using ApiTradeUnitStatusQueryRequest = com::tradex::api::TRXTradeUnitStatusQueryRequest;
using ApiTimestamp                   = com::tradex::api::timestamp_t                   ;
using ApiBasketID                    = com::tradex::api::basket_id_t                   ;

// Helper type defs
using ApiBalance                     = com::tradex::api::TRXBalance                    ;
using ApiBorrowingSecurity           = com::tradex::api::TRXBorrowingSecurity          ;
using ApiCreditAsset                 = com::tradex::api::TRXCreditAsset                ;
using ApiFinancingLiability          = com::tradex::api::TRXFinancingLiability         ;
using ApiLoginResponse               = com::tradex::api::TRXLoginResponse              ;
using ApiOrderCancelReject           = com::tradex::api::TRXOrderCancelReject          ;
using ApiOrderReport                 = com::tradex::api::TRXOrderReport                ;
using ApiOrderStatus                 = com::tradex::api::TRXOrderStatus                ;
using ApiPosition                    = com::tradex::api::TRXPosition                   ;
using ApiSecurityLiability           = com::tradex::api::TRXSecurityLiability          ;
using ApiServerType                  = com::tradex::api::TRXServerType                 ;
using ApiSide                        = com::tradex::api::TRXSide                       ;
using ApiTradeReport                 = com::tradex::api::TRXTradeReport                ;
using ApiTradeUnitConnStatusNotice   = com::tradex::api::TRXTradeUnitConnStatusNotice  ;

} /* namespace api     */
} /* namespace wct     */
