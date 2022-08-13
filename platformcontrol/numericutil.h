#ifndef NUMERICUTIL_H
#define NUMERICUTIL_H

#include "numericvalue.h"
#include <string>
#include <map>

namespace pc {

std::map<std::string, NumericValue> parseLineNv(const char *line);
std::map<std::string, NumericValue> parseLineNv(const std::string &line, int major, int minor);
inline std::map<std::string, NumericValue> parseLineNv(const std::string &line) {
    return parseLineNv(line.c_str());
}

}   // namespace pc

#endif // NUMERICUTIL_H
