/*
 * image_utils//image_utils/include/file.h
 *
 *  Created on: Feb 11, 2015
 *      Author: igkiou
 */

#ifndef FILE_MEX_H_
#define FILE_MEX_H_

#include "mex_utils.h"

namespace file {

class InputFileInterface {
public:
	InputFileInterface() = default;
	InputFileInterface(const InputFileInterface& other) = delete;
	InputFileInterface& operator=(const InputFileInterface& other) = delete;

	virtual mex::MxString getFileName() const = 0;
	virtual mex::MxNumeric<bool> isValidFile() const = 0;
	virtual int getHeight() const = 0;
	virtual int getWidth() const = 0;
	virtual int getNumberOfChannels() const = 0;
	virtual mex::MxArray getAttribute(const mex::MxString& attributeName)
									const = 0;
	virtual mex::MxArray getAttribute() const = 0;
	virtual mex::MxArray readData() = 0;
	virtual ~InputFileInterface() = default;
};

class OutputFileInterface {
public:
	OutputFileInterface() = default;
	OutputFileInterface(const OutputFileInterface& other) = delete;
	OutputFileInterface& operator=(const OutputFileInterface& other) = delete;

	virtual mex::MxString getFileName() const = 0;
	virtual int getHeight() const = 0;
	virtual int getWidth() const = 0;
	virtual void setAttribute(const mex::MxString& attributeName,
							const mex::MxArray& attribute) = 0;
	virtual void setAttribute(const mex::MxStruct& attributes) = 0;
	virtual void writeData(const mex::MxArray& data) = 0;
	virtual ~OutputFileInterface() = default;
};

}  // namespace file

#endif /* FILE_MEX_H_ */
