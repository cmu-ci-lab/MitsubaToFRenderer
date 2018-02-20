/*
    This file is added by Adithya.
    */

#pragma once
#if !defined(__MITSUBA_RENDER_pathlengthsampler_H_)
#define __MITSUBA_RENDER_pathlengthsampler_H_

#include <mitsuba/render/sampler.h>
MTS_NAMESPACE_BEGIN


class MTS_EXPORT_RENDER PathLengthSampler : public ConfigurableObject {
	/**
	 * This enumeration determines all the modulation types of the time-of-flight (TransientEllipse Renderer)
	 */
public:
	enum EModulationType {
		// None is same as rendering the entire TransientEllipse
		ENone 			= 0x00,
		ESine 			= 0x01,
		ESquare			= 0x02,
		EHamiltonian	= 0x03,
		EMSeq			= 0x04,
		EDepthSelective = 0x05,
	};

	inline EModulationType getModulationType() const{
		return m_modulationType;
	}

	inline Float mSeq(Float t, Float phase) const{
		t = t + phase*m_lambda*INV_PI/2;
		t = fmod(t, m_lambda);
		if(t < m_lambda/m_P){
			return 1 - t*(m_P-1)/m_lambda;
		}else if(t > (1 - 1.0/m_P)*m_lambda){
			return 1 - (m_lambda - t)*(m_P - 1)/m_lambda;
		}else
			return 1.0/m_P;
	}

	inline Float getSamplingWeight(Float t) const{
		if(m_modulationType == ENone){
			return m_areaUnderCorrelationGraph;
		}else{
			Float result = copysignf(1.0, correlationFunction(t))*m_areaUnderCorrelationGraph;
			return result;
		}
		return 0;
	}

	Float areaUnderRestrictedCorrelationGraph(Float plMin, Float plMax, int n) const;

	Float sampleRestrictedPathLengthTarget(Float plMin, Float plMax, ref<Sampler> sampler);

	Float areaUnderCorrelationGraph(int n) const;

	Float samplePathLengthTarget(ref<Sampler> sampler) const;

	Float correlationFunction(Float t) const;

	// =============================================================
	//! @{ \name ConfigurableObject interface
	// =============================================================

	/// Add a child node
	virtual void addChild(const std::string &name, ConfigurableObject *child);

	/// Add an unnamed child
	inline void addChild(ConfigurableObject *child) { addChild("", child); }

	/// Configure the film
	virtual void configure();

	/// Serialize this film to a binary data stream
	virtual void serialize(Stream *stream, InstanceManager *manager) const;

	//! @}
	// =============================================================

	MTS_DECLARE_CLASS()
//public:

	/// Create a PathLengthSampler
	PathLengthSampler(const Properties &props);

	/// Unserialize a PathLengthSampler
	PathLengthSampler(Stream *stream, InstanceManager *manager);

	/// Virtual destructor
	virtual ~PathLengthSampler();

protected:
	Float m_decompositionMinBound;
	Float m_decompositionMaxBound;

	Float m_lambda;
	Float m_phase;
	int   m_P;		   // For M-sequences and depth-selective camera
	int   m_neighbors; // For depth-selective camera;
	Float m_areaUnderCorrelationGraph;
	EModulationType m_modulationType;
};
MTS_NAMESPACE_END

#endif
