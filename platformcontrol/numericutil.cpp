#include "numericutil.h"
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
 * @brief Parses a line in the format "major:minor ..."
 * @param p The line
 * @param major Major device number
 * @param minor Minor device number
 * @return nullptr if major and minor of the line are different from the
 *         requested values, otherwise a pointer to the first character
 *         after the minor number
 * \exception PcException if the format of the line is invalid
 */
static const char *parseMajorMinor(const char *p, int major, int minor)
{
    const char *endptr;

    if (!isdigit(*p))
        throw PcException("Invalid line: digit expected for major");

    // Parse major device number

    int devMajor = strtol(p, (char **)&endptr, 10);
    if (*endptr != ':')
        throw PcException("Invalid line: ':' expected after major");
    if (devMajor != major)
        return nullptr;         // not the device we are looking for

    // Parse minor device number

    p = endptr + 1;
    if (!isdigit(*p))
        throw PcException("Invalid line: digit expected for minor");

    int devMinor = strtol(p, (char **)&endptr, 10);
    if (devMinor != minor)
        return nullptr;         // not the device we are looking for

    return endptr;
}

static pair<string, NumericValue> parseKeyValue(const char *ptr, const char **endptr)
{
    // parse the tag

    const char *eptr = findTokenEnd(ptr);
    if (eptr == ptr) {
        throw PcException("Invalid line: alpha character expected for tag");
    }
    if (*eptr != '=' && *eptr != ':') {
        throw PcException("Invalid line: '=' or ':' expected after tag");
    }
    string tag(ptr, eptr);
    ptr = eptr + 1;

    // parse the value

    eptr = findTokenEnd(ptr);
    if (eptr == ptr) {
        throw PcException("Invalid line: alphanumeric character expected for value");
    }

    NumericValue val(ptr, eptr);
    if (val.isInvalid())
        throw PcException("Invalid line: invalid numeric value");

    if (endptr != nullptr)
        *endptr = eptr;
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
map<string, NumericValue> parseLineNv(const string &line, int major, int minor)
{
    map<string, NumericValue> tags;
    const char *p0 = line.c_str();
    const char *p = p0;
    const char *endptr;

    endptr = parseMajorMinor(p, major, minor);
    if (endptr == nullptr)
        return tags;         // not the device we are looking for

    if (*endptr != ' ')
        throw PcException("Invalid line: space expected after minor");

    return parseLineNv(endptr + 1);
}

map<string, NumericValue> parseLineNv(const char *ptr)
{
    map<string, NumericValue> tags;
    const char *endptr;

    while (*ptr) {
        tags.insert(parseKeyValue(ptr, &endptr));
        ptr = endptr;
        if (*ptr) {
            if (*ptr != ' ')
                throw PcException("Invalid line: expected space character after value");
            ++ptr;
            if (!*ptr)
                throw PcException("Invalid line: expected new tag after last value");
        }
    }
    return tags;
}

}   // namespace pc
