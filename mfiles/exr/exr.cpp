/*
 * openexr_mex.cpp
 *
 *  Created on: May 28, 2012
 *      Author: igkiou
 */

#include "OpenEXR/Iex.h"
#include "OpenEXR/ImathBox.h"
#include "OpenEXR/ImfArray.h"
#include "OpenEXR/ImfAttribute.h"
#include "OpenEXR/ImfBoxAttribute.h"
#include "OpenEXR/ImfChannelList.h"
#include "OpenEXR/ImfChannelListAttribute.h"
#include "OpenEXR/ImfChromaticitiesAttribute.h"
#include "OpenEXR/ImfCompressionAttribute.h"
#include "OpenEXR/ImfDoubleAttribute.h" // new addition
#include "OpenEXR/ImfEnvmapAttribute.h"
#include "OpenEXR/ImfFloatAttribute.h"
//#include "OpenEXR/ImfFloatVectorAttribute.h" // new addition
#include "OpenEXR/ImfFrameBuffer.h"
#include "OpenEXR/ImfIntAttribute.h" // new addition
#include "OpenEXR/ImfLineOrderAttribute.h"
#include "OpenEXR/ImfOutputFile.h"
#include "OpenEXR/ImfPixelType.h"
#include "OpenEXR/ImfRgba.h"
#include "OpenEXR/ImfRgbaFile.h"
#include "OpenEXR/ImfStandardAttributes.h"
#include "OpenEXR/ImfStringAttribute.h"
#include "OpenEXR/ImfStringVectorAttribute.h" // new addition
#include "OpenEXR/ImfTestFile.h"
#include "OpenEXR/ImfVecAttribute.h"
#include "exr.h"


namespace exr {

namespace {

template <typename ExrPixelType>
class ImfPixelType {
public:
	ImfPixelType() :
		m_pixelType() {	}

	inline Imf::PixelType get_pixelType() const {
		return m_pixelType;
	}

