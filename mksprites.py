#!/usr/bin/python

import sys, struct
from PIL import Image
import re

def toARGB555(x) : 
    return "0x%x" % (((x[3]>>7)<<15 | (x[0]>>3)<<10 | (x[1]>>3)<<5 | x[2]>>3))

def writeTile2C(f, idx, idata, row, col, tsize, stide, name):
    f.write("static const unsigned short __"+name+"%d_data[] =\n{\n"%idx)
    for i in range(0, tsize[1]):
        p = (row*tsize[1]+i)*stride + col*tsize[0]
        ld = idata[p:p+tsize[0]]
        f.write("  " + ",".join(ld) + ",\n")
    f.write("};\n")



# Start parsing here !

_,file_in, name_out, tile_format = sys.argv
if not name_out:
    print("Provide a name for your tile set")
    exit(-1)
    
tsize = re.match("^(\d+)x(\d+)$", tile_format)
if not tsize:
    print("Describe tilesize in the form of e.g: 32x45")
    exit(-1)
    
tsize =  tuple(int(e) for e in tsize.groups())


im = Image.open(file_in).convert('RGBA')
om = [toARGB555(c) for c in im.getdata()]

ncol, nrow = (e % s for e,s in zip(im.size, tsize))
stride, _ = im.size
if nrow != 0 or ncol != 0:
    print "Image size is not a mutpile of the tile size"
    exit(-1)

ncol, nrow = (e / s for e,s in zip(im.size, tsize))

# Write C file
f = open(name_out+".c",'wb+')
# Process each tile in the left to right top to bottom order
for row in range(0, nrow):
    for col in range(0, ncol):
        writeTile2C(f, row*ncol+col, om, row, col, tsize, stride, name_out)
        
# Write index table
f.write("const unsigned short *_"+name_out+"_data[%d] =\n{\n"%(nrow*ncol))
for i in range(0, nrow*ncol):
    f.write("   __"+name_out+"%d_data,\n"%i)
    
f.write("};\n")
f.close()

# Write H file
f = open(name_out+".h",'wb+')
f.write("extern const unsigned short *_"+name_out+"_data[%d];\n"%(nrow*ncol))
f.write("static const unsigned int _"+name_out+"_tw = %d;\n"%tsize[0])
f.write("static const unsigned int _"+name_out+"_th = %d;\n"%tsize[1])
f.close()

    
