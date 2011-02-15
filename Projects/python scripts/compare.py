"""
Compare two aligned images of the same size and compute the difference image

Usage: python compare.py image1 image2
"""


import os, sys
from PIL import Image
from PIL import ImageChops
from PIL import ImageOps


def main():
	file1, file2 = sys.argv[1:1+2]
	print "compare images"
	print "image mode is greyscale (if images has another mode, the image will be converted)"
	im1 = Image.open(file1)
	im2 = Image.open(file2)
	print "image 1 : " , file1 , " , mode : ", im1.mode
	print "image 2 : " , file2 , " , mode : ", im2.mode
	
	if im1.mode != "L":
		im1 = im1.convert("L")
	if im2.mode != "L":
		im2 = im2.convert("L")
	
	diff = ImageChops.difference(im2,im1)
	diff = ImageOps.autocontrast(diff,0)
	diff.show("difference")
	im1.show("image1")
	im2.show("image2")
	
	save_path,f = os.path.split(file1)
	diff_path = save_path + "/diff.TIF"
	diff.save(diff_path)
	im1_path = save_path + "/im1.TIF"
	im1.save(im1_path)
	im2_path = save_path + "/im2.TIF"
	im2.save(im2_path)

if __name__ == "__main__":
	main()