	virtual ~ImfPixelType() { }

private:
	Imf::PixelType m_pixelType;
};

template <> inline ImfPixelType<unsigned int>::ImfPixelType() :
	m_pixelType(Imf::UINT) { }

template <> inline ImfPixelType<float>::ImfPixelType() :
	m_pixelType(Imf::FLOAT) { }

}  // namespace


/*
 * Check valid file function.
 */
mex::MxNumeric<bool> isExrFile(const mex::MxString& fileName) {
	return mex::MxNumeric<bool>(Imf::isOpenExrFile(fileName.c_str()));
}

/*
 * Input file handling.
 */
ExrInputFile::ExrInputFile(const mex::MxString& fileName):
						m_file(fileName.c_str()) {
	mexAssert(isValidFile()[0]);
}

mex::MxString ExrInputFile::getFileName() const {
	return mex::MxString(m_file.fileName());
}

mex::MxNumeric<bool> ExrInputFile::isValidFile() const {
	return isExrFile(mex::MxString(m_file.fileName()));
}

int ExrInputFile::getHeight() const {
	Imath::Box2i dw = m_file.header().dataWindow();
	return dw.max.y - dw.min.y + 1;
}

int ExrInputFile::getWidth() const {
	Imath::Box2i dw = m_file.header().dataWindow();
	return dw.max.x - dw.min.x + 1;
}

int ExrInputFile::getNumberOfChannels() const {
	int numChannels = 0;
	for (Imf::ChannelList::ConstIterator
			channelIter = m_file.header().channels().begin(),
			channelEnd = m_file.header().channels().end();
			channelIter != channelEnd;
			++channelIter) {
		++numChannels;
	}
	return numChannels;
}

std::vector<std::string> ExrInputFile::getChannelNames() const {
	std::vector<std::string> channelNames;
	for (Imf::ChannelList::ConstIterator
			channelIter = m_file.header().channels().begin(),
			channelEnd = m_file.header().channels().end();
			channelIter != channelEnd;
			++channelIter) {
		channelNames.push_back(std::string(channelIter.name()));
	}
	return channelNames;
}

bool ExrInputFile::isComplete() const {
	return m_file.isComplete();
}

bool ExrInputFile::hasChannel(const std::string& channelName) const {
	return (m_file.header().channels().findChannel(channelName.c_str()) != nullptr);
}

mex::MxArray ExrInputFile::readDataRGB() {
	std::vector<std::string> channelNameVector;
	channelNameVector.push_back("R");
	channelNameVector.push_back("G");
	channelNameVector.push_back("B");
	return readData(channelNameVector);
}

mex::MxArray ExrInputFile::readDataY() {
	std::vector<std::string> channelNameVector;
	channelNameVector.push_back("Y");
	return readData(channelNameVector);
}

mex::MxArray ExrInputFile::readData(const mex::MxString &channelName) {
	std::vector<std::string> channelNameVector;
	channelNameVector.push_back(channelName.get_string());
	return readData(channelNameVector);
}

mex::MxArray ExrInputFile::readData(const mex::MxCell &channelNames) {
	std::vector<std::string> channelNameVector;
	for (int iterName = 0; iterName < channelNames.getNumberOfElements();
			++iterName) {
		channelNameVector.push_back(
							mex::MxString(channelNames[iterName]).get_string());
	}
	return readData(channelNameVector);
}

mex::MxArray ExrInputFile::readData() {
	std::vector<std::string> channelNameVector = getChannelNames();
	return readData(channelNameVector);
}

mex::MxArray ExrInputFile::readData(
							const std::vector<std::string>& channelNameVector) {
	int width = getWidth();
	int height = getHeight();
	Imath::Box2i dw = m_file.header().dataWindow();
	int numChannels = channelNameVector.size();
	std::vector<int> dimensions;
	dimensions.push_back(height);
	dimensions.push_back(width);
	if (numChannels > 1) {
		dimensions.push_back(numChannels);
	}
	mex::MxNumeric<PixelType> pixelArray(static_cast<int>(dimensions.size()),
										&dimensions[0]);

	Imf::FrameBuffer frameBuffer;
	for (int iterChannel = 0; iterChannel < numChannels; ++iterChannel) {
		mexAssert(hasChannel(channelNameVector[iterChannel]));
		PixelType* pixelBuffer = &pixelArray[iterChannel * width * height];
		frameBuffer.insert(channelNameVector[iterChannel].c_str(),
						Imf::Slice(ImfPixelType<PixelType>().get_pixelType(),
								(char *) (pixelBuffer - dw.min.x * height - dw.min.y * 1),
								sizeof(*pixelBuffer) * height,
								sizeof(*pixelBuffer) * 1,
								1,
								1,
								FLT_MAX));
	}

	mexAssert(isComplete());
	m_file.setFrameBuffer(frameBuffer);
	m_file.readPixels(dw.min.y, dw.max.y);
	return mex::MxArray(pixelArray.get_array());
}

namespace {

enum class EExrAttributeType {
	EBox2f = 0,
	EBox2i,
	EChannelList,
	EChromaticities,
	ECompression,
	EDouble,
	EEnvmap,
	EFloat,
//	EFloatVector,
	EInt,
	ELineOrder,
	EString,
	EStringVector,
	EV2f,
	EV2i,
	ELength,
	EInvalid = -1
};

mex::ConstBiMap<EExrAttributeType, std::string> attributeTypeNameMap =
	mex::ConstBiMap<EExrAttributeType, std::string>
	(EExrAttributeType::EBox2f, std::string(Imf::Box2fAttribute::staticTypeName()))
	(EExrAttributeType::EBox2i, std::string(Imf::Box2iAttribute::staticTypeName()))
	(EExrAttributeType::EChannelList, std::string(Imf::ChannelListAttribute::staticTypeName()))
	(EExrAttributeType::EChromaticities, std::string(Imf::ChromaticitiesAttribute::staticTypeName()))
	(EExrAttributeType::ECompression, std::string(Imf::CompressionAttribute::staticTypeName()))
	(EExrAttributeType::EDouble, std::string(Imf::DoubleAttribute::staticTypeName()))
	(EExrAttributeType::EEnvmap, std::string(Imf::EnvmapAttribute::staticTypeName()))
	(EExrAttributeType::EFloat, std::string(Imf::FloatAttribute::staticTypeName()))
//	(EExrAttributeType::EFloatVector, std::string(Imf::FloatVectorAttribute::staticTypeName()))
	(EExrAttributeType::EInt, std::string(Imf::IntAttribute::staticTypeName()))
	(EExrAttributeType::ELineOrder, std::string(Imf::LineOrderAttribute::staticTypeName()))
	(EExrAttributeType::EString, std::string(Imf::StringAttribute::staticTypeName()))
	(EExrAttributeType::EStringVector, std::string(Imf::StringVectorAttribute::staticTypeName()))
	(EExrAttributeType::EV2f, std::string(Imf::V2fAttribute::staticTypeName()))
	(EExrAttributeType::EV2i, std::string(Imf::V2iAttribute::staticTypeName()))
	(EExrAttributeType::EInvalid, std::string("unknown"));

mex::ConstBiMap<Imf::Compression, std::string> compressionTypeNameMap =
	mex::ConstBiMap<Imf::Compression, std::string>
	(Imf::NO_COMPRESSION, std::string("no"))
	(Imf::RLE_COMPRESSION, std::string("rle"))
	(Imf::ZIPS_COMPRESSION, std::string("zips"))
	(Imf::ZIP_COMPRESSION, std::string("zip"))
	(Imf::PIZ_COMPRESSION, std::string("piz"))
	(Imf::PXR24_COMPRESSION, std::string("pxr24"))
	(Imf::B44_COMPRESSION, std::string("b44"))
	(Imf::B44A_COMPRESSION, std::string("b44a"))
	(Imf::NUM_COMPRESSION_METHODS, std::string("unknown"));

mex::ConstBiMap<Imf::LineOrder, std::string> lineOrderTypeNameMap =
	mex::ConstBiMap<Imf::LineOrder, std::string>
	(Imf::INCREASING_Y, std::string("increasing_y"))
	(Imf::DECREASING_Y,	std::string("decreasing_y"))
	(Imf::RANDOM_Y, std::string("random_y"))
	(Imf::NUM_LINEORDERS, std::string("unknown"));

mex::ConstBiMap<Imf::Envmap, std::string> envmapTypeNameMap =
	mex::ConstBiMap<Imf::Envmap, std::string>
	(Imf::ENVMAP_LATLONG, 	std::string("latlong"))
	(Imf::ENVMAP_CUBE,		std::string("cube"))
	(Imf::NUM_ENVMAPTYPES,	std::string("unknown"));

/*
 * Routines to convert an Attribute to MxArray.
 */

// VT
template <typename T>
mex::MxNumeric<T> toMxArray(
	const Imf::TypedAttribute<T>& attribute) {
	return mex::MxNumeric<T>(attribute.value());
}

// V2T
template <typename T>
mex::MxNumeric<T> toMxArray(
	const Imf::TypedAttribute<Imath::Vec2<T> >& attribute) {
	std::vector<T> temp;
	temp.push_back(attribute.value().x);
	temp.push_back(attribute.value().y);
	return mex::MxNumeric<T>(temp);
}

// Box2T
template <typename T>
mex::MxStruct toMxArray(
	const Imf::TypedAttribute<Imath::Box<Imath::Vec2<T> > >& attribute) {
	std::vector<std::string> boxNames;
	std::vector<mex::MxArray *> boxMx;

	/* min */
	boxNames.push_back(std::string("min"));
	std::vector<T> min;
	min.push_back(attribute.value().min.x);
	min.push_back(attribute.value().min.y);
	mex::MxNumeric<T> minMx(min);
	boxMx.push_back(&minMx);

	/* max */
	boxNames.push_back(std::string("max"));
	std::vector<T> max;
	max.push_back(attribute.value().max.x);
	max.push_back(attribute.value().max.y);
	mex::MxNumeric<T> maxMx(max);
	boxMx.push_back(&maxMx);

	return mex::MxStruct(boxNames, boxMx);
}

// Chromaticities
mex::MxStruct toMxArray(
	const Imf::TypedAttribute<Imf::Chromaticities>& attribute) {
	std::vector<std::string> chromaticityNames;
	std::vector<mex::MxArray*> chromaticityValues;

	/* red */
	chromaticityNames.push_back(std::string("red"));
	std::vector<float> red;
	red.push_back(attribute.value().red.x);
	red.push_back(attribute.value().red.y);
	mex::MxNumeric<float> redMx(red);
	chromaticityValues.push_back(&redMx);

	/* green */
	chromaticityNames.push_back(std::string("green"));
	std::vector<float> green;
	green.push_back(attribute.value().green.x);
	green.push_back(attribute.value().green.y);
	mex::MxNumeric<float> greenMx(green);
	chromaticityValues.push_back(&greenMx);

	/* blue */
	chromaticityNames.push_back(std::string("blue"));
	std::vector<float> blue;
	blue.push_back(attribute.value().blue.x);
	blue.push_back(attribute.value().blue.y);
	mex::MxNumeric<float> blueMx(blue);
	chromaticityValues.push_back(&blueMx);

	/* white */
	chromaticityNames.push_back(std::string("white"));
	std::vector<float> white;
	white.push_back(attribute.value().white.x);
	white.push_back(attribute.value().white.y);
	mex::MxNumeric<float> whiteMx(white);
	chromaticityValues.push_back(&whiteMx);

	return mex::MxStruct(chromaticityNames, chromaticityValues);
}

// String
mex::MxString toMxArray(
	const Imf::TypedAttribute<std::string>& attribute) {
	return mex::MxString(attribute.value());
}

// Envmap
mex::MxString toMxArray(
	const Imf::TypedAttribute<Imf::Envmap>& attribute) {
	return mex::MxString(envmapTypeNameMap[attribute.value()]);
}

// LineOrder
mex::MxString toMxArray(
	const Imf::TypedAttribute<Imf::LineOrder>& attribute) {
	return mex::MxString(lineOrderTypeNameMap[attribute.value()]);
}

// Compression
mex::MxString toMxArray(
	const Imf::TypedAttribute<Imf::Compression>& attribute) {
	return mex::MxString(compressionTypeNameMap[attribute.value()]);
}

// ChannelList
mex::MxCell toMxArray(
	const Imf::TypedAttribute<Imf::ChannelList>& attribute) {
	std::vector<mex::MxArray*> channelNames;
	for (Imf::ChannelList::ConstIterator iter = attribute.value().begin(),
		end = attribute.value().end();
		iter != end;
		++iter) {
		channelNames.push_back(new mex::MxString(iter.name()));
	}
	mex::MxCell retArg(channelNames);
	for (int iter = 0, numChannels = channelNames.size();
		iter < numChannels;
		++iter) {
		delete channelNames[iter];
	}
	return retArg;
}

// VectorT
template <typename T>
mex::MxNumeric<T> toMxArray(
	const Imf::TypedAttribute<std::vector<T> >& attribute) {
	return mex::MxNumeric<T>(attribute.value());
}

// VectorString
mex::MxCell toMxArray(
	const Imf::TypedAttribute<std::vector<std::string> >& attribute) {
	std::vector<mex::MxArray*> stringVector;
	for (std::vector<std::string>::const_iterator iter = attribute.value().begin(),
			end = attribute.value().end();
			iter != end;
			++iter) {
		stringVector.push_back(new mex::MxString(*iter));
	}
	mex::MxCell retArg(stringVector);
	for (int iter = 0, numStrings = stringVector.size();
			iter < numStrings;
			++iter) {
		delete stringVector[iter];
	}
	return retArg;
}

} /* namespace */

mex::MxArray ExrInputFile::getAttribute(const mex::MxString& attributeName) const {
	return getAttribute(attributeName.get_string());
}

mex::MxArray ExrInputFile::getAttribute() const {
	std::vector<std::string> nameVec;
	std::vector<mex::MxArray*> arrayVec;
	for (Imf::Header::ConstIterator iter = m_file.header().begin(),
			end = m_file.header().end();
			iter != end;
			++iter) {
		nameVec.push_back(std::string(iter.name()));
		mex::MxArray* tempAttributeArray = new mex::MxArray(
								getAttribute(std::string(iter.name()))
								.get_array());
		arrayVec.push_back(tempAttributeArray);
	}
	mex::MxArray retArg(mex::MxStruct(nameVec, arrayVec).get_array());
	for (int iter = 0, numAttributes = arrayVec.size();
		iter < numAttributes;
		++iter) {
		delete arrayVec[iter];
	}
	return retArg;
}

mex::MxArray ExrInputFile::getAttribute(const std::string& attributeName) const {
	const Imf::Attribute& attribute = m_file.header()[attributeName.c_str()];
	const EExrAttributeType type = attributeTypeNameMap.find(std::string(attribute.typeName()));
	switch(type) {
		case EExrAttributeType::EBox2f: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imath::Box2f>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EBox2i: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imath::Box2i>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EChannelList: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imf::ChannelList>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EChromaticities: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imf::Chromaticities>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::ECompression: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imf::Compression>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EDouble: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<double>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EEnvmap: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imf::Envmap>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EFloat: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<float>&>(
													attribute)).get_array());
		}
