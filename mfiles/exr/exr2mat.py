import OpenEXR, Imath
import re
from copy import deepcopy
import numpy as np 
from PIL import Image
from scipy.io import savemat
import sys

inputFile = OpenEXR.InputFile(sys.argv[1])
pixelType = Imath.PixelType(Imath.PixelType.HALF)
dataWin = inputFile.header()['dataWindow']
imgSize = (dataWin.max.x - dataWin.min.x + 1, dataWin.max.y - dataWin.min.y + 1)
tmp = list(inputFile.header()['channels'].keys())

if(len(tmp) != 3):
    prog = re.compile(r"\d+")
    channels = np.array(np.argsort([int(re.match(prog, x).group(0)) for x in tmp], -1, 'stable'))
    channels[0::3], channels[2::3] = deepcopy(channels[2::3]),deepcopy(channels[0::3])
    tmp = np.array(tmp)
    tmp = tmp[list(channels)]
else:
    tmp = np.array(tmp)
    tmp[0], tmp[2] = tmp[2], tmp[0]

video = np.zeros((imgSize[1],imgSize[0],len(tmp)))

index = 0
print(len(tmp))

for x in np.nditer(tmp): 
    print(index)
    imageStr = inputFile.channel(str(x),pixelType)
    image = np.reshape(np.frombuffer(imageStr, dtype=np.float16), imgSize)
    video[:,:,index] = image
    index+=1
savemat(sys.argv[2]+'.mat',{'I':video})
