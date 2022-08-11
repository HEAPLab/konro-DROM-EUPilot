#include "iocontrol.h"
#include "../tsplit.h"
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

namespace pc {

/*static*/
const char *IOControl::controllerName_ = "io";

/*static*/
const map<IOControl::ControllerFile, const char *> IOControl::fileNamesMap_ = {
    { STAT, "io.stat" },
    { MAX, "io.max" }
};

/*static*/
const map<IOControl::IoMax, const char *> IOControl::keyNames_ = {
    { RBPS, "rbps" },
    { WBPS, "wbps" },
    { RIOPS, "riops" },
    { WIOPS, "wiops" },
};

map<string, unsigned long> IOControl::getIOStat(App app)
{
    return getContentAsMap(fileNamesMap_.at(STAT), app);
}

void IOControl::setIOMax(int major, int minor, IoMax ioMax, int value, App app)
{
    ostringstream os;
    os << major << ':' << minor << ' ' << keyNames_.at(ioMax) << '=';
    if (value == MAX_IO_CONTROL)
        os << "max";
    else
        os << value;
    CGroupControl::setValue(controllerName_, fileNamesMap_.at(MAX), os.str(), app);
}

std::map<string, long> IOControl::getIOMax(int major, int minor, App app)
{
    std::map<std::string, long> tags;
    vector<string> lines = CGroupControl::getContent(fileNamesMap_.at(MAX), app);
    ostringstream os;
    os << major << ':' << minor << ' ';
    string initial = os.str();
    for (const string &line: lines) {
        cout << ">>>>>" << line << endl;
        string curInitial = line.substr(0, initial.size());
        cout << "initial=" << initial << ", curInitial=" << curInitial << endl;
        if (curInitial == initial) {
            cout << "equal\n";
            string curFollow = line.substr(initial.size());
            istringstream is(curFollow);
            string word;
            std::string tag;
            unsigned long value;
            while (true) {
                is >> word;
                cout << "word=" << word << endl;
                if (is.fail())
                    break;
                vector<string> tagValue = split::tsplit(word, "=");
                if (tagValue.size() != 2)
                    break;
                if (tagValue[1] == "max")
                    tags.emplace(tagValue[0], MAX_IO_CONTROL);
                else
                    tags.emplace(tagValue[0], strtoul(tagValue[1].c_str(), nullptr, 10));
            }
            break;
        }
    }
    return tags;
}

}   // namespace pc
