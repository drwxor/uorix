#!/bin/sh

# this was written by ai because i dont want to spend time doing a shell script to SAVE time.

set -eu

ROOTRUN=run0

KERNEL=build/uorix.elf
BOOTX64=/nix/store/kpgkpqpz1vjgzm5askic2pv0y7p4ssb0-limine-12.7.0/share/limine/BOOTX64.EFI
CODEFD=/nix/store/s2nn8543ykh79cfdi7l3m49snkv64lq3-OVMF-202608-fd/FV/OVMF_CODE.fd
IMG=image/uorix.img
MNT=/tmp/uorix-mnt

build() {
    echo "==> Building..."

    if ! output=$(samu 2>&1); then
        printf '%s\n' "$output" >&2
        return 1
    fi
}

build_scratch() {
    echo "==> Building..."

    rm -rf build/*

    if ! output=$(samu 2>&1); then
        printf '%s\n' "$output" >&2
        return 1
    fi
}

replace() {
    echo "==> Remaking image..."

    mkdir -p image

    rm -f "$IMG"
    dd if=/dev/zero of="$IMG" bs=1M count=64
    parted -s "$IMG" mklabel gpt
    parted -s "$IMG" mkpart ESP fat32 1MiB 100%
    parted -s "$IMG" set 1 esp on

    LOOP=$($ROOTRUN losetup --find --show --partscan "$IMG")
    $ROOTRUN mkfs.fat -F 32 -n UORIX "${LOOP}p1"

    $ROOTRUN mkdir -p "$MNT"
    $ROOTRUN mount "${LOOP}p1" "$MNT"
    $ROOTRUN mkdir -p "$MNT/EFI/BOOT" "$MNT/boot"
    $ROOTRUN cp "$BOOTX64" "$MNT/EFI/BOOT/BOOTX64.EFI"
    $ROOTRUN cp "$KERNEL" "$MNT/boot/uorix.elf"
    $ROOTRUN cp image/limine.conf "$MNT/limine.conf"
    sync
    $ROOTRUN umount "$MNT"
    $ROOTRUN losetup -d "$LOOP"
    sync
}

start() {
    echo "==> Starting QEMU..."
    qemu-system-x86_64 \
      -machine q35 \
      -m 512M \
      -drive if=pflash,format=raw,readonly=on,file=$CODEFD \
      -drive format=raw,file=image/uorix.img \
      -serial stdio \
      -no-reboot -no-shutdown
}

show_help() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  -b, --build    Run build (samu)"
    echo "  -B, --build-scr    Run build from scratch (samu)"
    echo "  -r, --replace  Mount image and replace uorix.elf"
    echo "  -R, --run      Boot QEMU"
    echo "  -h, --help     Show this menu"
    echo ""
    echo "Note: Short flags can be combined (e.g., -brR)"
}

if [ $# -eq 0 ]; then
    show_help
    exit 0
fi

while [ $# -gt 0 ]; do
    case "$1" in
        -h|--help)
            show_help
            exit 0
            ;;

        --build)
            build
            ;;

        --build-scr)
            build_scratch
            ;;

        --replace)
            replace
            ;;

        --run)
            start
            ;;

        -b|-B|-r|-R)
            case "$1" in
                -b)
                    build
                    ;;
                -b)
                    build_scratch
                    ;;
                -r)
                    replace
                    ;;
                -R)
                    start
                    ;;
            esac
            ;;

        -[!-]*)
            flags=${1#-}

            while [ -n "$flags" ]; do
                flag=${flags%"${flags#?}"}
                flags=${flags#?}

                case "$flag" in
                    b)
                        build
                        ;;
                    B)
                        build_scratch
                        ;;
                    r)
                        replace
                        ;;
                    R)
                        start
                        ;;
                    h)
                        show_help
                        exit 0
                        ;;
                    *)
                        echo "Error: Unknown option '-$flag'" >&2
                        show_help
                        exit 1
                        ;;
                esac
            done
            ;;

        *)
            echo "Error: Unknown option '$1'" >&2
            show_help
            exit 1
            ;;
    esac

    shift
done
