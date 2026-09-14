#!/bin/sh

# Development helper for Uorix – single ESP (works with OVMF)

set -eu

ROOTRUN=${ROOTRUN:-run0}

KERNEL=build/uorix.elf
SHELL=build/userland/shell.elf
BOOTX64=${BOOTX64:-/nix/store/kpgkpqpz1vjgzm5askic2pv0y7p4ssb0-limine-12.7.0/share/limine/BOOTX64.EFI}
CODEFD=${CODEFD:-/nix/store/s2nn8543ykh79cfdi7l3m49snkv64lq3-OVMF-202608-fd/FV/OVMF_CODE.fd}
IMG=image/uorix.img
MNT=/tmp/uorix-mnt
RT=/tmp/uorix-root

build() {
    echo "==> Building..."
    if ! output=$(samu 2>&1); then
        printf '%s\n' "$output" >&2
        return 1
    fi
}

build_scratch() {
    echo "==> Building from scratch..."
    rm -rf build/*
    if ! output=$(samu 2>&1); then
        printf '%s\n' "$output" >&2
        return 1
    fi
}

replace() {
    echo "==> Replacing..."

    mkdir -p image

    if [ ! -f "$KERNEL" ]; then
        echo "Error: $KERNEL not found – run ./uorix.sh -b first" >&2
        return 1
    fi
    if [ ! -f "$BOOTX64" ]; then
        echo "Error: $BOOTX64 not found" >&2
        return 1
    fi

    # rm -f "$IMG"
    # dd if=/dev/zero of="$IMG" bs=1M count=64 status=none

    # parted -s "$IMG" mklabel gpt
    # parted -s "$IMG" mkpart ESP fat32 1MiB 100%
    # parted -s "$IMG" set 1 esp on

    LOOP=$($ROOTRUN losetup --find --show --partscan "$IMG")
    sleep 0.5

    ESP="${LOOP}p1"
    ROOT="${LOOP}p2"

    if [ ! -b "$ESP" ]; then
        echo "Error: $ESP did not appear" >&2
        $ROOTRUN losetup -d "$LOOP" || true
        return 1
    fi

    if [ ! -b "$ROOT" ]; then
        echo "Error: $ROOT did not appear" >&2
        $ROOTRUN losetup -d "$LOOP" || true
        return 1
    fi

    $ROOTRUN mkdir -p "$MNT"
    $ROOTRUN mount "$ESP" "$MNT"
    $ROOTRUN mkdir -p "$MNT/EFI/BOOT" "$MNT/boot"

    $ROOTRUN cp "$BOOTX64" "$MNT/EFI/BOOT/BOOTX64.EFI"
    $ROOTRUN cp "$KERNEL"  "$MNT/boot/uorix.elf"

    $ROOTRUN mount -o loop,offset=$((67584 * 512)) $IMG $RT

    $ROOTRUN mkdir -p  $RT/bin
    $ROOTRUN cp $SHELL $RT/bin/sh

    cat > /tmp/uorix-limine.conf << 'EOF'
timeout: 1

/Uorix
    protocol: limine
    path: boot():/boot/uorix.elf
EOF
    $ROOTRUN cp /tmp/uorix-limine.conf "$MNT/limine.conf"

    sync
    $ROOTRUN umount $RT
    $ROOTRUN umount "$MNT"
    $ROOTRUN losetup -d "$LOOP"
    sync

    echo "==> Image ready: $IMG"
}

start() {
    echo "==> Starting QEMU..."

    qemu-system-x86_64 \
        -machine pc \
        -m 512M \
        -drive if=pflash,format=raw,readonly=on,file="$CODEFD" \
        -drive format=raw,file="$IMG" \
        -serial stdio \
        -no-reboot -no-shutdown
}

show_help() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -b, --build       Build (samu)"
    echo "  -B, --build-scr   Build from scratch"
    echo "  -r, --replace     Recreate disk image"
    echo "  -R, --run         Boot in QEMU"
    echo "  -h, --help        Show this help"
    echo ""
    echo "Short flags can be combined (e.g. -brR)"
}

if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)    show_help; exit 0 ;;
        --build)      build ;;
        --build-scr)  build_scratch ;;
        --replace)    replace ;;
        --run)        start ;;
        -[!-]*)
            flags=${1#-}
            while [ -n "$flags" ]; do
                flag=${flags%"${flags#?}"}
                flags=${flags#?}
                case "$flag" in
                    b) build ;;
                    B) build_scratch ;;
                    r) replace ;;
                    R) start ;;
                    h) show_help; exit 0 ;;
                    *)
                        echo "Error: unknown option '-$flag'" >&2
                        show_help
                        exit 1
                        ;;
                esac
            done
            ;;
        *)
            echo "Error: unknown option '$1'" >&2
            show_help
            exit 1
            ;;
    esac
    shift
done
