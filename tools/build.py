import subprocess
import sys

def build():
    print("==> Building...")
    result = subprocess.run(["mold"], capture_output=True, text=True)

    if result.returncode != 0:
        sys.stderr.write(result.stdout)
        return 1
    return 0

if __name__ == "__main__":
    sys.exit(build())
