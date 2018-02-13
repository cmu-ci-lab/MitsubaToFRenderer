/*
    This file is added by Adithya.
    */

#pragma once
#if !defined(__MITSUBA_RENDER_pathlengthsampler_H_)
#define __MITSUBA_RENDER_pathlengthsampler_H_

#include <mitsuba/render/sampler.h>
MTS_NAMESPACE_BEGIN


class MTS_EXPORT_RENDER PathLengthSampler{
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

private:

	Float m_decompositionMinBound;
	Float m_decompositionMaxBound;

	Float m_lambda;
	Float m_phase;
	int   m_P;		   // For M-sequences and depth-selective camera
	int   m_neighbors; // For depth-selective camera;
	Float m_areaUnderCorrelationGraph;
	EModulationType m_modulationType;

public:
	PathLengthSampler(const Float decompositionMinBound, const Float decompositionMaxBound, const std::string modulationType, const Float lambda, const Float phase, const Float P, const int neighbors):
		m_decompositionMinBound(decompositionMinBound),
		m_decompositionMaxBound(decompositionMaxBound),
		m_lambda(lambda),
		m_phase(phase),
		m_P(P),
		m_neighbors(neighbors){
			if (modulationType == "none") {
				m_modulationType = ENone;
			} else if (modulationType == "sine") {
				m_modulationType = ESine;
			} else if (modulationType == "square") {
				m_modulationType = ESquare;
			} else if (modulationType == "hamiltonian") {
				m_modulationType = EHamiltonian;
			} else if (modulationType == "mseq") {
				m_modulationType = EMSeq;
			} else if (modulationType == "depthselective") {
				m_modulationType = EDepthSelective;
			} else {
				SLog(EError, "The \"modulation\" parameter must be equal to"
					"either \"none\", \"square\", or \"hamiltonian\", or \"mseq\", or \"depthselective\"!");
			}

			m_areaUnderCorrelationGraph = areaUnderCorrelationGraph(1e6); // use a million point numerical approximation
	}

	inline EModulationType getModulationType() const{
		return m_modulationType;
	}

	inline Float areaUnderCorrelationGraph(int n) const{
		Float h = (m_decompositionMaxBound-m_decompositionMinBound)/(n-1);
		Float value = 0.5*( fabs(correlationFunction(m_decompositionMaxBound))+ fabs(correlationFunction(m_decompositionMinBound)));
		for(int i=2; i < n; i++){
			value += fabs(correlationFunction( m_decompositionMinBound + h*(i-1) ));
		}
		value *= h;
		return value;
	}

	inline Float samplePathLengthTarget(ref<Sampler> sampler) const{
		int rejects = 0;
		if(m_modulationType == ENone)
			return m_decompositionMinBound+(m_decompositionMaxBound-m_decompositionMinBound)*sampler->nextFloat();
		else{
			while(true){
				Float t = m_decompositionMinBound+(m_decompositionMaxBound-m_decompositionMinBound)*sampler->nextFloat();
				Float r = sampler->nextFloat();
				if(r < fabs(correlationFunction(t))){
					return t;
				}
				rejects++;
				if(rejects > 1e6){
					SLog(EError, "Rejects exceed 1e6.");
				}
			}
		}
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
			return (m_decompositionMaxBound-m_decompositionMinBound);
		}else{
			Float result = copysignf(1.0, correlationFunction(t))*m_areaUnderCorrelationGraph;
			return result;
		}
		return 0;
	}

	Float correlationFunction(Float t) const;
};
MTS_NAMESPACE_END

#endif
