/* TypeDefs.h
 * 
 * Author: Fudan CS Group
 * Created: 20220222
 * Version: 0.1
*/

#pragma once

#include "NumericTime.h"

#include <array>
#include <cstdint>

namespace kf {

    //===========================================
    // Basic Trade Types
    //===========================================
    using session_t       = uint32_t            ;
    using request_id_t    = uint64_t            ;
    using instrument_id_t = uint32_t            ; 
    using decimal_t       = uint64_t            ;
    using price_t         = double              ;  // price = decimal / 10000.0
    using volume_t        = uint64_t            ; 
    using timestamp_t     = NumericTime         ;
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

}
