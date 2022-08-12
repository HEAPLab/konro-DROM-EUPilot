#ifndef NUMERICUTIL_H
#define NUMERICUTIL_H

#include "numericvalue.h"
#include <string>
#include <map>

namespace pc {

std::map<std::string, NumericValue> parseLineNv(const std::string &line, int major, int minor);

}   // namespace pc

#endif // NUMERICUTIL_H
