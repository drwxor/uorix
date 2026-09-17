import argparse
import sys
import subprocess

def main():
    parser = argparse.ArgumentParser(description="Development helper for Uorix", add_help=False)

    parser.add_argument("-b", "--build", action="store_true")
    parser.add_argument("-r", "--replace", action="store_true")
    parser.add_argument("-R", "--run", action="store_true")
    parser.add_argument("-h", "--help", action="store_true")

    args = parser.parse_args()

    help_text = f"""Usage: {sys.argv[0]} [options]

Options:
  -b, --build       Run build script (build.py)
  -r, --replace     Run disk replacement script (replace.py)
  -R, --run         Run QEMU boot script (run.py)
  -h, --help        Show this help"""

    if args.help or not any([args.build, args.replace, args.run]):
        print(help_text)
        sys.exit(0)

    if args.build:
        print("==> Calling build script...")
        subprocess.run([sys.executable, "tools/build.py"])

    if args.replace:
        print("==> Calling replace script...")
        subprocess.run([sys.executable, "tools/replace.py"])

    if args.run:
        print("==> Calling run script...")
        subprocess.run([sys.executable, "tools/run.py"])

if __name__ == "__main__":
    main()