//		case EExrAttributeType::EFloatVector: {
//			return mex::MxArray(toMxArray(
//					static_cast<const Imf::TypedAttribute<std::vector<float> >&>(
//													attribute)).get_array());
//		}
		case EExrAttributeType::EInt: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<int>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::ELineOrder: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imf::LineOrder>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EString: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<std::string>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EStringVector: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<std::vector<std::string> >&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EV2f: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imath::V2f>&>(
													attribute)).get_array());
		}
		case EExrAttributeType::EV2i: {
			return mex::MxArray(toMxArray(
					static_cast<const Imf::TypedAttribute<Imath::V2i>&>(
													attribute)).get_array());
		}
		default: {
			mexAssertEx(0, "Unknown attribute type");
			return mex::MxArray();
		}
	}
}

/*
 * Output file handling.
 */
ExrOutputFile::ExrOutputFile(const mex::MxString& fileName, int width,
							int height):
						  m_header(width, height),
						  m_fileName(fileName.get_string()),
						  m_writtenFile(false) {	}

mex::MxString ExrOutputFile::getFileName() const {
	return mex::MxString(m_fileName);
}

int ExrOutputFile::getHeight() const {
	Imath::Box2i dw = m_header.dataWindow();
	return dw.max.y - dw.min.y + 1;
}

