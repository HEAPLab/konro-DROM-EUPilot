#ifndef KEYVALUEPARSER_H
#define KEYVALUEPARSER_H

#include "numericvalue.h"
#include <string>
#include <map>
#include <utility>

namespace pc {

/*!
 * \brief The KeyValueParser class
 *
 * Parses lines in one of the following formats:
 * \code
 * key=value
 * key:value
 *
 * key=value key=value [key=value ...]
 * key:value key:value [key:value ...]
 *
 * major:minor key=value [key=value ...]
 * major:minor key:value key:value [key:value ...]
 * \endcode
 */
class KeyValueParser {
    const char *ptr_, *endptr_;

    bool parseMajorMinor(int major, int minor);
    std::pair<std::string, NumericValue> parseKeyValue();

public:
    std::map<std::string, NumericValue> parseLineNv(const char *line);
    std::map<std::string, NumericValue> parseLineNv(const std::string &line) {
        return parseLineNv(line.c_str());
    }

    std::map<std::string, NumericValue> parseLineNv(const char *line, int major, int minor);
    std::map<std::string, NumericValue> parseLineNv(const std::string &line, int major, int minor) {
        return parseLineNv(line.c_str(), major, minor);
    }

    std::pair<std::string, NumericValue> parseKeyValue(const char *line) {
        ptr_ = line;
        return parseKeyValue();
    }

    std::pair<std::string, NumericValue> parseKeyValue(const std::string &line) {
        return parseKeyValue(line.c_str());
    }
};

}   // namespace pc

#endif // KEYVALUEPARSER_H
