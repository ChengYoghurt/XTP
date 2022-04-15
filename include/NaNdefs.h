/* NaNdefs.h
 * 
 * Author: Wentao Wu
 * Created: 20200414
 * Version: 0.1
*/

#pragma once

#include "NumericTime.h"

#include <cmath>

namespace strmagg {

    template<typename Value>
    struct GetNaN {
    };
    template<typename Value>
    inline bool isnan(const Value& v) {
        throw std::logic_error("Type not define NaN");
        return true;
    }

    template<>
    struct GetNaN<float> {
        static constexpr double value = std::numeric_limits<float>::quiet_NaN();
    };
    template<>
    inline bool isnan<float>(const float& v) {
        return std::isnan(v);
    }

    template<>
    struct GetNaN<double> {
        static constexpr double value = std::numeric_limits<double>::quiet_NaN();
    };
    template<>
    inline bool isnan<double>(const double& v) {
        return std::isnan(v);
    }

    template<>
    struct GetNaN<char> {
        static constexpr char value = '\0';
    };
    template<>
    inline bool isnan<char>(const char& v) { return v == '\0'; } ;

    template<>
    struct GetNaN<int> {
        static constexpr int value = std::numeric_limits<int>::min();
    };
    template<>
    inline bool isnan<int>(const int& v) {
        return v == GetNaN<int>::value;
    }

    template<>
    struct GetNaN<unsigned> {
        static constexpr unsigned value = std::numeric_limits<unsigned>::max() - 1;
    };
    template<>
    inline bool isnan<unsigned>(const unsigned& v) {
        return v == GetNaN<unsigned>::value;
    }

    template<>
    struct GetNaN<long> {
        static constexpr long value = std::numeric_limits<long>::min();
    };
    template<>
    inline bool isnan<long>(const long& v) {
        return v == GetNaN<long>::value;
    }

    template<>
    struct GetNaN<unsigned long> {
        static constexpr unsigned long value = std::numeric_limits<unsigned long>::max() - 1;
    };
    template<>
    inline bool isnan<unsigned long>(const unsigned long& v) {
        return v == GetNaN<unsigned long>::value;
    }

    template<>
    struct GetNaN<kf::NumericTime> {
        static const kf::NumericTime value;
    };
    template<>
<<<<<<< HEAD
    inline bool isnan<kf::NumericTime>(const kf::NumericTime& v) {
        return v == GetNaN<kf::NumericTime>::value;
=======
    inline bool isnan<NumericTime>(const kf::NumericTime& v) {
        return v == GetNaN<NumericTime>::value;
>>>>>>> 8f6e9e8c5afc702d7ea3e5fbb31a2a450222ea5a
    }
} // namespace strmagg