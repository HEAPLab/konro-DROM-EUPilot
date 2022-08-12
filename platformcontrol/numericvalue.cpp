#include "numericvalue.h"
#include <cstring>
#include <cstdlib>

namespace pc {

void NumericValue::init(const char *pStart, const char *pEnd)
{
    value_ = NUMERIC_VALUE_INVALID;
    if (!pStart)
        return;
    if (pEnd - pStart == 3 && strncmp(pStart, "max", 3) == 0) {
        value_ = NUMERIC_VALUE_MAX;
        return;
    }
    char *endptr;
    value_ = strtol(pStart, &endptr, 10);
    if (pEnd && endptr != pEnd) {
        value_ = NUMERIC_VALUE_INVALID;
    }
}

NumericValue::NumericValue(const char *pStart, const char *pEnd)
{
    init(pStart, pEnd);
}

NumericValue::NumericValue(const std::string &val)
{
    const char *pStart = val.c_str();
    const char *pEnd = pStart + val.size();
    init(pStart, pEnd);
}

}   // namespace pc
