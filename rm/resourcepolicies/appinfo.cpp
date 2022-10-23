#include "appinfo.h"

AppInfo::AppInfo(std::shared_ptr<rmcommon::App> app) :
    app_(app),
    coresNum_(-1)
{

}
