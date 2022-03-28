#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace wct {

// Error Definitions
enum class error_id_t {
    success                 =    0,
    unknown                 =   -1,
    not_login               = 1001,
    login_timeout           = 1002,
    not_connected_to_server = 1003,
    fail_authentication     = 1004,
    wrong_client_order_id   = 2001,
    wrong_instrument_id     = 2002,
    wrong_market_id         = 2003,
    wrong_request_id        = 2004,
    too_freq_query          = 3001,
    too_freq_trade          = 3002,
    cancel_after_traded     = 4001,
    not_implemented         = 9001,
};

static std::unordered_map<error_id_t, std::string> error_defs = {
    { error_id_t::unknown                , "unknown                " },
    { error_id_t::not_login              , "not_login              " },
    { error_id_t::login_timeout          , "login_timeout          " },
    { error_id_t::not_connected_to_server, "not_connected_to_server" },
    { error_id_t::fail_authentication    , "fail_authentication    " },
    { error_id_t::wrong_client_order_id  , "wrong_client_order_id  " },
    { error_id_t::wrong_instrument_id    , "wrong_instrument_id    " },
    { error_id_t::wrong_market_id        , "wrong_market_id        " },
    { error_id_t::wrong_request_id       , "wrong_request_id       " },
    { error_id_t::too_freq_query         , "too_freq_query         " },
    { error_id_t::too_freq_trade         , "too_freq_trade         " },
};

inline std::string error_message(error_id_t error_id) {
    auto iter = error_defs.find(error_id);
    if(iter == error_defs.end()) {
        return "Unknown error_id";
    }
    return iter->second;
}

} /* namespace wct */
