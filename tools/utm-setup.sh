#!/bin/bash

# UTM setup script for OpenAce

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

# Check if UTM is installed
if [ ! -d "/Applications/UTM.app" ]; then
    print_error "UTM is not installed. Please install UTM from the Mac App Store or https://mac.getutm.app/"
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
if [ ! -d "build" ]; then
    mkdir build
fi
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../tools/macos_toolchain.cmake ..
cmake --build . --clean-first

# Create UTM configuration directory if it doesn't exist
UTM_CONFIG_DIR="$HOME/Library/Containers/com.utmapp.UTM/Data/Documents"
if [ ! -d "$UTM_CONFIG_DIR" ]; then
    mkdir -p "$UTM_CONFIG_DIR"
fi

# Copy UTM configuration
print_status "Setting up UTM configuration..."
cp ../device/utm/OpenAce.utm "$UTM_CONFIG_DIR/"

print_status "Setup complete! You can now open UTM and run OpenAce."
print_warning "Note: Make sure to copy the kernel.bin and initrd.img to the correct location in UTM's configuration." 

ADDON_SOURCE_PATH="./build/native_modules/webcpp_addon.node"