#ifndef FILE_MONITOR_H
#define FILE_MONITOR_H

#include <ntifs.h>

namespace monitor {
namespace file {
bool initialize(PDRIVER_OBJECT drv);
void finalize();
}
}  // namespace monitor
#endif
