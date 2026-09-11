#!/bin/sh

# this was written by ai because i dont want to spend time doing a shell script to SAVE time.

set -eu

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
    echo "==> Replacing kernel in image..."

    loopdev=$(run0 losetup --find --show --partscan image/uorix.img)

    cleanup() {
        run0 umount /tmp/uorix-mnt 2>/dev/null || true
        run0 losetup -d "$loopdev" 2>/dev/null || true
    }

    trap cleanup EXIT

    run0 mount "${loopdev}p1" /tmp/uorix-mnt
    run0 cp build/uorix.elf /tmp/uorix-mnt/boot/uorix.elf
    run0 cp image/limine.conf /tmp/uorix-mnt/limine.conf
    sync

    trap - EXIT
    cleanup
}

start() {
    echo "==> Starting QEMU..."
    qemu-system-x86_64 \
      -machine q35 \
      -drive if=pflash,format=raw,readonly=on,file=/nix/store/50yl2vzad57dkkzyd3cwl8x8pfqaj50s-OVMF-202608-fd/FV/OVMF_CODE.fd \
      -drive format=raw,file=image/uorix.img \
      -chardev stdio,id=serial0 \
      -serial chardev:serial0 \
      -no-reboot \
      -no-shutdown
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
