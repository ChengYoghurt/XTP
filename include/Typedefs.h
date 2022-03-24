/* Typedefs.h
 * 
 * Author: Wentao Wu
*/

#pragma once

#include "NumericTime.h"
#include "NaNdefs.h"
#include <cstddef>
#include <vector>
#include <array>

namespace l2agg {

    // ============================================================//
    //                     CONSTANT DEFINITIONS                    //
    // ============================================================//
    static constexpr size_t k_max_instrument = 4096;
    static constexpr size_t k_max_exchange   =    8;
    static constexpr double k_eps            = 1e-8;
    static constexpr double valid_price_min  = 0.01;

    using price_t                    = double               ;
    using volume_t                   = int                  ; 
    using large_volume_t             = double               ;
    using order_volume_t             = int                  ; 
    using count_t                    = int                  ; 
    using trade_duration_t           = double               ; 
    using timestamp_t                = kf::NumericTime      ; 
    using millisec_t                 = int                  ; 

    using instrument_str_id_t        = std::array<char, 8>  ;
    using instrument_id_t            = int                  ;
    using exchange_str_id_t          = std::array<char, 8>  ; 
    using exchange_id_t              = int                  ; 
    using strategy_id_t              = int                  ;
    using ratio_t                    = double               ;
    using local_order_serial_id_t    = int                  ; 
    using tick_timestamp_t           = int                  ;
    using tick_duration_t            = int                  ;
    using bar_timestamp_t            = int                  ;
    using error_id_t                 = int                  ;
    using error_msg_t                = std::array<char, 16> ;
    using status_msg_t               = std::array<char, 16> ;
    using string_date_t              = std::array<char, 16> ; // 2019-01-01

    using request_id_t               = int                  ;
    using front_id_t                 = int                  ;
    using session_id_t               = int                  ;

    using seq_id_t                   = int                  ;
    using channel_id_t               = int                  ;
    using seq_id_t                   = int                  ;
    using biz_index_t                = int64_t              ;
    using flag_t                     = char                 ;

    using slice_index_t              = int                  ;

    // Transaction Data 
    using Transaction = std::tuple<
        seq_id_t    , // SeqNo;       // unique in one channel
        channel_id_t, // Channel;
        timestamp_t , // TransactionTime;
        price_t     , // Price;
        volume_t    , // Volume;
        price_t     , // Amount;      // SH only
        flag_t      , // BSFlag;      // SH only 'B' for active buy and 'S' for active sell, 'N' for unknown
        flag_t      , // CancelFlag;  // SZ only '4' for cancel, 'F' for trade
        seq_id_t    , // BidOrderSeqNo;
        seq_id_t    , // AskOrderSeqNo;
        biz_index_t , // business index;
        timestamp_t   // HostTime
    >;

    struct TransactionField { 
        using field_index_t = std::size_t;
        constexpr static field_index_t SeqNo           = 0;       // unique in one channel
        constexpr static field_index_t Channel         = 1;
        constexpr static field_index_t TransactionTime = 2;
        constexpr static field_index_t Price           = 3;
        constexpr static field_index_t Volume          = 4;
        constexpr static field_index_t Amount          = 5;      // SH only
        constexpr static field_index_t BSFlag          = 6;      // SH only 'B' for active buy and 'S' for active sell, 'N' for unknown 
        constexpr static field_index_t CancelFlag      = 7;      // SZ only '4' for cancel, 'F' for trade
        constexpr static field_index_t BidOrderSeqNo   = 8;
        constexpr static field_index_t AskOrderSeqNo   = 9;
        constexpr static field_index_t BizIndex        = 10;
        constexpr static field_index_t HostTime        = 11;
    };

    // Order Data
    using Order = std::tuple<
        seq_id_t     , // SeqNo;      // unique in one channel
        channel_id_t , // ChannelNo;
        timestamp_t  , // TransactionTime;
        price_t      , // Price;
        volume_t     , // Volume;
                       // SH: remained untraded limit order volume
                       // SZ: real order volume
        flag_t       , // BSFlag;
                       // (B): Buy
                       // (S): Sell
        flag_t       , // OrderType; 
                       // SH: (A) add order
                       //     (D) delete order
                       // SZ: (0) limit order
                       //     (1) market order
                       //     (2) optimal price order 本方最优价格
        seq_id_t     , // order origin number, match (BidOrderSeqNo|AskOrderSeqNo) in transaction
        biz_index_t  , // business index
        timestamp_t    // host timestamp
    >;
    struct OrderField {
        using field_index_t = std::size_t;
        constexpr static field_index_t SeqNo            = 0; 
        constexpr static field_index_t ChannelNo        = 1;
        constexpr static field_index_t TransactionTime  = 2;
        constexpr static field_index_t Price            = 3;
        constexpr static field_index_t Volume           = 4;
        constexpr static field_index_t BSFlag           = 5;
        constexpr static field_index_t OrderType        = 6;
        constexpr static field_index_t OriginOrderSeqNo = 7;
        constexpr static field_index_t BizIndex         = 8;
        constexpr static field_index_t HostTime         = 9;
    };

