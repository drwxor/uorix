import subprocess
import sys

def build():
    print("==> Building...")
    result = subprocess.run(["kage"])

    return result.returncode;

if __name__ == "__main__":
    sys.exit(build())
