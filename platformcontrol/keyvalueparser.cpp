#include "keyvalueparser.h"
#include "pcexception.h"
#include <cctype>
#include <cstdlib>

using namespace std;

static inline const char *findTokenEnd(const char *pStart)
{
    while (isalnum(*pStart)) {
        ++pStart;
    }
    return pStart;
}

namespace pc {

bool KeyValueParser::parseMajorMinor(int major, int minor)
{
    if (!isdigit(*ptr_))
        throw PcException("Invalid line: digit expected for major");

    // Parse major device number

    int devMajor = strtol(ptr_, (char **)&endptr_, 10);
    if (*endptr_ != ':')
        throw PcException("Invalid line: ':' expected after major");
    if (devMajor != major)
        return false;         // not the device we are looking for

    // Parse minor device number

    ptr_ = endptr_ + 1;
    if (!isdigit(*ptr_))
        throw PcException("Invalid line: digit expected for minor");

    int devMinor = strtol(ptr_, (char **)&endptr_, 10);
    if (devMinor != minor)
        return false;

    return true;
}

pair<string, NumericValue> KeyValueParser::parseKeyValue()
{
    // parse the tag

    endptr_ = findTokenEnd(ptr_);
    if (endptr_ == ptr_) {
        throw PcException("Invalid line: alpha character expected for tag");
    }
    if (*endptr_ != '=' && *endptr_ != ':') {
        throw PcException("Invalid line: '=' or ':' expected after tag");
    }
    string tag(ptr_, endptr_);
    ptr_ = endptr_ + 1;

    // parse the value

    endptr_ = findTokenEnd(ptr_);
    if (endptr_ == ptr_) {
        throw PcException("Invalid line: alphanumeric character expected for value");
    }

    NumericValue val(ptr_, endptr_);
    if (val.isInvalid())
        throw PcException("Invalid line: invalid numeric value");
    return make_pair<>(tag, val);
}

map<string, NumericValue> KeyValueParser::parseLineNv(const char *line, int major, int minor)
{
    ptr_ = line;
    if (!parseMajorMinor(major, minor) || *endptr_ == '\0')
        return map<string, NumericValue>();         // not the device we are looking for or no tags

    if (*endptr_ != ' ')
        throw PcException("Invalid line: space expected after minor");

    return parseLineNv(endptr_ + 1);
}

map<string, NumericValue> KeyValueParser::parseLineNv(const char *line)
{
    map<string, NumericValue> tags;

    ptr_ = line;
    while (*ptr_) {
        tags.insert(parseKeyValue());
        ptr_ = endptr_;
        if (*ptr_) {
            if (*ptr_ != ' ')
                throw PcException("Invalid line: expected space character after value");
            ++ptr_;
            if (!*ptr_)
                throw PcException("Invalid line: expected new tag after last value");
        }
    }
    return tags;
}

}   // namespace pc
