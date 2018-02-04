/*
    This file is part of Mitsuba, a physically based rendering system.

    Copyright (c) 2007-2014 by Wenzel Jakob and others.

    Mitsuba is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Mitsuba is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <mitsuba/core/bitmap.h>
#include <mitsuba/core/fstream.h>
#include "bdpt_wr.h"

MTS_NAMESPACE_BEGIN

/* ==================================================================== */
/*                             Work result                              */
/* ==================================================================== */

BDPTWorkResult::BDPTWorkResult(const BDPTConfiguration &conf,
		const ReconstructionFilter *rfilter, Vector2i blockSize) {
	/* Stores the 'camera image' -- this can be blocked when
	   spreading out work to multiple workers */
	if (blockSize == Vector2i(-1, -1))
		blockSize = Vector2i(conf.blockSize, conf.blockSize);

	m_decompositionMaxBound = conf.m_decompositionMaxBound;
	m_decompositionMinBound = conf.m_decompositionMinBound;
	m_decompositionBinWidth = conf.m_decompositionBinWidth;
	m_decompositionType = conf.m_decompositionType;
	m_frames = conf.m_frames;
	m_subSamples = conf.m_subSamples;

	m_modulationType = conf.m_modulationType;
	m_lambda  		 = conf.m_lambda;
	m_phase  		 = conf.m_phase;
	m_P  		 	 = conf.m_P;
	m_neighbors 	 = conf.m_neighbors;
	if( (m_decompositionType == Film::ETransient || m_decompositionType == Film::ETransientEllipse) && m_modulationType != Film::ENone)
		m_areaUnderCorrelationGraph = areaUnderCorrelationGraph(10000); // evaluates trapezoidal rules with n = 10000
	else
		m_areaUnderCorrelationGraph = 0;

	m_forceBounces = conf.m_forceBounces;
	m_sBounces = conf.m_sBounces;
	m_tBounces = conf.m_tBounces;

	if (conf.m_decompositionType == Film::ESteadyState || ( (conf.m_decompositionType == Film::ETransient || conf.m_decompositionType == Film::ETransientEllipse) && conf.m_modulationType != Film::ENone)) {
		m_block = new ImageBlock(Bitmap::ESpectrumAlphaWeight, blockSize, rfilter);
	} else {
		m_block = new ImageBlock(Bitmap::EMultiSpectrumAlphaWeight, blockSize,
				rfilter, (int) (SPECTRUM_SAMPLES * m_frames + 2));
	}
	m_block->setOffset(Point2i(0, 0));
	m_block->setSize(blockSize);


	if (conf.lightImage) {
		/* Stores the 'light image' -- every worker requires a
		   full-resolution version, since contributions of s==0
		   and s==1 paths can affect any pixel of this bitmap */

		if (conf.m_decompositionType == Film::ESteadyState || ((conf.m_decompositionType == Film::ETransient || conf.m_decompositionType == Film::ETransientEllipse) && conf.m_modulationType != Film::ENone)) {
			m_lightImage = new ImageBlock(Bitmap::ESpectrum,
				conf.cropSize, rfilter);
		} else {
			m_lightImage = new ImageBlock(Bitmap::EMultiSpectrumAlphaWeight,
							conf.cropSize, rfilter, (int) (SPECTRUM_SAMPLES * m_frames + 2));
		}
		m_lightImage->setSize(conf.cropSize);
		m_lightImage->setOffset(Point2i(0, 0));
	}

	/* When debug mode is active, we additionally create
	   full-resolution bitmaps storing the contributions of
	   each individual sampling strategy */
#if BDPT_DEBUG == 1
	m_debugBlocks.resize(
		conf.maxDepth*(5+conf.maxDepth)/2);

	for (size_t i=0; i<m_debugBlocks.size(); ++i) {
		m_debugBlocks[i] = new ImageBlock(
				Bitmap::ESpectrum, conf.cropSize, rfilter);
		m_debugBlocks[i]->setOffset(Point2i(0,0));
		m_debugBlocks[i]->setSize(conf.cropSize);
	}
#endif
}

BDPTWorkResult::~BDPTWorkResult() { }

void BDPTWorkResult::put(const BDPTWorkResult *workResult) {
#if BDPT_DEBUG == 1
	for (size_t i=0; i<m_debugBlocks.size(); ++i)
		m_debugBlocks[i]->put(workResult->m_debugBlocks[i].get());
#endif
	m_block->put(workResult->m_block.get());
	if (m_lightImage)
		m_lightImage->put(workResult->m_lightImage.get());
}

