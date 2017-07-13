/*
 * image_utils//image_utils/openexr_mex/isexr.cpp/isexr.cpp
 *
 *  Created on: Mar 13, 2013
 *      Author: igkiou
 */

#include "mex_utils.h"

#include "exr.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	/* Check number of input arguments */
	if (nrhs != 1) {
		mexErrMsgTxt("Exactly one input argument is required.");
	}

	/* Check number of output arguments */
	if (nlhs > 1) {
		mexErrMsgTxt("Too many output arguments.");
	}

	plhs[0] = exr::isExrFile(
					mex::MxString(const_cast<mxArray*>(prhs[0]))).get_array();
}
