# script to convert images to grayscale images
# (c) Maik Stille <maik.stille@gmail.com>

import os, sys
from PIL import Image
from PIL import ImageOps


def main():
	
	numarg = len(sys.argv)
	if numarg < 2:
		print ""
		print "Syntax: convertToGrayscale.py <path to images>*<extension of images>"
		print ""
		exit(0)
	
	greyscale_path = sys.argv[1]
	greyscale_path,tmpfile = os.path.split(greyscale_path)
	greyscale_path += "/gray/"
	try:
		os.mkdir(greyscale_path)
	except OSError, e:
		print e
		print "files not converted"
		quit()
	print "files saved in :"
	print greyscale_path


	for infile in sys.argv[1:]:
		image_path, image_file = os.path.split(infile)
		outfile = greyscale_path + image_file
		f,e = os.path.splitext(outfile)
		outfile = f + "_gray.TIF"
		if infile != outfile:
			try:
				image = Image.open(infile)
				ImageOps.grayscale(image).save(outfile)		
			except Exception, e:
				print "cannot convert: ", infile , e



def normailzeImage(image):
	"""docstring for normailzeImage"""
	

				
if __name__ == '__main__':
	main()