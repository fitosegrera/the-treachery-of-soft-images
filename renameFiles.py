'''
Bulk file rename
Author: fito_segrera
USAGE: python renameFiles.py endFolder
For more information check the README.md file
'''

import os, sys

image_type = sys.argv[1]

directoryname="searchImageScrape/images/" + image_type + "/"
listFiles = os.listdir(directoryname)

for i in range(len(listFiles)):
    os.rename(os.path.join(directoryname, str(listFiles[i])), 
    	os.path.join(directoryname, str(i) + ".jpg"))