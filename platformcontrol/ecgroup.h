#ifndef ECGROUP_H
#define ECGROUP_H

#include <string>

namespace pc {

class EcGroup {
    static const char *fileNames[];
public:
    enum ECGROUP {
        // read-write
        CPU_WEIGHT = 0,
        CPU_MAX = 1,
        // read-only
        CPU_STAT = 2,

        // read-write
        CPUSET_CPUS = 3,
        CPUSET_MEMS = 4,
        // read-only
        CPUSET_CPUS_EFFECTIVE = 5,
        CPUSET_MEMS_EFFECTIVE = 6,

        // read-write
        PIDS_MAX = 7,

    };

    static const char *getControllerName(EcGroup::ECGROUP ecGroup);
    static const char *getFileName(EcGroup::ECGROUP ecGroup);
};

}   // namespace pc
#endif // ECGROUP_H
