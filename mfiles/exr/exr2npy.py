from tkinter.tix import Tree
import OpenEXR, Imath
import re
from copy import deepcopy
import numpy as np 
from PIL import Image
from scipy.io import savemat
import sys
import cv2

# Reset to false if using google colab to avoid opencv window imshow call
viz_output = True

# Set to false if no tone mapping is required
tone_map = True

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

video = inputFile.channels(tmp, pixelType)


video_rgb = []
for i in range(len(video)):
    img_ = np.reshape(np.frombuffer(video[i], dtype=np.float16), imgSize)
    if i%3 == 0:
        rgb = np.zeros((imgSize[1],imgSize[0],3))
        rgb[:,:,0] = img_
    
    elif i%3 == 1:
        rgb[:,:,1] = img_
    
    else:
        rgb[:,:,2] = img_

        rgb = rgb[:,:,::-1].astype(np.float32)
        video_rgb.append(rgb)

output_path = sys.argv[1][:-4]

# Refer to https://learnopencv.com/high-dynamic-range-hdr-imaging-using-opencv-cpp-python/ for better understanding of parameters
# You may need to change the parameters for cv2.createTonemapReinhard().
if(len(tmp) == 3):
    tonemapDurand = cv2.createTonemapReinhard(1.5, 1.0,0.5,0.2)
else:
    tonemapDurand = cv2.createTonemapReinhard(0.2, 0.0,0,0)

all_transients = []
for frame in video_rgb:
    if tone_map:
        frame = tonemapDurand.process(frame)
    if viz_output:
        cv2.imshow("img", frame/frame.max())
        cv2.waitKey(0)
    all_transients.append(frame/frame.max())

all_transients = np.array(all_transients)
np.save(output_path+"_all_transients.npy", all_transients)
