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

/**
 * \brief Parses a line in the format "major:minor ..."
 * \param major Major device number
 * \param minor Minor device number
 * \return nullptr if major and minor of the line are different from the
 *         requested values, otherwise a pointer to the first character
 *         after the minor number
 * \exception PcException if the format of the line is invalid
 */
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

/*!
 * Parses a key-value pair, such as key=value or key:value
 *
 * \return The key-value pair
 * \exception PcException in case of format error
 */
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

/*!
 * Parses a line in the format
 * \code
 * major:minor tag=value [tag=value ...]
 * \endcode
 * where value can be numeric or the string "max"
 *
 * \param line the line to parse
 * \param major expected device major number
 * \param minor expected device minor number
 * \return map of key and values. If the major or minor device numbers
 *         of the line are different from the requested major and minor
 *         numbers, the map is empty
 * \exception PcException if the format of the line is invalid
 */
map<string, NumericValue> KeyValueParser::parseLineNv(const char *line, int major, int minor)
{
    ptr_ = line;
    if (!parseMajorMinor(major, minor))
        return map<string, NumericValue>();         // not the device we are looking for

    if (*endptr_ != ' ')
        throw PcException("Invalid line: space expected after minor");

    return parseLineNv(endptr_ + 1);
}

/*!
 * Parses a line in the format
 * \code
 * tag=value [tag=value ...]
 * \endcode
 * where value can be numeric or the string "max"
 *
 * \param line the line to parse
 * \return map of key and values
 * \exception PcException if the format of the line is invalid
 */
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
