#ifndef DEV_H
#define DEV_H

#include <wdm.h>

namespace monitor {
namespace dev {

bool initialize(PDRIVER_OBJECT drv);
void finalize();

}  // namespace dev
}  // namespace monitor

#endif