int ExrOutputFile::getWidth() const {
	Imath::Box2i dw = m_header.dataWindow();
	return dw.max.x - dw.min.x + 1;
}

void ExrOutputFile::writeDataRGB(const mex::MxArray& rgbPixels) {
	std::vector<std::string> channelNameVector;
	channelNameVector.push_back(std::string("R"));
	channelNameVector.push_back(std::string("G"));
	channelNameVector.push_back(std::string("B"));
	writeData(channelNameVector, rgbPixels);
}

void ExrOutputFile::writeDataY(const mex::MxArray& yPixels) {
	std::vector<std::string> channelNameVector;
	channelNameVector.push_back(std::string("Y"));
	writeData(channelNameVector, yPixels);
}

void ExrOutputFile::writeData(const mex::MxString& channelName,
							const mex::MxArray& channelPixels) {
	std::vector<std::string> channelNameVector;
	channelNameVector.push_back(channelName.get_string());
	writeData(channelNameVector, channelPixels);
}

void ExrOutputFile::writeData(const mex::MxArray& channelPixels) {
	std::vector<int> dimensions = channelPixels.getDimensions();
	int numChannels = (dimensions.size() == 2)?(1):(dimensions[2]);
	std::vector<std::string> channelNameVector;
	for (int iterChannel = 0; iterChannel < numChannels; ++iterChannel) {
		std::stringstream temp;
		temp << iterChannel;
		channelNameVector.push_back(temp.str());
	}
	writeData(channelNameVector, channelPixels);
}

