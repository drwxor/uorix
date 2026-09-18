import subprocess
import sys

def build():
    print("==> Building...")
    result = subprocess.run(["mold"])

    return result.returncode;

if __name__ == "__main__":
    sys.exit(build())
