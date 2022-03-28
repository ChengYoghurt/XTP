/* TraderTypes.h
 * 
 * Author: Wentao Wu
*/

#pragma once

#include "NumericTime.h"
#include "OrderID.h"
#include "ErrorMessage.h"
#include <unordered_map>
#include <vector>

namespace wct {

    //===========================================
    // Basic Trade Types
    //===========================================
    using basket_id_t     = OrderID             ;
    using order_id_t      = OrderID             ;
    using session_t       = uint32_t            ;
    using request_id_t    = uint64_t            ;
    using trade_unit_t    = uint32_t            ;
    using instrument_id_t = uint32_t            ; 
    using decimal_t       = uint64_t            ;
    using price_t         = double              ;  // price = decimal / 10000.0
    using volume_t        = uint64_t            ; 
    using timestamp_t     = kf::NumericTime     ;
    using trade_sn_t      = std::array<char, 64>;
    using message_t       = std::array<char, 64>;
    using account_name_t  = std::array<char, 64>;
    using millisec_t      = uint8_t             ; // N in [0, 999]
    using ratio_t         = double              ;

    enum class side_t {
        buy  = 'B',
        sell = 'S',
    };

    enum class market_t { 
        unknown  = 'U',
        sh       = '6', 
        shsecond = '8', 
        sz       = '0', 
        szsecond = '3',
        bj       = 'B',
    };

    enum class order_status_t {
        unknown   = 'U',
        created   = '0',
        accepted  = 'A',
        rejected  = 'R',
        canceled  = 'C',
        completed = 'K',
    };

    enum class price_type_t {
        unknown         = 'U',     // Unknown Price Type
        limit           = 'L',     // limit orders
        oppo_side_best  = 'O',     // opposite side best
        same_side_best  = 'S',     // same side best
        best5_or_cancel = '5',     // Five Best Orders Immediate or Cancel
        fill_and_kill   = 'I',     // Immediate or Cancel
        fill_or_kill    = 'A',     // All or Cancel
    };

    //===========================================
    // Acount Infomation Types
    //===========================================
    struct HoldingInfo {
        volume_t holding;
        volume_t available;
    };
    using PositionInfo = std::unordered_map<instrument_id_t, HoldingInfo>;

    struct BalanceInfo {
        price_t  initial_balance   ;
        price_t  available_balance ;
        price_t  market_value      ;
        price_t  total_asset       ;
    };

    struct AccountStatus {
        bool is_logined         ;
        bool is_query_completed ;
        bool has_untraded_orders;
    };

    struct AccountInfo {
        uint16_t        trade_id   ;
        PositionInfo    holdings   ;
        BalanceInfo     balance    ;
        AccountStatus   status     ;
    };

    //===========================================
    // WC Trade API Types
    //===========================================
    struct WCAgentPCFingerprint {
        std::string token    ; // software key, certification, etc.
        std::string mac      ;
        std::string cpu      ;
        std::string disk     ;
        std::string local_ip ;
    };

    struct WCLoginRequest {
        std::string  username    ;
        std::string  password    ;
        std::string  server_ip   ;
        uint32_t     server_port ;
        WCAgentPCFingerprint agent_fingerprint;
    };

    struct WCLoginResponse {
        session_t       session_id ;
        error_id_t      error_id   ;
    };

    struct WCOrderRequest {
        order_id_t      client_order_id ;
        instrument_id_t instrument      ;
        market_t        market          ;
        price_t         price           ;
        volume_t        volume          ;
        side_t          side            ;
        price_type_t    price_type      ;
    };

    struct WCAlgoOrderRequest {
        WCOrderRequest  order      ;
        std::string     algo_name  ;
    };

    struct WCOrderCancelRequest {
        order_id_t client_order_id;
    };

    struct WCOrderResponse {
        order_id_t      client_order_id ;

        instrument_id_t instrument      ;
        volume_t        volume          ;  // placed volume of order
        price_t         price           ;
        volume_t        traded          ;  // volume been traded
        price_t         average_price   ;

        order_status_t  order_status    ;

        timestamp_t     transaction_time;  // updated time
        timestamp_t     host_time       ;

        error_id_t      error_id        ;
    };

    struct WCTradeResponse {
        order_id_t      client_order_id ;

        instrument_id_t instrument      ;
        volume_t        trade_volume    ;
        price_t         trade_price     ;

        timestamp_t     transaction_time;
        timestamp_t     host_time       ;

        error_id_t      error_id        ;
    };

    struct WCCancelRejectedResponse {
        order_id_t client_order_id;
        error_id_t error_id       ;
    };

    //===========================================
    // WC Query API Types
    //===========================================
    struct WCPositionQueryRequest {
        bool query_all;
        instrument_id_t instrument;
    };

    struct WCEmptyResponse { };

    struct WCPositionResponse {
        instrument_id_t instrument       ;
        volume_t        yesterday_volume ;
        volume_t        latest_volume    ;
        volume_t        available_volume ;
        bool            is_last          ;
        error_id_t      error_id         ;
    };

    struct WCBalanceResponse {
        price_t     initial_balance   ;
        price_t     available_balance ;
        price_t     market_value      ;
        price_t     total_asset       ;
        error_id_t  error_id          ;
    };

    struct WCCreditBalanceResponse {
        ratio_t     maintainance_ratio;  // ratio of colateral for maintanace
        price_t     total_asset       ;
        price_t     total_debt        ;
        price_t     line_of_credit    ;
        price_t     available_margin  ;
        error_id_t  error_id          ;
    };

    //===========================================
    // WC Algorithm Basket API Types
    //===========================================
    struct WCBasketOrderRequest {
        order_id_t  client_basket_id   ;
        timestamp_t start_time         ;
        timestamp_t end_time           ;
        std::string algo_name          ;
        std::vector<WCOrderRequest> basket_legs;
    };

    struct WCBasketOrderCancelRequest {
        order_id_t client_order_id;
    };

} /* namespace wct */
