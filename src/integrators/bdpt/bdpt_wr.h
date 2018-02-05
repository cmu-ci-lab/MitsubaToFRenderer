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

//	inline Float samplePathLengthTarget(ref<Sampler> sampler) const {
//		return m_decompositionMinBound+(m_decompositionMaxBound-m_decompositionMinBound)*sampler->nextFloat();
//	}
	Float areaUnderCorrelationGraph(int n) const;
	Float samplePathLengthTarget(ref<Sampler> sampler) const;

	Float mSeq(Float t, Float phase) const{
		t = t + phase*m_lambda*INV_PI/2;
		t = fmod(t, m_lambda);
		if(t < m_lambda/m_P){
			return 1 - t*(m_P-1)/m_lambda;
		}else if(t > (1 - 1/m_P)*m_lambda){
			return 1 - (m_lambda - t)*(m_P - 1)/m_lambda;
		}else
			return 1/m_P;
	}

	Float correlationFunction(Float t) const {
		switch(m_modulationType){
			case Film::ENone:{
				SLog(EError, "Cannot call correlation function when the modulation type is not defined");
				break;
			}
			case Film::ESine:{
				t = t + m_phase*m_lambda*INV_PI/2;
				return cos(t*2*M_PI/m_lambda);
				break;
			}
			case Film::ESquare:{
				t = t + m_phase*m_lambda*INV_PI/2;
				return 4/m_lambda*(fabs(fmod(t, m_lambda)-m_lambda/2) - m_lambda/4);
				break;
			}
			case Film::EHamiltonian:{
				t = t + m_phase*m_lambda*INV_PI/2;
				t = fmod(t, m_lambda);
				if(t < m_lambda/6){
					return 6*t/m_lambda;
				}else if(t < m_lambda/2 	&& t >= m_lambda/6){
					return 1.0;
				}else if(t < 2*m_lambda/3 	&& t >= m_lambda/2){
					return 1 - (t - m_lambda/2)*6/m_lambda;
				}else{
					return 0;
				}
				break;
			}
			case Film::EMSeq:{
				return mSeq(t, m_phase);
				break;
			}
			case Film::EDepthSelective:{
				Float value = 0;
				for(int i = 0; i < m_neighbors; i++){
					value += mSeq(t, m_phase + i*m_lambda/m_P);
				}
				value -= (m_neighbors-1)/m_P;
				return value;
				break;
			}
			default:
				SLog(EError, "Modulation type is not defined");
		}
		return 0;
	}

	Float getSamplingWeight(Float t) const{
		if(m_modulationType == Film::ENone){
			return (m_decompositionMaxBound-m_decompositionMinBound);
		}else{
			// We should compute Area/correlationFunction(t);
//			return m_areaUnderCorrelationGraph/correlationFunction(t);
			return copysignf(1.0, correlationFunction(t));
		}
		return 0;
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

	// For special case of ToF Renderer
	Film::EModulationType m_modulationType;
	Float m_lambda;
	Float m_phase;
	int   m_P;		   // For M-sequences and depth-selective camera
	int   m_neighbors; // For depth-selective camera;
	Float m_areaUnderCorrelationGraph;

	bool m_forceBounces;
	unsigned int m_sBounces;
	unsigned int m_tBounces;
};

MTS_NAMESPACE_END

#endif /* __BDPT_WR_H */