    // DepthMarket Data
    using DepthMarket = std::tuple<
        timestamp_t, // Depth Time;
        int        , // Status    ;
        price_t    , // PreClose  ;
        price_t    , // Open      ;
        price_t    , // High      ;
        price_t    , // Low       ;
        price_t    , // Close     ;
        price_t    , // AskPrice1 ;
        price_t    , // AskPrice2 ;
        price_t    , // AskPrice3 ;
        price_t    , // AskPrice4 ;
        price_t    , // AskPrice5 ;
        price_t    , // AskPrice6 ;
        price_t    , // AskPrice7 ;
        price_t    , // AskPrice8 ;
        price_t    , // AskPrice9 ;
        price_t    , // AskPrice10;
        volume_t   , // AskVol1   ;
        volume_t   , // AskVol2   ;
        volume_t   , // AskVol3   ;
        volume_t   , // AskVol4   ;
        volume_t   , // AskVol5   ;
        volume_t   , // AskVol6   ;
        volume_t   , // AskVol7   ;
        volume_t   , // AskVol8   ;
        volume_t   , // AskVol9   ;
        volume_t   , // AskVol10  ;
        price_t    , // BidPrice1 ;
        price_t    , // BidPrice2 ;
        price_t    , // BidPrice3 ;
        price_t    , // BidPrice4 ;
        price_t    , // BidPrice5 ;
        price_t    , // BidPrice6 ;
        price_t    , // BidPrice7 ;
        price_t    , // BidPrice8 ;
        price_t    , // BidPrice9 ;
        price_t    , // BidPrice10;
        volume_t   , // BidVol1   ;
        volume_t   , // BidVol2   ;
        volume_t   , // BidVol3   ;
        volume_t   , // BidVol4   ;
        volume_t   , // BidVol5   ;
        volume_t   , // BidVol6   ;
        volume_t   , // BidVol7   ;
        volume_t   , // BidVol8   ;
        volume_t   , // BidVol9   ;
        volume_t   , // BidVol10  ;
        count_t    , // Trades(for validation);
        volume_t   , // Volume(for validation);
        price_t    , // Turnover    ;
        volume_t   , // TotalBidVol ;
        volume_t   , // TotalAskVol ;
        price_t    , // WeightedAvgBidPrice;
        price_t    , // WeightedAvgAskPrice;
        price_t    , // HighLimit ;
        price_t    , // LowLimit  ;
        timestamp_t // host timestamp
    >;

    struct DepthMarketField {
        using field_index_t = std::size_t;
        constexpr static field_index_t DepthMarketTime     = 0 ;
        constexpr static field_index_t Status              = 1 ;
        constexpr static field_index_t PreClose            = 2 ;
        constexpr static field_index_t Open                = 3 ;
        constexpr static field_index_t High                = 4 ;
        constexpr static field_index_t Low                 = 5 ;
        constexpr static field_index_t Close               = 6 ;
        constexpr static field_index_t AskPrice1           = 7 ;
        constexpr static field_index_t AskPrice2           = 8 ;
        constexpr static field_index_t AskPrice3           = 9 ;
        constexpr static field_index_t AskPrice4           = 10;
        constexpr static field_index_t AskPrice5           = 11;
        constexpr static field_index_t AskPrice6           = 12;
        constexpr static field_index_t AskPrice7           = 13;
        constexpr static field_index_t AskPrice8           = 14;
        constexpr static field_index_t AskPrice9           = 15;
        constexpr static field_index_t AskPrice10          = 16;
        constexpr static field_index_t AskVol1             = 17;
        constexpr static field_index_t AskVol2             = 18;
        constexpr static field_index_t AskVol3             = 19;
        constexpr static field_index_t AskVol4             = 20;
        constexpr static field_index_t AskVol5             = 21;
        constexpr static field_index_t AskVol6             = 22;
        constexpr static field_index_t AskVol7             = 23;
        constexpr static field_index_t AskVol8             = 24;
        constexpr static field_index_t AskVol9             = 25;
        constexpr static field_index_t AskVol10            = 26;
        constexpr static field_index_t BidPrice1           = 27;
        constexpr static field_index_t BidPrice2           = 28;
        constexpr static field_index_t BidPrice3           = 29;
        constexpr static field_index_t BidPrice4           = 30;
        constexpr static field_index_t BidPrice5           = 31;
        constexpr static field_index_t BidPrice6           = 32;
        constexpr static field_index_t BidPrice7           = 33;
        constexpr static field_index_t BidPrice8           = 34;
        constexpr static field_index_t BidPrice9           = 35;
        constexpr static field_index_t BidPrice10          = 36;
        constexpr static field_index_t BidVol1             = 37;
        constexpr static field_index_t BidVol2             = 38;
        constexpr static field_index_t BidVol3             = 39;
        constexpr static field_index_t BidVol4             = 40;
        constexpr static field_index_t BidVol5             = 41;
        constexpr static field_index_t BidVol6             = 42;
        constexpr static field_index_t BidVol7             = 43;
        constexpr static field_index_t BidVol8             = 44;
        constexpr static field_index_t BidVol9             = 45;
        constexpr static field_index_t BidVol10            = 46;
        constexpr static field_index_t Trades              = 47;
        constexpr static field_index_t Volume              = 48;
        constexpr static field_index_t Turnover            = 49;
        constexpr static field_index_t TotalBidVol         = 50;
        constexpr static field_index_t TotalAskVol         = 51;
        constexpr static field_index_t WeightedAvgBidPrice = 52;
        constexpr static field_index_t WeightedAvgAskPrice = 53;
        constexpr static field_index_t HighLimit           = 54;
        constexpr static field_index_t LowLimit            = 55;
        constexpr static field_index_t HostTime            = 56;
    };

    struct DepthMarketField dmfeild;

    struct StreamFlags {
        constexpr static char BuyTransFlag    = 'B';
        constexpr static char SellTransFlag   = 'S';
        constexpr static char BuyOrderFlag    = 'B';
        constexpr static char SellOrderFlag   = 'S';
        constexpr static char CancelTransFlag = 'C';
        constexpr static char TradedTransFlag = 'F';
    };
    

} // namespace l2agg 