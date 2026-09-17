#!/usr/bin/env python3
import os
import subprocess
import sys
import time
from pathlib import Path

def replace():
    print("==> Replacing...")

    rootrun = os.environ.get("RUN_AS_ROOT", "run0")
    kernel = "build/uorix.elf"
    shell = "build/userland/shell.elf"
    bootx64 = os.environ.get("BOOTX64", "/nix/store/kpgkpqpz1vjgzm5askic2pv0y7p4ssb0-limine-12.7.0/share/limine/BOOTX64.EFI")
    img = "image/uorix.img"
    mnt = "/tmp/uorix-mnt"
    rt = "/tmp/uorix-root"

    os.makedirs("image", exist_ok=True)

    if not os.path.isfile(kernel):
        sys.stderr.write(f"Error: {kernel} not found – run with -b first\n")
        return 1
    if not os.path.isfile(bootx64):
        sys.stderr.write(f"Error: {bootx64} not found\n")
        return 1

    loop_cmd = [rootrun, "losetup", "--find", "--show", "--partscan", img]
    result = subprocess.run(loop_cmd, capture_output=True, text=True)
    if result.returncode != 0:
        sys.stderr.write("Error: Failed to setup loop device\n")
        return 1

    loop = result.stdout.strip()
    time.sleep(0.5)

    esp = f"{loop}p1"
    root = f"{loop}p2"

    if not Path(esp).is_block_device() or not Path(root).is_block_device():
            sys.stderr.write("Error: Loop partition devices did not appear properly\n")
            subprocess.run([rootrun, "losetup", "-d", loop])
            return 1

    subprocess.run([rootrun, "mkdir", "-p", mnt])
    subprocess.run([rootrun, "mount", esp, mnt])
    subprocess.run([rootrun, "mkdir", "-p", f"{mnt}/EFI/BOOT", f"{mnt}/boot"])

    subprocess.run([rootrun, "cp", bootx64, f"{mnt}/EFI/BOOT/BOOTX64.EFI"])
    subprocess.run([rootrun, "cp", kernel, f"{mnt}/boot/uorix.elf"])

    offset = 67584 * 512
    subprocess.run([rootrun, "mount", "-o", f"loop,offset={offset}", img, rt])

    subprocess.run([rootrun, "mkdir", "-p", f"{rt}/bin"])
    subprocess.run([rootrun, "cp", shell, f"{rt}/bin/sh"])

    conf_path = "/tmp/uorix-limine.conf"
    with open(conf_path, "w") as f:
        f.write("timeout: 1\n\n/Uorix\n    protocol: limine\n    path: boot():/boot/uorix.elf\n")

    subprocess.run([rootrun, "cp", conf_path, f"{mnt}/limine.conf"])

    subprocess.run(["sync"])
    subprocess.run([rootrun, "umount", rt])
    subprocess.run([rootrun, "umount", mnt])
    subprocess.run([rootrun, "losetup", "-d", loop])
    subprocess.run(["sync"])

    print(f"==> Image ready: {img}")
    return 0

if __name__ == "__main__":
    sys.exit(replace())
