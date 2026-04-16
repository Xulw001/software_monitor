#ifndef IOCTL_H
#define IOCTL_H

#include <wdm.h>

namespace monitor {
namespace ioctl {

bool initialize();
void finalize();

NTSTATUS Dispatcher(PIRP irp);

struct MessageHeader {
    SIZE_T size;  // message size, excluding size field
};
bool SendMessage(MessageHeader* msg);

}  // namespace ioctl
}  // namespace monitor

#endif