void ExrOutputFile::writeData(const mex::MxCell& channelNames,
							const mex::MxArray& channelPixels) {
	std::vector<std::string> channelNameVector;
	for (int iterName = 0; iterName < channelNames.getNumberOfElements();
			++iterName) {
		channelNameVector.push_back(
							mex::MxString(channelNames[iterName]).get_string());
	}
	writeData(channelNameVector, channelPixels);
}

void ExrOutputFile::writeData(const std::vector<std::string>& channelNameVector,
							const mex::MxArray& channelPixels) {
	mexAssert(!m_writtenFile);
	mex::MxNumeric<PixelType> pixelArray(channelPixels.get_array());
	std::vector<int> dimensions = pixelArray.getDimensions();
	int width = getWidth();
	int height = getHeight();
	int numChannels = channelNameVector.size();
	mexAssert((((numChannels == 1) && (dimensions.size() == 2)) ||
					((dimensions.size() == 3) && (dimensions[2] == numChannels))) &&
					(dimensions[0] == height) &&
					(dimensions[1] == width));

	Imf::FrameBuffer frameBuffer;
	for (int iterChannel = 0; iterChannel < numChannels; ++iterChannel) {
		m_header.channels().insert(channelNameVector[iterChannel].c_str(),
								Imf::Channel(ImfPixelType<PixelType>().get_pixelType()));
		PixelType* pixelBuffer = &pixelArray[iterChannel * width * height];
		frameBuffer.insert(channelNameVector[iterChannel].c_str(),
							Imf::Slice(ImfPixelType<PixelType>().get_pixelType(),
									(char *) pixelBuffer,
									sizeof(PixelType) * height,
									sizeof(PixelType) * 1));
	}

	Imf::OutputFile outFile(m_fileName.c_str(), m_header);
	outFile.setFrameBuffer(frameBuffer);
	outFile.writePixels(height);
	m_writtenFile = true;
}

