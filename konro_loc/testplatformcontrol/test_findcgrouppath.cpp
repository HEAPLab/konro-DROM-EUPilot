#include "unittest.h"
#include "cgroup/cgrouputil.h"
#include <string>

using namespace std;

static int testFind1()
{
    string path1 = pc::util::findCgroupPath((pid_t)1);
    if (path1 != "/sys/fs/cgroup/init.scope")
        return TEST_FAILED;
    return TEST_OK;
}

int main()
{
    if (testFind1() != TEST_OK)
        return TEST_FAILED;

   return TEST_OK;
}