void BDPTWorkResult::clear() {
#if BDPT_DEBUG == 1
	for (size_t i=0; i<m_debugBlocks.size(); ++i)
		m_debugBlocks[i]->clear();
#endif
	if (m_lightImage)
		m_lightImage->clear();
	m_block->clear();
}

#if BDPT_DEBUG == 1
/* In debug mode, this function allows to dump the contributions of
   the individual sampling strategies to a series of images */
void BDPTWorkResult::dump(const BDPTConfiguration &conf,
		const fs::path &prefix, const fs::path &stem) const {
	Float weight = (Float) 1.0f / (Float) conf.sampleCount;
	for (int k = 1; k<=conf.maxDepth; ++k) {
		for (int t=0; t<=k+1; ++t) {
			size_t s = k+1-t;
			Bitmap *bitmap = const_cast<Bitmap *>(m_debugBlocks[strategyIndex(s, t)]->getBitmap());
			ref<Bitmap> ldrBitmap = bitmap->convert(Bitmap::ERGB, Bitmap::EUInt8, -1, weight);
			fs::path filename =
				prefix / fs::path(formatString("%s_k%02i_s%02i_t%02i.png", stem.filename().string().c_str(), k, s, t));
			ref<FileStream> targetFile = new FileStream(filename,
				FileStream::ETruncReadWrite);
			ldrBitmap->write(Bitmap::EPNG, targetFile, 1);
		}
	}
}
#endif

Float BDPTWorkResult::areaUnderCorrelationGraph(int n) const{
	Float h = (m_decompositionMaxBound-m_decompositionMinBound)/(n-1);
	Float value = 0.5*( fabs(correlationFunction(m_decompositionMaxBound))+ fabs(correlationFunction(m_decompositionMinBound)));
	for(int i=2; i < n; i++){
		value += fabs(correlationFunction( m_decompositionMinBound + h*(i-1) ));
	}
	value *= h;
	return value;
}

Float BDPTWorkResult::samplePathLengthTarget(ref<Sampler> sampler) const {
	int rejects = 0;
	if(m_modulationType == Film::ENone)
		return m_decompositionMinBound+(m_decompositionMaxBound-m_decompositionMinBound)*sampler->nextFloat();
	else{
		while(true){
			Float t = m_decompositionMinBound+(m_decompositionMaxBound-m_decompositionMinBound)*sampler->nextFloat();
			Float r = sampler->nextFloat();
			if(r < fabs(correlationFunction(t))){
				return t;
			}
			rejects++;
			if(rejects > 10000){
				SLog(EError, "Rejects exceed 10000.");
			}
		}
	}
}

void BDPTWorkResult::load(Stream *stream) {
#if BDPT_DEBUG == 1
	for (size_t i=0; i<m_debugBlocks.size(); ++i)
		m_debugBlocks[i]->load(stream);
#endif
	if (m_lightImage)
		m_lightImage->load(stream);
	m_block->load(stream);

	m_decompositionType = (Film::EDecompositionType) stream->readUInt();
	m_decompositionMinBound = stream->readFloat();
	m_decompositionMaxBound = stream->readFloat();
	m_decompositionBinWidth = stream->readFloat();
	m_frames = stream->readSize();
	m_subSamples = stream->readSize();

	m_modulationType = (Film::EModulationType) stream->readUInt();
	m_lambda 		 = stream->readFloat();
	m_phase 		 = stream->readFloat();
	m_P				 = stream->readInt();
	m_neighbors		 = stream->readInt();

	m_forceBounces = stream->readBool();
	m_sBounces = stream->readUInt();
	m_tBounces = stream->readUInt();
}

void BDPTWorkResult::save(Stream *stream) const {
#if BDPT_DEBUG == 1
	for (size_t i=0; i<m_debugBlocks.size(); ++i)
		m_debugBlocks[i]->save(stream);
#endif
	if (m_lightImage.get())
		m_lightImage->save(stream);
	m_block->save(stream);

	stream->writeUInt(m_decompositionType);
	stream->writeFloat(m_decompositionMinBound);
	stream->writeFloat(m_decompositionMaxBound);
	stream->writeFloat(m_decompositionBinWidth);
	stream->writeSize(m_frames);
	stream->writeSize(m_subSamples);

	stream->writeUInt(m_modulationType);
	stream->writeFloat(m_lambda);
	stream->writeFloat(m_phase);
	stream->writeInt(m_P);
	stream->writeInt(m_neighbors);

	stream->writeBool(m_forceBounces);
	stream->writeUInt(m_sBounces);
	stream->writeUInt(m_tBounces);
}

std::string BDPTWorkResult::toString() const {
	return m_block->toString();
}

MTS_IMPLEMENT_CLASS(BDPTWorkResult, false, WorkResult)
MTS_NAMESPACE_END
