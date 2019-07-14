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

#if !defined(__BDPT_H)
#define __BDPT_H

#include <mitsuba/mitsuba.h>
#include <mitsuba/render/film.h>
#include <mitsuba/render/pathlengthsampler.h>
/**
 * When the following is set to "1", the Bidirectional Path Tracer
 * will generate a series of debugging images that split up the final
 * rendering into the weighted contributions of the individual sampling
 * strategies.
 */
//#define BDPT_DEBUG 1

MTS_NAMESPACE_BEGIN

/* ==================================================================== */
/*                         Configuration storage                        */
/* ==================================================================== */

/**
 * \brief Stores all configuration parameters of the
 * bidirectional path tracer
 */
struct BDPTConfiguration {
	int maxDepth, blockSize, borderSize;
	bool lightImage;
	bool sampleDirect;
	bool showWeighted;
	size_t sampleCount;
	Vector2i cropSize;
	int rrDepth;
	Film::EDecompositionType m_decompositionType;
	bool m_combineBDPTAndElliptic;
	Float m_decompositionMinBound;
	Float m_decompositionMaxBound;
	Float m_decompositionBinWidth;
	bool m_isldSampling;

	// adaptive sampling
	bool m_isAdaptive;
	Float m_adapMaxError, m_adapQuantile, m_adapPValue, m_adapAverageLuminance;
	int m_adapMaxSampleFactor;

	size_t m_frames;
	size_t m_subSamples;

	ref<PathLengthSampler> pathLengthSampler;

	bool m_forceBounces;
	unsigned int m_sBounces;
	unsigned int m_tBounces;

	ref<PathLengthSampler> pathLengthSampler;

	bool m_forceBounces;
	unsigned int m_sBounces;
	unsigned int m_tBounces;

	inline BDPTConfiguration() { }

	inline BDPTConfiguration(Stream *stream) {
		maxDepth = stream->readInt();
		blockSize = stream->readInt();
		lightImage = stream->readBool();
		sampleDirect = stream->readBool();
		showWeighted = stream->readBool();
		sampleCount = stream->readSize();
		cropSize = Vector2i(stream);
		rrDepth = stream->readInt();
		m_decompositionType = (Film::EDecompositionType) stream->readUInt();
		m_combineBDPTAndElliptic  = stream->readBool();
		m_decompositionMinBound   = stream->readFloat();
		m_decompositionMaxBound   = stream->readFloat();
		m_decompositionBinWidth   = stream->readFloat();
		if (maxDepth!=-1 && m_decompositionType == Film::EBounce){
			if (maxDepth > m_decompositionMaxBound)
				maxDepth = m_decompositionMaxBound;
			if (maxDepth < m_decompositionMinBound)
				SLog(EError, "maxDepth of BDPT is less than the minimum bound; Rendering is futile");
		}
		m_isldSampling = stream->readBool();

		m_isAdaptive   			= stream->readBool();
		m_adapMaxError 			= stream->readFloat();
		m_adapQuantile 			= stream->readFloat();
		m_adapPValue 			= stream->readFloat();
		m_adapAverageLuminance 	= stream->readFloat();
		m_adapMaxSampleFactor	= stream->readInt();

		m_frames = stream->readSize();
		m_subSamples = stream->readSize();
		m_forceBounces = stream->readBool();
		m_sBounces = stream->readUInt();
		m_tBounces = stream->readUInt();
	}

	inline void serialize(Stream *stream) const {
		stream->writeInt(maxDepth);
		stream->writeInt(blockSize);
		stream->writeBool(lightImage);
		stream->writeBool(sampleDirect);
		stream->writeBool(showWeighted);
		stream->writeSize(sampleCount);
		cropSize.serialize(stream);
		stream->writeInt(rrDepth);
		stream->writeUInt(m_decompositionType);
		stream->writeBool(m_combineBDPTAndElliptic);
		stream->writeFloat(m_decompositionMinBound);
		stream->writeFloat(m_decompositionMaxBound);
		stream->writeFloat(m_decompositionBinWidth);
		stream->writeBool(m_isldSampling);

        stream->writeBool(m_isAdaptive);
		stream->writeFloat(m_adapMaxError);
		stream->writeFloat(m_adapQuantile);
		stream->writeFloat(m_adapPValue);
		stream->writeFloat(m_adapAverageLuminance);
		stream->writeInt(m_adapMaxSampleFactor);

        stream->writeSize(m_frames);
		stream->writeSize(m_subSamples);

		stream->writeBool(m_forceBounces);
		stream->writeUInt(m_sBounces);
		stream->writeUInt(m_tBounces);
	}

	void dump() const {

		std::string decompositionType;
		if (m_decompositionType == Film::ESteadyState) {
			decompositionType = "none";
		} else if (m_decompositionType == Film::ETransient) {
			decompositionType = "transient";
		} else if (m_decompositionType == Film::EBounce) {
			decompositionType = "bounce";
		} else if (m_decompositionType == Film::ETransientEllipse) {
			decompositionType = "transientellipse";
		}

		SLog(EDebug, "Bidirectional path tracer configuration:");
		SLog(EDebug, "   Maximum path depth          : %i", maxDepth);
		SLog(EDebug, "   Image size                  : %ix%i",
			cropSize.x, cropSize.y);
		SLog(EDebug, "   Direct sampling strategies  : %s",
			sampleDirect ? "yes" : "no");
		SLog(EDebug, "   Generate light image        : %s",
			lightImage ? "yes" : "no");
		SLog(EDebug, "   Russian roulette depth      : %i", rrDepth);
		SLog(EDebug, "   Block size                  : %i", blockSize);
		SLog(EDebug, "   Number of samples           : " SIZE_T_FMT, sampleCount);
		SLog(EDebug, "   decomposition type 		 : %s", decompositionType.c_str());
		SLog(EDebug, "   Combine BDPT and Elliptic?  : %s",
				m_combineBDPTAndElliptic ? "yes" : "no");
		SLog(EDebug, "   decomposition min bound	 : %f", m_decompositionMinBound);
		SLog(EDebug, "   decomposition max bound	 : %f", m_decompositionMaxBound);
		SLog(EDebug, "   decomposition bin width 	 : %f", m_decompositionBinWidth);
		SLog(EDebug, "   is ldSampling Enabled	   	 : %s",
				m_isldSampling ? "yes" : "no");
		SLog(EDebug, "   is adaptiveSampling Enabled : %s",
						m_isAdaptive ? "yes" : "no");
		SLog(EDebug, "   adapMaxError		    	 : %f", m_adapMaxError);
		SLog(EDebug, "   m_adapQuantile		    	 : %f", m_adapQuantile);
		SLog(EDebug, "   m_adapPValue		    	 : %f", m_adapPValue);
		SLog(EDebug, "   m_adapAverageLuminance		 : %f", m_adapAverageLuminance);
		SLog(EDebug, "   m_adapMaxSampleFactor		 : %i", m_adapMaxSampleFactor);

		SLog(EDebug, "   number of frames	   	     : %i", m_frames);
		SLog(EDebug, "   number of subsamples		 : %i", m_subSamples);
		SLog(EDebug, "   Force Bounces		 	 	 : %i", m_forceBounces);
		SLog(EDebug, "   S Bounce number		 	 : %i", m_sBounces);
		SLog(EDebug, "   T Bounce number		 	 : %i", m_tBounces);

		#if BDPT_DEBUG == 1
			SLog(EDebug, "   Show weighted contributions : %s", showWeighted ? "yes" : "no");
		#endif
	}
};

MTS_NAMESPACE_END

#endif /* __BDPT_H */
