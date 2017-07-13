/*
 * exrget.cpp
 *
 *  Created on: May 22, 2012
 *      Author: igkiou
 */

#include "mex_utils.h"

#include "exr.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	/* Check number of input arguments */
	if (nrhs < 1) {
		mexErrMsgTxt("At least one input argument is required.");
	} else if (nrhs > 2) {
		mexErrMsgTxt("Two or fewer input arguments are required.");
	}

	/* Check number of output arguments */
	if (nlhs > 1) {
		mexErrMsgTxt("Too many output arguments.");
	}

	exr::ExrInputFile file(mex::MxString(const_cast<mxArray*>(prhs[0])));
	if (nrhs >= 2) {
		mex::MxString attributeName(const_cast<mxArray*>(prhs[1]));
		plhs[0] = file.getAttribute(attributeName).get_array();
	} else {
		plhs[0] = file.getAttribute().get_array();
	}
}
