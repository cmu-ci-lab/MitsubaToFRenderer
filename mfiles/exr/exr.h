/*
 * exr_mex.h
 *
 *  Created on: May 23, 2012
 *      Author: igkiou
 */

#ifndef EXR_MEX_H_
#define EXR_MEX_H_

#include <vector>
#include <string>

#include "OpenEXR/ImfHeader.h"
#include "OpenEXR/ImfInputFile.h"

#include "../include/file.h"

namespace exr {

/*
 * TODO: Maybe add support for preview images?
 * TODO: Add general support for alpha channel?
 * TODO: Move EExrAttributeType enum and registeredAttributeNameAttributeTypeMap
 * in header, to allow for easy expansion of registered attributes.
 * TODO: Provide better exposure of registered attributes and their types to
 * user.
 * TODO: Provide set attribute variant that allows specifying type of attribute.
 */

using PixelType = float;

mex::MxNumeric<bool> isExrFile(const mex::MxString& fileName);

class ExrInputFile : public file::InputFileInterface {
public:
	explicit ExrInputFile(const mex::MxString& fileName);

	mex::MxString getFileName() const override;
	mex::MxNumeric<bool> isValidFile() const override;

	int getHeight() const override;
	int getWidth() const override;
	int getNumberOfChannels() const override;
	mex::MxArray getAttribute(const mex::MxString& attributeName) const override;
	mex::MxArray getAttribute() const override;
	mex::MxArray readData() override;

	mex::MxArray readDataRGB();
	mex::MxArray readDataY();
	mex::MxArray readData(const mex::MxString& channelName);
	mex::MxArray readData(const mex::MxCell& channelNames);

	/*
	 * TODO: Should be made private.
	 */
	bool hasChannel(const std::string& channelName) const;

	~ExrInputFile() override = default;

private:
	std::vector<std::string> getChannelNames() const;
	bool isComplete() const;
	mex::MxArray readData(const std::vector<std::string>& channelNameVector);
	mex::MxArray getAttribute(const std::string& attributeName) const;

	Imf::InputFile m_file;
};

class ExrOutputFile : public file::OutputFileInterface {
public:
	ExrOutputFile(const mex::MxString& fileName, int width, int height);

	mex::MxString getFileName() const override;
	int getHeight() const override;
	int getWidth() const override;

	void setAttribute(const mex::MxString& attributeName,
						const mex::MxArray& attribute) override;
	void setAttribute(const mex::MxStruct& attributes) override;

	void writeData(const mex::MxArray& data) override;
	void writeDataRGB(const mex::MxArray& data);
	void writeDataY(const mex::MxArray& data);
	void writeData(const mex::MxString& channelName, const mex::MxArray& data);
	void writeData(const mex::MxCell& channelNames,
				const mex::MxArray& data);

	~ExrOutputFile() override = default;

private:
	void writeData(const std::vector<std::string>& channelNameVector,
				const mex::MxArray& data);
	void setAttribute(const std::string& attributeName,
					const mex::MxArray& attribute);

	Imf::Header m_header;
	std::string m_fileName;
	bool m_writtenFile;
};

}	/* namespace exr */

#endif /* EXR_MEX_H_ */
