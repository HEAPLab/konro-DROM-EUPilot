#include "numericutil.h"
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
 * \exception runtime_error if the format of the line is invalid
 */
map<string, NumericValue> parseLineNv(const string &line, int major, int minor)
{
    map<string, NumericValue> tags;
    const char *p0 = line.c_str();
    const char *p = p0;
    const char *endptr;

    if (!isdigit(*p))
        throw runtime_error("Invalid line: digit expected for major");

    // Parse major device number

    int devMajor = strtol(p, (char **)&endptr, 10);
    if (*endptr != ':')
        throw runtime_error("Invalid line: ':' expected after major");
    if (devMajor != major)
        return tags;        // not the device we are looking for

    // Parse minor device number

    p = endptr + 1;
    if (!isdigit(*p))
        throw runtime_error("Invalid line: digit expected for minor");

    int devMinor = strtol(p, (char **)&endptr, 10);
    if (*endptr != ' ')
        throw runtime_error("Invalid line: space expected after minor");
    if (devMinor != minor)
        return tags;        // not the device we are looking for

    p = endptr + 1;
    while (*p) {

        // parse the tag

        endptr = findTokenEnd(p);
        if (endptr == p) {
            throw runtime_error("Invalid line: alpha character expected for tag");
        }
        if (*endptr != '=') {
            throw runtime_error("Invalid line: '=' expected after tag");
        }
        string tag(p, endptr);
        p = endptr + 1;

        // parse the value

        endptr = findTokenEnd(p);
        if (endptr == p) {
            throw runtime_error("Invalid line: alphanumeric character expected for value");
        }

        NumericValue val(p, endptr);
        if (val.isInvalid())
            throw runtime_error("Invalid line: invalid numeric value");
        tags.emplace(tag, val);
        p = endptr;
        if (*p) {
            if (*p != ' ')
                throw runtime_error("Invalid line: expected space character after value");
            ++p;
            if (!*p)
                throw runtime_error("Invalid line: expected new tag after last value");
        }
    }

    return tags;
}

}   // namespace pc
