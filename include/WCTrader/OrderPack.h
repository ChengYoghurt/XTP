/* OrderPack.h
 * OrderPack class forms a group in mathematics under actions in OrderAction
 * 
 * Author: Wentao Wu
*/

#pragma once

#include "TraderTypes.h"

#include <variant>

namespace wct {

    struct OrderPlaceAction {
        // keys
        instrument_id_t instrument_id;
        side_t side;
        price_t limit_price;

        // values
        volume_t volume; 
    }; // class OrderAction
    struct OrderCancelAction {
        // keys
        OrderID order_id;
    }; // class OrderAction

    using OrderAction = std::variant< OrderPlaceAction, OrderCancelAction >;

    class OrderPack {
    public:
        using container_type         = std::vector<OrderAction>;
        using iterator               = typename container_type::iterator;
        using const_iterator         = typename container_type::const_iterator;
        using reverse_iterator       = typename container_type::reverse_iterator;
        using const_reverse_iterator = typename container_type::const_reverse_iterator;
        using size_type              = typename container_type::size_type;

        OrderPack& append(OrderAction const& action) {
            actions_.push_back(action);
            return *this;
        }
        OrderPack& append(OrderPack const& pack) {
            actions_.insert(actions_.end(), pack.cbegin(), pack.cend());
            return *this;
        }

        iterator                  begin()       { return actions_.begin()    ; }
        const_iterator           cbegin() const { return actions_.cbegin()   ; }
        iterator                    end()       { return actions_.end()      ; }
        const_iterator             cend() const { return actions_.cend()     ; }
        reverse_iterator         rbegin()       { return actions_.rbegin()   ; }
        const_reverse_iterator  crbegin() const { return actions_.crbegin()  ; }
        reverse_iterator           rend()       { return actions_.rend()     ; }
        const_reverse_iterator    crend() const { return actions_.crend()    ; }
        size_type                  size() const { return actions_.size()     ; }

        OrderAction& operator[](size_type index) {
            return actions_[index];
        }
        const OrderAction& operator[](size_type index) const {
            return actions_[index];
        }

    private:

        container_type actions_;

    }; // OrderPack

} // namespace rsg
