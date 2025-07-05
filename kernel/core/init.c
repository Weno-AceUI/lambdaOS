#include "init.h"
#include "scheduler.h"
#include "process.h"
#include "memory.h"
#include "ipc.h"
#include "device.h"
#include "fs.h"
#include "net.h"
#include "log.h"
#include "security.h"

void kernel_init(void) {
    log_init();
    memory_init();
    process_init(); // You may want to implement this
    scheduler_init(SCHED_RR);
    ipc_init();
    device_init();
    fs_init();
    net_init();
    security_init();
}

void kernel_start(void) {
    // Start the first process, enter main scheduling loop, etc.
    // For now, just a stub
    while (1) {
        scheduler_schedule();
    }
} 