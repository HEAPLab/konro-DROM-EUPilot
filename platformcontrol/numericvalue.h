#ifndef NUMERICVALUE_H
#define NUMERICVALUE_H

#include <iostream>
#include <string>
#include <limits>

namespace pc {

/*!
 * The NumericValue class encapsulates an integral number
 * which can have the value "max"
 */
class NumericValue {
    static constexpr uint64_t NUMERIC_VALUE_MAX = std::numeric_limits<uint64_t>::max();
    static constexpr uint64_t NUMERIC_VALUE_INVALID = NUMERIC_VALUE_MAX - 1;
    uint64_t value_;

    void init(const char *pStart, const char *pEnd);
public:
    /*!
     * \brief Initializes NumericValue to default of "max"
     * \param value the initial value
     */
    NumericValue(uint64_t value = NUMERIC_VALUE_INVALID) : value_(value) {}

    /*!
     * \brief Initializes a NumericValue from a (part of a) C string
     * \param pStart Start of the numeric value
     * \param pEnd End of the numeric value (i.e. points to first
     *             character after the numeric value).
     *             nullptr means that the string is asciiz.
     */
    NumericValue(const char *pStart, const char *pEnd = nullptr);

    /*!
     * \brief Initializes a NumericValue from a C++ string
     * \param val The numeric value
     */
    NumericValue(const std::string &val);

    /*!
     * \brief Factory function to get a "max" value
     * \return A NumericValue initialized to max
     */
    static NumericValue max() {
        return NumericValue(NUMERIC_VALUE_MAX);
    }

    bool isMax() const { return value_ == NUMERIC_VALUE_MAX; }
    bool isInvalid() const { return value_ == NUMERIC_VALUE_INVALID; }
    void set(uint64_t value = NUMERIC_VALUE_MAX) { value_ = value; }
    void setMax() { value_ = NUMERIC_VALUE_MAX; }
    void setInvalid() { value_ = NUMERIC_VALUE_INVALID; }

    operator uint64_t() {
        return static_cast<uint64_t>(value_);
    }

    friend std::ostream &operator <<(std::ostream &os, const NumericValue &num) {
        if (num.value_ == NUMERIC_VALUE_INVALID)
            os << "invalid";
        else if (num.value_ == NUMERIC_VALUE_MAX)
            os << "max";
        else
            os << num.value_;
        return os;
    }

    friend std::istream &operator >>(std::istream &is, NumericValue &num) {
        num.set(NUMERIC_VALUE_INVALID);
        std::string s;
        if (is >> s) {
            if (s == "max")
                num.set(NUMERIC_VALUE_MAX);
            else
                num.set(strtoull(s.c_str(), nullptr, 10));
        }
        return is;
    }
};

}   // namespace pc

#endif // NUMERICVALUE_H
