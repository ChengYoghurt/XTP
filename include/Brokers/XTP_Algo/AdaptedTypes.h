#pragma once

#include "xtp_algo_trader_api.h"

namespace wct     {
namespace api     {

using BrokerApi                      = XTP::API::TraderApi                     ;
using BrokerSpi                      = XTP::API::TraderSpi                     ;

// Export Spi Types
//using ApiBalance                     = XTPQueryAssetRsp                        ; //asset include balance   
//using ApiBorrowingSecurity           = com::tradex::api::TRXBorrowingSecurity          ;
//using ApiCreditAsset                 = com::tradex::api::TRXCreditAsset                ;
//using ApiFinancingLiability          = com::tradex::api::TRXFinancingLiability         ;
//using ApiLoginResponse               = com::tradex::api::TRXLoginResponse              ;
using ApiOrderCancelReport           = XTPStrategyInfoStruct                   ;
using ApiOrderReport                 = XTPStrategyStateReportStruct            ;
using ApiOrderStatus                 = XTPStrategyStateType                    ;
using ApiPosition                    = XTPQueryStkPositionRsp                  ;
using ApiInsertReport                = XTPStrategyInfoStruct                   ;   
//using ApiSecurityLiability           = com::tradex::api::TRXSecurityLiability          ;
//using ApiServerType                  = com::tradex::api::TRXServerType                 ;
//using ApiSide                        = com::tradex::api::TRXSide                       ;
//using ApiTradeReport                 = XTPTradeReport                          ;
//using ApiTradeUnitConnStatusNotice   = com::tradex::api::TRXTradeUnitConnStatusNotice  ;
using ApiText                        = XTPRI                                   ;
//using ApiQureryTradeReport           = XTPQueryTradeRsp;
//using ApiQureryOrderReport           = XTPOrderInfoEx 
// Export Api Types

//using ApiTradeUnitID                 = com::tradex::api::trade_unit_t                  ;
using ApiRequestID                   = int                                     ;
using ApiPriceType                   = XTP_PRICE_TYPE                          ;
using ApiSide                        = XTP_SIDE_TYPE                           ;
using ApiMarket                      = XTP_MARKET_TYPE                         ;
using ApiExchange                    = XTP_EXCHANGE_TYPE                       ;
using ApiBusiness                    = XTP_BUSINESS_TYPE                       ;
//using ApiLoginRequest                = com::tradex::api::TRXLoginRequest               ;
//using ApiAlgoParameters              = com::tradex::api::TRXAlgoParameters             ;

using ApiSingleOrder                 = XTPOrderInsertInfo                      ;
using ApiLogLevel                    = XTP_LOG_LEVEL                           ;
/*
using ApiBasketOrder                 = com::tradex::api::TRXBasketOrder                ;
using ApiBasketLeg                   = com::tradex::api::TRXBasketLeg                  ;
using ApiBalanceQueryRequest         = com::tradex::api::TRXBalanceQueryRequest        ;
using ApiPositionQueryRequest        = com::tradex::api::TRXPositionQueryRequest       ;
using ApiOrderCancelRequest          = com::tradex::api::TRXOrderCancelRequest         ;
using ApiBasketCancelRequest         = com::tradex::api::TRXBasketCancelRequest        ;
using ApiTradeUnitStatusQueryRequest = com::tradex::api::TRXTradeUnitStatusQueryRequest;
using ApiTimestamp                   = com::tradex::api::timestamp_t                   ;
using ApiBasketID                    = com::tradex::api::basket_id_t                   ;
*/
// Helper type defs
/*
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
*/
} /* namespace api     */
} /* namespace wct     */
