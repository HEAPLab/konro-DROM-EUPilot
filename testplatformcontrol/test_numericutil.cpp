#include "unittest.h"
#include "numericvalue.h"
#include "numericutil.h"

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;
using namespace pc;

static int testParseNvSyntax(int major, int minor, const char *line, bool expectedError = false)
{
    try {
        map<string, NumericValue> tags = pc::parseLineNv(line, major, minor);
        if (expectedError) {
            return TEST_FAILED;
        }
#if 0
        if (tags.empty()) {
            cout << "    No tags found\n";
        } else {
            for (const auto &kp : tags) {
                cout << "    TAG: " << kp.first << ", VALUE: " << kp.second << endl;
            }
        }
#endif
    } catch (runtime_error &e) {
        if (!expectedError) {
            return TEST_FAILED;
        }
    }
    return TEST_OK;
}

static int testParseNvValues()
{
    const char line[] = "8:0 abcd=12345 efgh=max pippo=7 pluto=12345678";

    map<string, NumericValue> tags = pc::parseLineNv(line, 8, 0);
    if (tags.empty())
        return TEST_FAILED;
    if (tags["abcd"] != 12345)
        return TEST_FAILED;
    if (!tags["efgh"].isMax())
        return TEST_FAILED;
    if (tags["pippo"] != 7)
        return TEST_FAILED;
    if (tags["pluto"] != 12345678)
        return TEST_FAILED;

    return TEST_OK;
}

int main(int argc, char *argv[])
{
    // expected to be successful

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345 efgh=54321 pippo=7 pluto=12345678") != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 1, "8:0 abcd=12345 efgh=54321 pippo=7 pluto=12345678") != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345 efgh=max pippo=7 pluto=12345678") != TEST_OK)
        return TEST_FAILED;

    // expected to fail

    if (testParseNvSyntax(8, 0, "8.0 abcd=12345 efgh=max pippo=7 pluto=12345678", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0:abcd=12345 efgh=max pippo=7 pluto=12345678", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0  abcd=12345 efgh=max pippo=7 pluto=12345678", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "x:0 abcd=12345 efgh=max pippo=7 pluto=12345678", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 =12345 efgh=max pippo=7 pluto", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345 efgh=max pippo=7 pluto", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345 efgh=max pippo=7 pluto=", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:a abcd=12345 efgh=max pippo=7 pluto=12345678", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345 efgh=max pippo=7 pluto=1234567a", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd =12345 efgh=max pippo=7 pluto=1234567", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd= 12345 efgh=max pippo=7 pluto=1234567", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=1 2345 efgh=max pippo=7 pluto=1234567", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345  efgh=max pippo=7 pluto=1234567a", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvSyntax(8, 0, "8:0 abcd=12345 efgh=max pippo=7 pluto=1234567 ", true) != TEST_OK)
        return TEST_FAILED;

    if (testParseNvValues() != TEST_OK)
        return TEST_FAILED;

    return TEST_OK;
}
