/*
 * exrwrite.cpp
 *
 *  Created on: Jun 14, 2011
 *      Author: igkiou
 */

#include "mex_utils.h"

#include "exr.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	(void) plhs;
	/* Check number of input arguments */
	if (nrhs > 4) {
		mexErrMsgTxt("Four or fewer input arguments are required.");
	} else if (nrhs < 2) {
		mexErrMsgTxt("At least two input arguments are required.");
	}

	/* Check number of output arguments */
	if (nlhs > 1) {
		mexErrMsgTxt("Too many output arguments.");
	}

	const mex::MxArray image(const_cast<mxArray*>(prhs[0]));
	std::vector<int> dimensions = image.getDimensions();
	mexAssert((dimensions.size() == 2) || (dimensions.size() == 3));
	mex::MxString fileName(mex::MxString(const_cast<mxArray*>(prhs[1])));
	exr::ExrOutputFile file(fileName, dimensions[1], dimensions[0]);

	if (nrhs >= 4) {
		mex::MxStruct attributes(const_cast<mxArray*>(prhs[3]));
		file.setAttribute(attributes);
	}

	int numChannels = (dimensions.size() == 2)?(1):(dimensions[2]);
	if ((nrhs >= 3) && (!mex::MxArray(const_cast<mxArray*>(prhs[2])).isEmpty())) {
		mex::MxCell channelNames(const_cast<mxArray*>(prhs[2]));
		mexAssert(numChannels == channelNames.getNumberOfElements());
		file.writeData(channelNames, image);
	} else {
		if (numChannels == 1) {
			file.writeDataY(image);
		} else if (numChannels == 3) {
			file.writeDataRGB(image);
		} else {
			file.writeData(image);
		}
	}
}
