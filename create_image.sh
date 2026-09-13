ROOTR=run0
KERNEL=build/uorix.elf
BOOTX64=/nix/store/kpgkpqpz1vjgzm5askic2pv0y7p4ssb0-limine-12.7.0/share/limine/BOOTX64.EFI
IMG=image/uorix.img

mkdir -p image
cat > image/limine.conf << 'EOF'
timeout: 1

/Uorix
    protocol: limine
    path: boot():/boot/uorix.elf
EOF

rm -f "$IMG"
dd if=/dev/zero of="$IMG" bs=1M count=64
parted -s "$IMG" mklabel gpt
parted -s "$IMG" mkpart ESP fat32 1MiB 100%
parted -s "$IMG" set 1 esp on

LOOP=$($ROOTR losetup --find --show --partscan "$IMG")
$ROOTR mkfs.fat -F 32 -n UORIX "${LOOP}p1"

MNT=/tmp/uorix-mnt
$ROOTR mkdir -p "$MNT"
$ROOTR mount "${LOOP}p1" "$MNT"
$ROOTR mkdir -p "$MNT/EFI/BOOT" "$MNT/boot"
$ROOTR cp "$BOOTX64" "$MNT/EFI/BOOT/BOOTX64.EFI"
$ROOTR cp "$KERNEL"  "$MNT/boot/uorix.elf"
$ROOTR cp image/limine.conf "$MNT/limine.conf"
sync
$ROOTR umount "$MNT"
$ROOTR losetup -d "$LOOP"
