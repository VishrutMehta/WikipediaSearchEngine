#!/usr/env python
import glob
import os

def main():
	path = "/home/himanshu/mini_project/Wiki_Split_Files/*"
	fileNames = glob.glob(path)
	for files in fileNames:
		files = files.split('/')[-1]
		os.system("./a.out %s" % (files))

if __name__ == "__main__":
	main()
