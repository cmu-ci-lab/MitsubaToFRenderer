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

#if !defined(__BDPT_WR_H)
#define __BDPT_WR_H

#include <mitsuba/render/imageblock.h>
#include <mitsuba/core/fresolver.h>
#include "bdpt.h"
#include <mitsuba/render/pathlengthsampler.h>

MTS_NAMESPACE_BEGIN

/* ==================================================================== */
/*                             Work result                              */
/* ==================================================================== */

/**
   Bidirectional path tracing needs its own WorkResult implementation,
   since each rendering thread simultaneously renders to a small 'camera
   image' block and potentially a full-resolution 'light image'.
*/
class BDPTWorkResult : public WorkResult {
public:
	BDPTWorkResult(const BDPTConfiguration &conf, const ReconstructionFilter *filter,
			Vector2i blockSize = Vector2i(-1, -1));

	// Clear the contents of the work result
	void clear();

	/// Fill the work result with content acquired from a binary data stream
	virtual void load(Stream *stream);

	/// Serialize a work result to a binary data stream
	virtual void save(Stream *stream) const;

	/// Aaccumulate another work result into this one
	void put(const BDPTWorkResult *workResult);

#if BDPT_DEBUG == 1
	/* In debug mode, this function allows to dump the contributions of
	   the individual sampling strategies to a series of images */
	void dump(const BDPTConfiguration &conf,
			const fs::path &prefix, const fs::path &stem) const;

	inline void putDebugSample(int s, int t, const Point2 &sample,
			const Spectrum &spec) {
		m_debugBlocks[strategyIndex(s, t)]->put(sample, (const Float *) &spec);
	}
#endif

	/// For decomposition bitmap
	inline void putSample(const Point2 &sample, const Float *value) {
		m_block->put(sample, value);
	}

	inline void putSample(const Point2 &sample, const Spectrum &spec) {
		m_block->put(sample, spec, 1.0f);
	}

	inline void putLightSample(const Point2 &sample, const Float *value) {
		m_lightImage->put(sample, value);
	}

	inline void putLightSample(const Point2 &sample, const Spectrum &spec) {
		m_lightImage->put(sample, spec, 1.0f);
	}

	inline Float areaUnderCorrelationGraph(int n) const{
		return pathLengthSampler->areaUnderCorrelationGraph(n);
	}

	inline Float samplePathLengthTarget(ref<Sampler> sampler) const{
		return pathLengthSampler->samplePathLengthTarget(sampler);
	}

	inline Float sampleRestrictedPathLengthTarget(Float plMin, Float plMax, ref<Sampler> sampler){
		return pathLengthSampler->sampleRestrictedPathLengthTarget(plMin, plMax, sampler);
	}

	inline Float mSeq(const Float& t, const Float& phase) const{
		return pathLengthSampler->mSeq(t, phase);
	}

	inline Float correlationFunction(const Float& t) const {
		return pathLengthSampler->correlationFunction(t);
	}

	inline Float getSamplingWeight(const Float& plMin, const Float& plMax, const Float& t) const{
		return pathLengthSampler->getSamplingWeight(plMin, plMax, t);
	}

	inline PathLengthSampler::EModulationType getModulationType() const{
		return pathLengthSampler->getModulationType();
	}

	inline const ImageBlock *getImageBlock() const {
		return m_block.get();
	}

	inline const ImageBlock *getLightImage() const {
		return m_lightImage.get();
	}

	/// Return the number of channels stored by the image block
	inline int getChannelCount() const { return m_block->getChannelCount(); }


	inline void setSize(const Vector2i &size) {
		m_block->setSize(size);
	}

	inline void setOffset(const Point2i &offset) {
		m_block->setOffset(offset);
	}

	/// Return a string representation
	std::string toString() const;

	MTS_DECLARE_CLASS()
protected:
	/// Virtual destructor
	virtual ~BDPTWorkResult();

	inline int strategyIndex(int s, int t) const {
		int above = s+t-2;
		return s + above*(5+above)/2;
	}
protected:
#if BDPT_DEBUG == 1
	ref_vector<ImageBlock> m_debugBlocks;
#endif
	ref<ImageBlock> m_block, m_lightImage;
public:
	Film::EDecompositionType m_decompositionType;
	Float m_decompositionMinBound;
	Float m_decompositionMaxBound;
	Float m_decompositionBinWidth;
	size_t m_frames;
	size_t m_subSamples; // For elliptic sampling. Defaults to 1.

	ref<PathLengthSampler> pathLengthSampler;

	bool m_forceBounces;
	unsigned int m_sBounces;
	unsigned int m_tBounces;
};

MTS_NAMESPACE_END

#endif /* __BDPT_WR_H */