namespace {

/*
 * Routines to convert an MxArray to Attribute.
 */

/*
 * Some empty templates to allow  for return type overloading.
 */
template <typename T>
Imf::TypedAttribute<T> toAttribute(const mex::MxNumeric<double>& mxNumeric) {
	mexAssertEx(0, "This template specialization should never be used");
}

template <typename T>
Imf::TypedAttribute<T> toAttribute(const mex::MxNumeric<float>& mxNumeric) {
	mexAssertEx(0, "This template specialization should never be used");
}

template <typename T>
Imf::TypedAttribute<T> toAttribute(const mex::MxNumeric<int>& mxNumeric) {
	mexAssertEx(0, "This template specialization should never be used");
}

template <typename T>
Imf::TypedAttribute<T> toAttribute(const mex::MxCell& mxCell) {
	mexAssertEx(0, "This template specialization should never be used");
}

template <typename T>
Imf::TypedAttribute<T> toAttribute(const mex::MxStruct& mxStruct) {
	mexAssertEx(0, "This template specialization should never be used");
}

template <typename T>
Imf::TypedAttribute<T> toAttribute(const mex::MxString& mxString) {
	mexAssertEx(0, "This template specialization should never be used");
}

// VT
template <>
Imf::TypedAttribute<double> toAttribute(
	const mex::MxNumeric<double>& mxNumeric) {
	return Imf::TypedAttribute<double>(mxNumeric[0]);
}

template <>
Imf::TypedAttribute<float> toAttribute(
	const mex::MxNumeric<float>& mxNumeric) {
	return Imf::TypedAttribute<float>(mxNumeric[0]);
}

template <>
Imf::TypedAttribute<int> toAttribute(
	const mex::MxNumeric<int>& mxNumeric) {
	return Imf::TypedAttribute<int>(mxNumeric[0]);
}

// V2T
template <>
Imf::TypedAttribute<Imath::Vec2<float> > toAttribute(
	const mex::MxNumeric<float>& mxNumeric) {
	return Imf::TypedAttribute<Imath::Vec2<float> >(
											Imath::Vec2<float>(mxNumeric[0],
														mxNumeric[1]));
}

template <>
Imf::TypedAttribute<Imath::Vec2<int> > toAttribute(
	const mex::MxNumeric<int>& mxNumeric) {
	return Imf::TypedAttribute<Imath::Vec2<int> >(
											Imath::Vec2<int>(mxNumeric[0],
														mxNumeric[1]));
}

// Box2T
template <>
Imf::TypedAttribute<Imath::Box<Imath::Vec2<float> > > toAttribute(
												const mex::MxStruct& mxStruct) {
	mex::MxNumeric<float> minMx = mex::MxNumeric<float>(mxStruct[std::string("min")]);
	Imath::Vec2<float> min(minMx[0], minMx[1]);
	mex::MxNumeric<float> maxMx = mex::MxNumeric<float>(mxStruct[std::string("max")]);
	Imath::Vec2<float> max(maxMx[0], maxMx[1]);
	return Imf::TypedAttribute<Imath::Box<Imath::Vec2<float> > >(
								Imath::Box<Imath::Vec2<float> >(min, max));
}

template <>
Imf::TypedAttribute<Imath::Box<Imath::Vec2<int> > > toAttribute(
												const mex::MxStruct& mxStruct) {
	mex::MxNumeric<int> minMx = mex::MxNumeric<int>(mxStruct[std::string("min")]);
	Imath::Vec2<int> min(minMx[0], minMx[1]);
	mex::MxNumeric<int> maxMx = mex::MxNumeric<int>(mxStruct[std::string("max")]);
	Imath::Vec2<int> max(maxMx[0], maxMx[1]);
	return Imf::TypedAttribute<Imath::Box<Imath::Vec2<int> > >(
								Imath::Box<Imath::Vec2<int> >(min, max));
}

// VectorT
//template <>
//Imf::TypedAttribute<std::vector<float> > toAttribute(
//									const mex::MxNumeric<float>& mxNumeric) {
//	return Imf::TypedAttribute<std::vector<float> >(mxNumeric.vectorize());
//}

// VectorString
template <>
Imf::TypedAttribute<std::vector<std::string> > toAttribute(
													const mex::MxCell& mxCell) {
	int numStrings = mxCell.size();
	std::vector<std::string> stringVector;
	for (int iter = 0; iter < numStrings; ++iter) {
		stringVector.push_back(mex::MxString(mxCell[0]).get_string());
	}
	return Imf::TypedAttribute<std::vector<std::string> >(stringVector);
}

// Chromaticities
template <>
Imf::TypedAttribute<Imf::Chromaticities> toAttribute(
												const mex::MxStruct& mxStruct) {
	mex::MxNumeric<float> redMx(mxStruct[std::string("red")]);
	Imath::V2f red(redMx[0], redMx[1]);
	mex::MxNumeric<float> greenMx(mxStruct[std::string("green")]);
	Imath::V2f green(greenMx[0], greenMx[1]);
	mex::MxNumeric<float> blueMx(mxStruct[std::string("blue")]);
	Imath::V2f blue(blueMx[0], blueMx[1]);
	mex::MxNumeric<float> whiteMx(mxStruct[std::string("white")]);
	Imath::V2f white(whiteMx[0], whiteMx[1]);
	return Imf::Chromaticities(red, green, blue, white);
}

// String
template <>
Imf::TypedAttribute<std::string> toAttribute(const mex::MxString& mxString) {
	return Imf::TypedAttribute<std::string>(mxString.get_string());
}

// Envmap
template <>
Imf::TypedAttribute<Imf::Envmap> toAttribute(const mex::MxString& mxString) {
	return Imf::TypedAttribute<Imf::Envmap>(envmapTypeNameMap.find(
														mxString.get_string()));
}

// LineOrder
template <>
Imf::TypedAttribute<Imf::LineOrder> toAttribute(const mex::MxString& mxString) {
	return Imf::TypedAttribute<Imf::LineOrder>(lineOrderTypeNameMap.find(
														mxString.get_string()));
}

// Compression
template <>
Imf::TypedAttribute<Imf::Compression> toAttribute(const mex::MxString& mxString) {
	return Imf::TypedAttribute<Imf::Compression>(compressionTypeNameMap.find(
														mxString.get_string()));
}

// ChannelList
// Not supported.

} /* namespace */

