# Kernel-specific settings
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Include directories
include_directories(
    include
    core
    drivers
    services
    boot
    arch
    syscalls
)

# Source files
set(KERNEL_SOURCES
    core/init.c
    core/memory.c
    core/process.c
    core/device.c
    core/config.c
    core/power.c
    core/time.c
    core/devicetree.c
    core/ipc.c
    core/security.c
    core/log.c
    core/net.c
    core/fs.c
    core/scheduler.c
    drivers/driver.c
    services/devmgr.c
    boot/boot.s
)

# Set compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fPIE -m64 -march=x86-64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fPIE -m64 -march=x86-64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow")

# Set linker flags
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -z nodefaultlib -z noexecstack -no-pie -T ${CMAKE_CURRENT_SOURCE_DIR}/boot/linker.ld")

# Build kernel
add_executable(kernel.bin ${KERNEL_SOURCES})

# Build initrd
add_custom_command(
    OUTPUT initrd.img
    COMMAND ${CMAKE_SOURCE_DIR}/tools/mkinitrd.sh
    DEPENDS kernel.bin
)

# Copy files to build directory
add_custom_command(
    TARGET kernel.bin
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/kernel.bin
        ${CMAKE_BINARY_DIR}/initrd.img
        ${CMAKE_BINARY_DIR}/
) 