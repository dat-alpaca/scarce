#!/usr/bin/env python3

import os
import sys
import subprocess


def main():
    prefix   = os.environ.get('MESON_INSTALL_DESTDIR_PREFIX', '')
    elf_name = sys.argv[1]
    bin_name = sys.argv[2]

    elf_path = os.path.join(prefix, elf_name)
    bin_path = os.path.join(prefix, bin_name)

    subprocess.run(['objcopy', '-O', 'binary', elf_path, bin_path], check=True)
    os.remove(elf_path)

if __name__ == '__main__':
    main()