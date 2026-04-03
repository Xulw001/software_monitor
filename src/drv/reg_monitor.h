#ifndef REG_MONITOR_H
#define REG_MONITOR_H

#include <ntifs.h>

namespace monitor {
namespace reg {

bool initialize(PDRIVER_OBJECT drv);
void finalize();
}  // namespace reg
}  // namespace monitor
#endif
