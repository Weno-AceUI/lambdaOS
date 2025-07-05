#!/bin/bash

# QEMU testing script for OpenAce

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Function to print status
print_status() {
    echo -e "${GREEN}[*]${NC} $1"
}

# Function to print error
print_error() {
    echo -e "${RED}[!]${NC} $1"
}

# Function to print warning
print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

# Check if QEMU is installed
if ! command -v qemu-system-aarch64 &> /dev/null; then
    print_error "QEMU is not installed. Please install QEMU first."
    print_warning "On macOS: brew install qemu"
    print_warning "On Ubuntu: sudo apt-get install qemu-system-arm"
    exit 1
fi

# Create disk images if they don't exist
if [ ! -f "openace.img" ]; then
    print_status "Creating OpenAce disk image..."
    qemu-img create -f raw openace.img 4G
fi

if [ ! -f "userdata.img" ]; then
    print_status "Creating user data disk image..."
    qemu-img create -f raw userdata.img 2G
fi

# Build OpenAce
print_status "Building OpenAce..."
make clean
make

# Run QEMU
print_status "Starting QEMU..."
qemu-system-aarch64 \
    -machine virt \
    -cpu cortex-a53 \
    -m 2G \
    -smp 4 \
    -nographic \
    -drive file=openace.img,format=raw,if=virtio \
    -drive file=userdata.img,format=raw,if=virtio \
    -net nic,model=virtio \
    -net user \
    -device virtio-gpu-pci \
    -device virtio-keyboard-pci \
    -device virtio-mouse-pci \
    -kernel build/kernel.bin \
    -initrd build/initrd.img \
    -append "console=ttyAMA0 root=/dev/vda1"

# Handle QEMU exit
if [ $? -eq 0 ]; then
    print_status "QEMU exited successfully"
else
    print_error "QEMU exited with an error"
fi 