#!/usr/bin/env python

import argparse
import sys
from sys import platform as _platform
#from secureboot import which

def get_system_name():
	if "linux" in _platform:
		return "Linux"
	elif any(win in _platform for win in ['win32', 'win64']):
		if (which('uname') == ""):
			return "windows"
		return "CYGWIN"
	elif "cygwin" in _platform:
		return "CYGWIN"
	elif "darwin" in _platform:
		return "Darwin"
	else:
		return None

def main():
	parser = argparse.ArgumentParser(description='This script prints OS name')

	parser.add_argument("-V", "--version",
			action="version",
			version="%(prog)s v1.0")

	utilParser = parser.add_mutually_exclusive_group()

	utilParser.add_argument("-s", "--get_system_name",
			action="store_true",
			dest="get_system_name",
			default=False,
			help="Get system name")

	opts, unknown = parser.parse_known_args()

	if len(sys.argv) == 1:
		parser.print_help()
		parser.exit(1)

	if opts.get_system_name:
		systemName = get_system_name()
		if systemName:
			print systemName

if __name__ == '__main__':
	main()