const mex::ConstMap<std::string, EExrAttributeType> registeredAttributeNameAttributeTypeMap
	= mex::ConstMap<std::string, EExrAttributeType>
	(std::string("gain"),				EExrAttributeType::EFloat)
	(std::string("wavelength"),			EExrAttributeType::EFloat)
//	(std::string("wavelengths"),		EExrAttributeType::EFloatVector)
//	(std::string("scanSteps"),			EExrAttributeType::EFloatVector)
	(std::string("extTube"),			EExrAttributeType::EString)
	(std::string("lens"),				EExrAttributeType::EString)
	(std::string("material"),			EExrAttributeType::EString)
	(std::string("chromaticities"),		EExrAttributeType::EChromaticities)
	(std::string("whiteLuminance"),		EExrAttributeType::EFloat)
	(std::string("adoptedNeutral"),		EExrAttributeType::EV2f)
	(std::string("renderingTransform"),	EExrAttributeType::EString)
	(std::string("lookModTransform"),	EExrAttributeType::EFloat)
	(std::string("xDensity"),			EExrAttributeType::EFloat)
	(std::string("owner"),				EExrAttributeType::EString)
	(std::string("comments"),			EExrAttributeType::EString)
	(std::string("capDate"),			EExrAttributeType::EString)
	(std::string("utcOffset"),			EExrAttributeType::EFloat)
	(std::string("longitude"),			EExrAttributeType::EFloat)
	(std::string("latitude"),			EExrAttributeType::EFloat)
	(std::string("altitude"),			EExrAttributeType::EFloat)
	(std::string("focus"),				EExrAttributeType::EFloat)
	(std::string("expTime"),			EExrAttributeType::EFloat)
//	(std::string("exposures"),			EExrAttributeType::EFloatVector)
	(std::string("multiView"),			EExrAttributeType::EStringVector)
	(std::string("aperture"),			EExrAttributeType::EFloat)
	(std::string("isoSpeed"),			EExrAttributeType::EFloat)
	(std::string("envmap"),				EExrAttributeType::EEnvmap);


void ExrOutputFile::setAttribute(const mex::MxString& attributeName,
										const mex::MxArray& attribute) {
	setAttribute(attributeName.get_string(), attribute);
}

void ExrOutputFile::setAttribute(const mex::MxStruct& attributes) {
	for (int iter = 0, numFields = attributes.getNumberOfFields();
		iter < numFields;
		++iter) {
		setAttribute(attributes.getFieldName(iter),
					mex::MxArray(attributes[iter]));
	}
}

