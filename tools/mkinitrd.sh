#!/bin/bash

# Create a temporary directory for initrd contents
TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

# Create basic directory structure
mkdir -p "$TMPDIR"/{bin,dev,etc,lib,proc,sys}

# Copy essential files
cp build/WebCppApp "$TMPDIR/bin/"
cp aceboot/init.conf "$TMPDIR/etc/"

# Create device nodes
mknod "$TMPDIR/dev/console" c 5 1
mknod "$TMPDIR/dev/null" c 1 3

# Create init script
cat > "$TMPDIR/init" << 'EOF'
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

# Start system services
for service in /etc/init.conf; do
    if [ -f "$service" ]; then
        while read -r line; do
            case "$line" in
                \#*|"") continue ;;
                *) eval "$line" ;;
            esac
        done < "$service"
    fi
done

# Start WebCpp
exec /bin/WebCppApp
EOF

chmod +x "$TMPDIR/init"

# Create initrd image
(cd "$TMPDIR" && find . | cpio -o -H newc | gzip > "$(pwd)/build/initrd.img") 