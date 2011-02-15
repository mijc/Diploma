# batch script for computing transformations for each image with elastix
# fixed image is the image in the middle of the given image array

# (c) Maik Stille <maik.stille@gmail.com>

import sys
import os
import math

def main():
    """docstring for main"""
    
    numarg = len(sys.argv)

    if numarg < 4:
        # print help message
        print ""
        print "Syntax : batchElastix.py <path_to_images>*<extension_of_images> <path_for_output> <parameterfile> [fixed_image_index]"
        print ""        
        exit( 0 )

    print ""
    print "using the following settings"

    # this is dirty, fix this!!
    no_fixed_image_index = False    
    try:
        fixed_image_index = int(sys.argv[numarg - 1])
    except Exception, e:
        print "no fixed_image_index is set"
        no_fixed_image_index = True
    else:
        fixed_image_index = int(sys.argv[numarg - 1])
        parameterfile = sys.argv[numarg - 2]
        outputdir = sys.argv[numarg - 3]
        numimages = numarg - 4
    finally:
        if no_fixed_image_index:
            parameterfile = sys.argv[numarg - 1]
            outputdir = sys.argv[numarg - 2]
            numimages = numarg - 3
            fixed_image_index = numimages / 2

    print "fixed image index : ", fixed_image_index     
    print "number of images : ", numimages
    print "parameterfile : ", parameterfile
    print "outputdir : ", outputdir
    create_output_dir(outputdir)
    print ""

    fixed_image_path = sys.argv[fixed_image_index]
    left_side_images_path = sys.argv[1:fixed_image_index + 1]
    right_side_images_path = sys.argv[fixed_image_index:numimages]
    
    print "left side:"
    for path in left_side_images_path:
        print path
    print " "
    print "right side:"
    for path in right_side_images_path:
        print path
    print ""

    # seq = range(8)
    # print reduce(add,seq)



def registrate(fixed_image,moving_image):
    """compute the transformation to map the <moving_image> with the <fixed_image> and returns the transformed <moving_image>"""
    pass



def create_output_dir(path):
    """creates a given directory
        
        if it allready exists, ask to overwrite"""

    if not os.path.exists(path):
        try:
            os.mkdir(path)
        except Exception, e:
            raise e
    else:
      print "WARNING : "
      print path, " already exist."
      print "files will be overwritten."
      print ""
      if not ask_ok("continue? "):
          exit(1)
        
def ask_ok(prompt, retries=4, complaint='Yes or no, please!'):
    """Ask if something <prompt> is okay or not"""
    while True:
        ok = raw_input(prompt)
        if ok in ('y', 'ye', 'yes'):
            return True
        if ok in ('n', 'no', 'nop', 'nope'):
            return False
        retries = retries - 1
        if retries < 0:
            raise IOError('refusenik user (you are stupid!)')
        print complaint


if __name__ == '__main__':
    main()