void ExrOutputFile::setAttribute(const std::string& attributeName,
								const mex::MxArray& attribute) {
	std::map<std::string, EExrAttributeType>::const_iterator iteratorToType =
											registeredAttributeNameAttributeTypeMap.
											get_map().
											find(attributeName);
	const EExrAttributeType type(
				(iteratorToType != registeredAttributeNameAttributeTypeMap.get_map().end())
				?(iteratorToType->second)
				:(EExrAttributeType::EString));
	switch(type) {
		case EExrAttributeType::EBox2f: {
			const mex::MxStruct tempArray(attribute.get_array());
			mexAssert((tempArray.getNumberOfFields() == 1) &&
					((tempArray.isField(std::string("min"))) &&
					(mex::MxNumeric<float>(tempArray[std::string("min")]).getNumberOfElements() == 2)) &&
					((tempArray.isField(std::string("max"))) &&
					(mex::MxNumeric<float>(tempArray[std::string("max")]).getNumberOfElements() == 2)));
			m_header.insert(attributeName.c_str(),
					toAttribute<Imath::Box<Imath::Vec2<float> > >(tempArray));
			break;
		}
		case EExrAttributeType::EBox2i: {
			const mex::MxStruct tempArray(attribute.get_array());
			mexAssert((tempArray.getNumberOfFields() == 1) &&
					((tempArray.isField(std::string("min"))) &&
					(mex::MxNumeric<int>(tempArray[std::string("min")]).getNumberOfElements() == 2)) &&
					((tempArray.isField(std::string("max"))) &&
					(mex::MxNumeric<int>(tempArray[std::string("max")]).getNumberOfElements() == 2)));
			m_header.insert(attributeName.c_str(),
					toAttribute<Imath::Box<Imath::Vec2<int> > >(tempArray));
			break;
		}
		case EExrAttributeType::EChannelList: {
			mexAssertEx(0, "Unsupported attribute type");
			break;
		}
		case EExrAttributeType::EChromaticities: {
			const mex::MxStruct tempArray(attribute.get_array());
			mexAssert((tempArray.getNumberOfFields() == 4) &&
					((tempArray.isField(std::string("red"))) &&
					(mex::MxNumeric<float>(tempArray[std::string("red")]).getNumberOfElements() == 2)) &&
					((tempArray.isField(std::string("green"))) &&
					(mex::MxNumeric<float>(tempArray[std::string("green")]).getNumberOfElements() == 2)) &&
					((tempArray.isField(std::string("blue"))) &&
					(mex::MxNumeric<float>(tempArray[std::string("blue")]).getNumberOfElements() == 2)) &&
					((tempArray.isField(std::string("white"))) &&
					(mex::MxNumeric<float>(tempArray[std::string("white")]).getNumberOfElements() == 2)));
			m_header.insert(attributeName.c_str(),
					toAttribute<Imf::Chromaticities>(tempArray));
			break;
		}
		case EExrAttributeType::ECompression: {
			const mex::MxString tempArray(attribute.get_array());
			m_header.insert(attributeName.c_str(),
					toAttribute<Imf::Compression>(tempArray));
			break;
		}
		case EExrAttributeType::EDouble: {
			const mex::MxNumeric<double> tempArray(attribute.get_array());
			mexAssert(tempArray.getNumberOfElements() == 1);
			m_header.insert(attributeName.c_str(),
					toAttribute<double>(tempArray));
			break;
		}
		case EExrAttributeType::EEnvmap: {
			const mex::MxString tempArray(attribute.get_array());
			m_header.insert(attributeName.c_str(),
					toAttribute<Imf::Envmap>(tempArray));
			break;
		}
		case EExrAttributeType::EFloat: {
			const mex::MxNumeric<float> tempArray(attribute.get_array());
			mexAssert(tempArray.getNumberOfElements() == 1);
			m_header.insert(attributeName.c_str(),
					toAttribute<float>(tempArray));
			break;
		}
//		case EExrAttributeType::EFloatVector: {
//			const mex::MxNumeric<float> tempArray(attribute.get_array());
//			m_header.insert(attributeName.c_str(),
//					toAttribute<std::vector<float> >(tempArray));
//			break;
//		}
		case EExrAttributeType::EInt: {
			const mex::MxNumeric<int> tempArray(attribute.get_array());
			mexAssert(tempArray.getNumberOfElements() == 1);
			m_header.insert(attributeName.c_str(),
					toAttribute<int>(tempArray));
			break;
		}
		case EExrAttributeType::ELineOrder: {
			const mex::MxString tempArray(attribute.get_array());
			m_header.insert(attributeName.c_str(),
					toAttribute<Imf::LineOrder>(tempArray));
			break;
		}
		case EExrAttributeType::EString: {
			const mex::MxString tempArray(attribute.get_array());
			m_header.insert(attributeName.c_str(),
					toAttribute<std::string>(tempArray));
			break;
		}
		case EExrAttributeType::EStringVector: {
			const mex::MxCell tempArray(attribute.get_array());
			m_header.insert(attributeName.c_str(),
					toAttribute<std::vector<std::string> >(tempArray));
			break;
		}
		case EExrAttributeType::EV2f: {
			const mex::MxNumeric<float> tempArray(attribute.get_array());
			mexAssert(tempArray.getNumberOfElements() == 2);
			m_header.insert(attributeName.c_str(),
					toAttribute<Imath::Vec2<float> >(tempArray));
			break;
		}
		case EExrAttributeType::EV2i: {
			const mex::MxNumeric<int> tempArray(attribute.get_array());
			mexAssert(tempArray.getNumberOfElements() == 2);
			m_header.insert(attributeName.c_str(),
					toAttribute<Imath::Vec2<int> >(tempArray));
			break;
		}
		default: {
			mexAssertEx(0, "Unknown attribute type");
			break;
		}
	}
}

}  // namespace exr
