#!/usr/bin/env bash
set -euo pipefail

echo "=== EMBEDDED TOOLCHAIN DETECTOR (LINUX/macOS) ==="

OK="\033[0;32m[OK]\033[0m"
WARN="\033[0;33m[WARN]\033[0m"
ERR="\033[0;31m[ACTION REQUIRED]\033[0m"

find_tool() {
    local name="$1"
    local path
    path=$(command -v "$name" 2>/dev/null || true)
    if [[ -n "$path" ]]; then
        echo -e "$OK $name -> $path ($($name --version 2>/dev/null | head -1 || true))"
        return 0
    else
        echo -e "$WARN $name NOT FOUND"
        return 1
    fi
}

echo ""
echo "--- Cross-Compilers (ARM MCU Target) ---"
find_tool arm-none-eabi-gcc || true
find_tool arm-none-eabi-g++ || true
find_tool arm-none-eabi-objcopy || true
find_tool arm-none-eabi-size || true

echo ""
echo "--- Host Compilers (PC Unit Tests) ---"
host_ok=0
find_tool gcc && host_ok=1 || true
find_tool g++ && host_ok=1 || true
if [[ $host_ok -eq 0 ]]; then
    echo -e "$ERR No Host C/C++ compiler found."
    echo "  Install via: sudo apt install -y build-essential"
fi

echo ""
echo "--- Debugger ---"
find_tool gdb-multiarch || find_tool arm-none-eabi-gdb || true

echo ""
echo "--- Build Tools ---"
find_tool cmake || true
find_tool ninja || true

echo ""
echo "--- Debug Probes ---"
find_tool openocd || true

echo ""
echo "--- Coverage Tools ---"
find_tool gcov || true
find_tool lcov || true
find_tool genhtml || true

echo ""
echo "--- USB Permissions (ST-Link) ---"
if [[ -f /etc/udev/rules.d/49-stlinkv3.rules ]]; then
    echo -e "$OK udev rules for ST-Link V3 exist."
else
    echo -e "$WARN /etc/udev/rules.d/49-stlinkv3.rules not found."
    echo "  Create with:"
    echo '  sudo tee /etc/udev/rules.d/49-stlinkv3.rules << EOF'
    echo '  ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374e", MODE="0666", GROUP="plugdev"'
    echo '  ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374f", MODE="0666", GROUP="plugdev"'
    echo '  ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3753", MODE="0666", GROUP="plugdev"'
    echo '  EOF'
fi

echo ""
echo "=============================================="
