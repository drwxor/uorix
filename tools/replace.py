#!/usr/bin/env python3

import os
import subprocess
import sys
import time
from pathlib import Path

def run(*args):
    result = subprocess.run(args, capture_output=True, text=True)

    if result.returncode != 0:
        if result.stderr:
            sys.stderr.write(result.stderr)
        raise RuntimeError(f"command failed: {' '.join(args)}")

    return result.stdout.strip()

def replace():
    print("==> Replacing...")

    rootrun = os.environ.get("RUN_AS_ROOT", "run0")

    kernel = "build/uorix.elf"
    shell = "build/userland/shell.elf"

    bootx64 = os.environ.get(
        "BOOTX64",
        "/nix/store/kpgkpqpz1vjgzm5askic2pv0y7p4ssb0-limine-12.7.0/share/limine/BOOTX64.EFI"
    )

    img = "image/uorix.img"
    mnt = "/tmp/uorix-mnt"
    rt = "/tmp/uorix-root"

    os.makedirs("image", exist_ok=True)

    if not os.path.isfile(kernel):
        sys.stderr.write(f"Error: {kernel} not found – run with -b first\n")
        return 1

    if not os.path.isfile(shell):
        sys.stderr.write(f"Error: {shell} not found – run with -b first\n")
        return 1

    if not os.path.isfile(bootx64):
        sys.stderr.write(f"Error: {bootx64} not found\n")
        return 1

    loop = None
    mounted_esp = False
    mounted_root = False

    try:
        loop = run(rootrun, "losetup", "--find", "--show", "--partscan", img)

        time.sleep(0.5)

        esp = f"{loop}p1"
        root = f"{loop}p2"

        if not Path(esp).is_block_device() or not Path(root).is_block_device():
            raise RuntimeError(
                "Loop partition devices did not appear properly"
            )

        run(rootrun, "mkdir", "-p", mnt)
        run(rootrun, "mkdir", "-p", rt)

        run(rootrun, "mount", esp, mnt)
        mounted_esp = True

        run(rootrun, "mkdir", "-p", f"{mnt}/EFI/BOOT", f"{mnt}/boot")

        run(rootrun, "cp", bootx64, f"{mnt}/EFI/BOOT/BOOTX64.EFI")

        run(rootrun, "cp", kernel, f"{mnt}/boot/uorix.elf")

        run(rootrun, "mount", root, rt)
        mounted_root = True

        run(rootrun, "mkdir", "-p", f"{rt}/bin")

        run(rootrun, "cp", shell, f"{rt}/bin/sh")

        conf_path = "/tmp/uorix-limine.conf"

        with open(conf_path, "w") as f:
            f.write(
                "timeout: 1\n"
                "\n"
                "/Uorix\n"
                "    protocol: limine\n"
                "    path: boot():/boot/uorix.elf\n"
            )

        run(rootrun, "cp", conf_path, f"{mnt}/limine.conf")

        run("sync")

    except RuntimeError as e:
        sys.stderr.write(f"Error: {e}\n")
        return 1

    finally:
        if mounted_root:
            subprocess.run([rootrun, "umount", rt], check=False)

        if mounted_esp:
            subprocess.run([rootrun, "umount", mnt], check=False)

        if loop:
            subprocess.run([rootrun, "losetup", "-d", loop], check=False)

        subprocess.run(["sync"], check=False)

    print(f"==> Image ready: {img}")
    return 0


if __name__ == "__main__":
    sys.exit(replace())
