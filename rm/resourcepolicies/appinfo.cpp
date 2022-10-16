#include "appinfo.h"

AppInfo::AppInfo(std::shared_ptr<pc::App> app) :
    app_(app),
    cpu_(-1)
{

}
