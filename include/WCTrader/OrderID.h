/* OrderID.h
 * order id numeric system 
 * 
 * Author: Wentao Wu
*/
#pragma once

#include "NumericTime.h"
#include <mutex>

namespace wct {
    using NumericTime = kf::NumericTime;

    enum ChildFlag: uint32_t {
        SingleOrder     = 0,
        ParentOrder     = 1,
        ChildOrder      = 2,
        BasketLegOrder  = 4,
    };

    class  OrderID;
    struct OrderInfo;
    class  OrderIDGenerator;

    // OrderID encoding
    // safe int max  : 922'337203685'4'775'807
    // order encoding: 000'HHMMSSBBB'0'001'001
    // order meaning : trader_id'timestamp'child_status'child_id'order_id
    // timestamp: HH:MM:SS.BBB
    // child_flag:
    //   0: no children, single order
    //   1: has children, parent order
    //   2: is child order
    //   4: is a basket leg
    // child_id:
    //   0000: if child_status != ChildOrder
    //   0001: first child order id
    // order_id:
    //   0001: first order id
    struct OrderInfo {
        uint32_t             trader_id ;
        NumericTime          timestamp ;
        ChildFlag            child_flag;
        uint32_t             child_id  ;
        uint32_t             order_id  ;

        uint64_t encode() const noexcept {
            return   static_cast<uint64_t>(trader_id ) * 1'000000'000'0'000'000
                   + static_cast<uint64_t>(timestamp ) *            1'0'000'000
                   + static_cast<uint64_t>(child_flag) *              1'000'000
                   + static_cast<uint64_t>(child_id  ) *                  1'000
                   + static_cast<uint64_t>(order_id  );
        }
    };

    class OrderID {
    public:
        using timestamp_t = NumericTime;

        OrderID(uint64_t const& id = 0) : id_(id) { }
        operator uint64_t() const { return id_; }

        OrderInfo decode() const {
            uint64_t order_id = id_;
            OrderInfo id_info;
            id_info.order_id = order_id % 1'000;
            order_id /= 1'000;
            id_info.child_id = order_id % 1'000;
            order_id /= 1'000;
            id_info.child_flag = static_cast<ChildFlag>(order_id % 10);
            order_id /= 10;
            id_info.timestamp = timestamp_t( order_id % 1'000000'000 );
            order_id /= 1'000000'000;
            id_info.trader_id = order_id;
            return id_info;
        }

        bool is_single    () const {
            return get_child_flag() == ChildFlag::SingleOrder;
        }
        bool is_parent    () const {
            return get_child_flag() == ChildFlag::ParentOrder;
        }
        bool is_child     () const {
            return get_child_flag() == ChildFlag::ChildOrder;
        }
        bool is_basket_leg() const {
            return get_child_flag() == ChildFlag::BasketLegOrder;
        }
        OrderID get_parent() const {
            OrderInfo order_info = decode();
            order_info.child_flag = ChildFlag::ParentOrder;
            order_info.child_id = 0;
            return order_info.encode();
        }
    
        bool is_belonged(OrderID const& parent_id) const {
            return get_parent() == parent_id;
        }
        bool is_siblings(OrderID const& child_rhs) const {
            return get_parent() == child_rhs.get_parent();
        }
        bool is_from_trader(uint32_t trader_id) const {
            return id_ / 1'000000'000'0'000'000 == trader_id;
        }
    private:
        ChildFlag get_child_flag() const {
            uint64_t child_flag = id_ / 1'000'000;
            child_flag %= 10;
            return static_cast<ChildFlag>(child_flag);
        }
    private:
        uint64_t id_;
    };

    class OrderIDGenerator {
    public:
        using timestamp_t = typename OrderID::timestamp_t;
        OrderIDGenerator(uint32_t trader_id)
            : trader_id_(trader_id)
            , curr_uniq_id_(0)
        {}

        OrderID create_single() {
            timestamp_t timestamp = timestamp_t::now();
            uint32_t uniq_id;
            {
                std::lock_guard<std::mutex> lk(mtx_);
                curr_uniq_id_ = (curr_uniq_id_ + 1) % 1000;
                uniq_id = curr_uniq_id_;
            }
            return static_cast<uint64_t>(trader_id_) * 1'000000'000'0'000'000 
                 + static_cast<uint64_t>(timestamp ) *            1'0'000'000 
                 + static_cast<uint64_t>(uniq_id   );
        }
        OrderID create_parent() {
            timestamp_t timestamp = timestamp_t::now();
            uint32_t uniq_id;
            {
                std::lock_guard<std::mutex> lk(mtx_);
                curr_uniq_id_ = (curr_uniq_id_ + 1) % 1000;
                uniq_id = curr_uniq_id_;
            }
            return static_cast<uint64_t>(trader_id_            ) * 1'000000'000'0'000'000 
                 + static_cast<uint64_t>(timestamp             ) *            1'0'000'000 
                 + static_cast<uint64_t>(ChildFlag::ParentOrder) *              1'000'000
                 + static_cast<uint64_t>(uniq_id);

        }

        // throw if prev_child is not a valid child_id or parent_id
        // pass parent_id if first child
        OrderID create_child (OrderID const& prev_child, ChildFlag child_flag = ChildFlag::ChildOrder) {
            OrderInfo id_info = prev_child.decode();

            id_info.child_flag = child_flag;
            id_info.child_id = (id_info.child_id + 1) % 1000;
            return id_info.encode();
        }

    private:
        mutable std::mutex mtx_;
        uint32_t trader_id_;
        uint32_t curr_uniq_id_;
    };

} // namespace rsg

namespace std
{
    template<> struct hash<wct::OrderID>
    {
        std::size_t operator()(wct::OrderID const& id) const noexcept {
            return static_cast<uint64_t>(id);
        }
    };
}

