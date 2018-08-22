# Mitsuba_clt
mitsuba_clt extends the functionality of mitsuba renderer 0.5.0 by adding perspective projector, orthographic projector, coded perspective camera and coded orthogrpahic camera to it. 

## Gompiling the extended renderer

To compile Mitsuba_clt, you need to follow build instructions inside [Mitusba documentation](https://www.mitsuba-renderer.org/releases/current/documentation.pdf), yet at the step of 
```
hg clone https://www.mitsuba-renderer.org/hg/mitsuba
```
you should clone this repository with following command:
```
git clone https://github.com/cmu-ci-lab/mitsuba_clt.git
```

## Basic usage

To use basic rendering functionality of Mitsuba, you can also look into [Mitusba documentation](https://www.mitsuba-renderer.org/releases/current/documentation.pdf). As for extended functionalities, you can look into following instructions to put projectors and coded cameras into the scene file. If you look for example scene files, there are those modified from Mitsuba scene files with projectors or coded cameras in examples/ directory.  

### Perspective Projector

Perspective projector constructor uses parameters similar to that of perspectivecamera. Specifically, it includes all parameters of perspective camera except for the film. Also, it has extra parameters "filename"(string) and "scale"(float) to specify the image to be projected into the scene and the amount of scale the brightness of the projected image. For details pertaining to scene file creation of perspectiveprojector, please look into the documentation code at the very first few lines of the src/emitters/perspetctiveEmitterImpl.cpp file. 

### Orthographic Projector 

Similar to perspective projector, orthographic projector retains parameters of orthographic camera except for film. Increased parameters are "filename"(string) and "irradiance"(float) which are used to specify projection image and a scale to amount of power per unit area received by a hypothetical surface normal to the specified direction. Further information of orthographic projector scene file creation can be found in src/emitters/orthographicEmitterImpl.cpp

### Coded Perspective Camera and Coded Orthographic Camera

An additional parameter "filename(string)" is added to both coded cameras compared to their versions without masks. The filename is used as a path to the image file that contains the mask that encodes the camera. If the image has resolution inconsistent with the camera film, the mask will be scaled to fit the size of the film. There is no default value for filename, so this is a required parameter for coded cameras. You can view more detailed documentations of constructing coded cameras in src/sensors/codedOrthographic.cpp and src/sensors/codedPerspective.cpp

### "filename" parameter is required for all extensional features and please make sure having them in your scene file for these projectors or cameras. 

