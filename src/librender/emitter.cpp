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

#include <mitsuba/render/emitter.h>
#include <mitsuba/render/medium.h>
#include <mitsuba/core/track.h>
#include <mitsuba/render/shape.h>
#include <boost/algorithm/string.hpp>
#include <mitsuba/core/fresolver.h>
#include <mitsuba/core/fstream.h>
#include <mitsuba/core/mstream.h>
#include <mitsuba/core/plugin.h>
#include <mitsuba/core/timer.h>
#include <mitsuba/hw/gpuprogram.h>
#include <mitsuba/hw/gputexture.h>

MTS_NAMESPACE_BEGIN


#if SPECTRUM_SAMPLES == 3
# define ENVMAP_PIXELFORMAT Bitmap::ERGB
#else
# define ENVMAP_PIXELFORMAT Bitmap::ESpectrum
#endif

AbstractEmitter::AbstractEmitter(const Properties &props)
 : ConfigurableObject(props), m_shape(NULL), m_type(0) {
	m_worldTransform = props.getAnimatedTransform("toWorld", Transform());
}

AbstractEmitter::AbstractEmitter(Stream *stream, InstanceManager *manager)
 : ConfigurableObject(stream, manager) {
	m_worldTransform = new AnimatedTransform(stream);
	m_medium = static_cast<Medium *>(manager->getInstance(stream));
	m_shape = static_cast<Shape *>(manager->getInstance(stream));
	m_type = stream->readUInt();
 }

AbstractEmitter::~AbstractEmitter() {
}

void AbstractEmitter::serialize(Stream *stream, InstanceManager *manager) const {
	ConfigurableObject::serialize(stream, manager);
	m_worldTransform->serialize(stream);
	manager->serialize(stream, m_medium.get());
	manager->serialize(stream, m_shape);
	stream->writeUInt(m_type);
}

void AbstractEmitter::addChild(const std::string &name, ConfigurableObject *child) {
	if (child->getClass()->derivesFrom(MTS_CLASS(Medium))) {
		Assert(m_medium == NULL);
		m_medium = static_cast<Medium *>(child);
	} else {
		ConfigurableObject::addChild(name, child);
	}
}

ref<Shape> AbstractEmitter::createShape(const Scene *scene) {
	return NULL;
}

Spectrum AbstractEmitter::samplePosition(PositionSamplingRecord &pRec,
		const Point2 &sample, const Point2 *extra) const {
	NotImplementedError("samplePosition");
}

Spectrum AbstractEmitter::sampleDirection(DirectionSamplingRecord &dRec,
		PositionSamplingRecord &pRec, const Point2 &sample,
		const Point2 *extra) const {
	NotImplementedError("sampleDirection");
}

Spectrum AbstractEmitter::sampleDirect(DirectSamplingRecord &dRec, const Point2 &sample) const {
	NotImplementedError("sampleDirect");
}

Spectrum AbstractEmitter::evalPosition(const PositionSamplingRecord &pRec) const {
	NotImplementedError("evalPosition");
}

Spectrum AbstractEmitter::evalDirection(const DirectionSamplingRecord &dRec,
		const PositionSamplingRecord &pRec) const {
	NotImplementedError("evalDirection");
}

Float AbstractEmitter::pdfPosition(const PositionSamplingRecord &pRec) const {
	NotImplementedError("pdfPosition");
}

Float AbstractEmitter::pdfDirection(const DirectionSamplingRecord &dRec,
		const PositionSamplingRecord &pRec) const {
	NotImplementedError("pdfDirection");
}

Float AbstractEmitter::pdfDirect(const DirectSamplingRecord &dRec) const {
	NotImplementedError("pdfDirect");
}

Emitter::Emitter(const Properties &props)
 : AbstractEmitter(props) {
	// Importance sampling weight (used by the luminaire sampling code in \ref Scene)
	m_samplingWeight = props.getFloat("samplingWeight", 1.0f);
}

Emitter::Emitter(Stream *stream, InstanceManager *manager)
 : AbstractEmitter(stream, manager) {
	m_samplingWeight = stream->readFloat();
}

void Emitter::serialize(Stream *stream, InstanceManager *manager) const {
	AbstractEmitter::serialize(stream, manager);

	stream->writeFloat(m_samplingWeight);
}

Spectrum Emitter::sampleRay(Ray &ray,
		const Point2 &spatialSample,
		const Point2 &directionalSample,
		Float time) const {
	NotImplementedError("sampleRay");
}

Spectrum Emitter::eval(const Intersection &its, const Vector &d) const {
	NotImplementedError("eval");
}

Spectrum Emitter::evalEnvironment(const RayDifferential &ray) const {
	NotImplementedError("evalEnvironment");
}

bool Emitter::fillDirectSamplingRecord(DirectSamplingRecord &dRec,
		const Ray &ray) const {
	NotImplementedError("fillDirectSamplingRecord");
}

Emitter::~Emitter() { }

Emitter *Emitter::getElement(size_t index) {
	return NULL;
}

bool Emitter::isCompound() const {
	return false;
}

ref<Bitmap> Emitter::getBitmap(const Vector2i &sizeHint) const {
	NotImplementedError("getBitmap");
}

	ProjectiveEmitter::ProjectiveEmitter(const Properties &props) : Emitter(props) {
		/* Distance to the near clipping plane */
		m_nearClip = props.getFloat("nearClip", 1e-2f);
		/* Distance to the far clipping plane */
		m_farClip = props.getFloat("farClip", 1e4f);
		/* Distance to the focal plane */
		m_focusDistance = props.getFloat("focusDistance", m_farClip);

		if (m_nearClip <= 0)
			Log(EError, "The 'nearClip' parameter must be greater than zero!");
		if (m_nearClip >= m_farClip)
			Log(EError, "The 'nearClip' parameter must be smaller than 'farClip'.");

		m_type |= EProjectiveEmitter;


		uint64_t timestamp = 0;
		bool tryReuseCache = false;
		fs::path cacheFile;
		ref<Bitmap> bitmap;

		m_filename = Thread::getThread()->getFileResolver()->resolve(
				props.getString("filename"));

		Log(EInfo, "Loading environment map \"%s\"", m_filename.filename().string().c_str());
		if (!fs::exists(m_filename))
			Log(EError, "Environment map file \"%s\" could not be found!", m_filename.string().c_str());

		boost::system::error_code ec;
		timestamp = (uint64_t) fs::last_write_time(m_filename, ec);
		if (ec.value())
			Log(EError, "Could not determine modification time of \"%s\"!", m_filename.string().c_str());

		/* Create MIP map a cache when the environment map is large, and
           reuse cache files that have been created previously */
		cacheFile = m_filename;
		cacheFile.replace_extension(".mip");
		tryReuseCache = fs::exists(cacheFile) && props.getBoolean("cache", true);

		/* These are reasonable MIP map defaults for environment maps, I don't
       think there is a need to expose them through plugin parameters */
		EMIPFilterType filterType = EEWA;
		Float maxAnisotropy = 10.0f;

		if (tryReuseCache && MIPMap::validateCacheFile(cacheFile, timestamp,
													   ENVMAP_PIXELFORMAT, ReconstructionFilter::ERepeat,
													   ReconstructionFilter::EClamp, filterType, 0)) {
			/* Reuse an existing MIP map cache file */
			m_mipmap = new MIPMap(cacheFile, maxAnisotropy);
		} else {
			if (bitmap == NULL) {
				/* Load the input image if necessary */
				ref<Timer> timer = new Timer();
				ref<FileStream> fs = new FileStream(m_filename, FileStream::EReadOnly);
				bitmap = new Bitmap(Bitmap::EAuto, fs);
				Log(EDebug, "Loaded \"%s\" in %i ms", m_filename.filename().string().c_str(),
					timer->getMilliseconds());
			}

			/* (Re)generate the MIP map hierarchy; downsample using a
                2-lobed Lanczos reconstruction filter */
			Properties rfilterProps("lanczos");
			rfilterProps.setInteger("lobes", 2);
			ref<ReconstructionFilter> rfilter = static_cast<ReconstructionFilter *> (
					PluginManager::getInstance()->createObject(
							MTS_CLASS(ReconstructionFilter), rfilterProps));
			rfilter->configure();

			/* Potentially create a new MIP map cache file */
			bool createCache = !cacheFile.empty() && props.getBoolean("cache",
																	  bitmap->getSize().x * bitmap->getSize().y > 1024*1024);

			m_mipmap = new MIPMap(bitmap, ENVMAP_PIXELFORMAT, Bitmap::EFloat,
								  rfilter, ReconstructionFilter::ERepeat, ReconstructionFilter::EClamp,
								  filterType, maxAnisotropy, createCache ? cacheFile : fs::path(), timestamp,
								  std::numeric_limits<Float>::infinity(), Spectrum::EIlluminant);
		}
		/// Build CDF tables to sample the emitter map
		const MIPMap::Array2DType &array = m_mipmap->getArray();
		m_size = array.getSize();
        m_aspect = (Float)m_size.x / (Float) m_size.y;

        m_resolution = Vector2(m_size.x,m_size.y);
        m_invResolution = Vector2(
                (Float) 1 / m_resolution.x,
                (Float) 1 / m_resolution.y);


		size_t nEntries = (size_t) (m_size.x + 1) * (size_t) m_size.y,
				totalStorage = sizeof(float) * (m_size.x + 1 + nEntries);

		Log(EInfo, "Precomputing data structures for environment map sampling (%s)",
			memString(totalStorage).c_str());

		ref<Timer> timer = new Timer();
		m_cdfCols = new float[nEntries];
		m_cdfRows = new float[m_size.y + 1];
		m_rowWeights = new Float[m_size.y];

		size_t colPos = 0, rowPos = 0;
		Float rowSum = 0.0f;

		/* Build a marginal & conditional cumulative distribution
           function over luminances weighted uniformly */
		m_cdfRows[rowPos++] = 0;
		for (int y=0; y<m_size.y; ++y) {
			Float colSum = 0;

			m_cdfCols[colPos++] = 0;
			for (int x = 0; x < m_size.x; ++x) {
				Spectrum value(array(x, y));

				colSum += value.getLuminance();
				m_cdfCols[colPos++] = (float) colSum;
			}

			float normalization = 1.0f / std::max(Epsilon,(float) colSum);
			for (int x = 1; x < m_size.x; ++x)
				m_cdfCols[colPos - x - 1] *= normalization;

            if (colSum == 0){
                m_cdfCols[colPos - 1] = 0.0f;
            }
            else {
                m_cdfCols[colPos - 1] = 1.0f;
            }

            Float weight = 1.0f;
			m_rowWeights[y] = weight;
			rowSum += colSum * weight;
			m_cdfRows[rowPos++] = (float) rowSum;
		}

		float normalization = 1.0f / std::max(Epsilon,(Float) rowSum);

		for (int y=1; y<m_size.y; ++y){
			m_cdfRows[rowPos-y-1] *= normalization;
        }
		m_cdfRows[rowPos-1] = 1.0f;

		if (rowSum == 0)
			Log(EError, "The projective emitter map is completely black -- this is not allowed.");
		else if (!std::isfinite(rowSum))
			Log(EError, "The projective emitter contains an invalid floating"
					" point value (nan/inf) -- giving up.");

		m_normalSpectrum = 1.0f / (Float) rowSum ;

		Log(EInfo, "Done (took %i ms)", timer->getMilliseconds());



	}

	ProjectiveEmitter::ProjectiveEmitter(Stream *stream, InstanceManager *manager)
			: Emitter(stream, manager) {
		m_nearClip = stream->readFloat();
		m_farClip = stream->readFloat();
		m_focusDistance = stream->readFloat();

		size_t size = stream->readSize();
		ref<MemoryStream> mStream = new MemoryStream(size);
		stream->copyTo(mStream, size);
		mStream->seek(0);
		ref<Bitmap> bitmap = new Bitmap(Bitmap::EAuto, mStream);

		/* Downsample using a 2-lobed Lanczos reconstruction filter */
		Properties rfilterProps("lanczos");
		rfilterProps.setInteger("lobes", 2);
		ref<ReconstructionFilter> rfilter = static_cast<ReconstructionFilter *> (
				PluginManager::getInstance()->createObject(
						MTS_CLASS(ReconstructionFilter), rfilterProps));
		rfilter->configure();

		m_mipmap = new MIPMap(bitmap, ENVMAP_PIXELFORMAT, Bitmap::EFloat, rfilter,
							  ReconstructionFilter::ERepeat, ReconstructionFilter::EClamp, EEWA, 10.0f,
							  fs::path(), 0, std::numeric_limits<Float>::infinity(), Spectrum::EIlluminant);
	}

	void ProjectiveEmitter::serialize(Stream *stream, InstanceManager *manager) const {
		Emitter::serialize(stream, manager);
		stream->writeFloat(m_nearClip);
		stream->writeFloat(m_farClip);
		stream->writeFloat(m_focusDistance);

        if (!m_filename.empty() && fs::exists(m_filename)) {
            /* We still have access to the original image -- use that, since
               it is probably much smaller than the in-memory representation */
            ref<Stream> is = new FileStream(m_filename, FileStream::EReadOnly);
            stream->writeSize(is->getSize());
            is->copyTo(stream);
        } else {
            /* No access to the original image anymore. Create an EXR image
               from the top MIP map level and serialize that */
            ref<MemoryStream> mStream = new MemoryStream();
            ref<Bitmap> bitmap = m_mipmap->toBitmap();
            bitmap->write(Bitmap::EOpenEXR, mStream);

            stream->writeSize(mStream->getSize());
            stream->write(mStream->getData(), mStream->getSize());
        }
	}

	void ProjectiveEmitter::setFocusDistance(Float focusDistance) {
		if (m_focusDistance != focusDistance) {
			m_focusDistance = focusDistance;
			m_properties.setFloat("focusDistance", focusDistance, false);
		}
	}

	void ProjectiveEmitter::setNearClip(Float nearClip) {
		if (m_nearClip != nearClip) {
			m_nearClip = nearClip;
			m_properties.setFloat("nearClip", nearClip, false);
		}
	}

	void ProjectiveEmitter::setFarClip(Float farClip) {
		if (m_farClip != farClip) {
			m_farClip = farClip;
			m_properties.setFloat("farClip", farClip, false);
		}
	}

	ProjectiveEmitter::~ProjectiveEmitter() {
        if (m_mipmap)
            delete m_mipmap;
        if (m_cdfRows)
            delete[] m_cdfRows;
        if (m_cdfCols)
            delete[] m_cdfCols;
        if (m_rowWeights)
            delete[] m_rowWeights;
	}

	void ProjectiveEmitter::setWorldTransform(const Transform &trafo) {
		m_worldTransform = new AnimatedTransform(trafo);
		m_properties.setTransform("toWorld", trafo, false);
	}

	void ProjectiveEmitter::setWorldTransform(AnimatedTransform *trafo) {
		m_worldTransform = trafo;
		m_properties.setAnimatedTransform("toWorld", trafo, false);
	}

	PerspectiveEmitter::PerspectiveEmitter(const Properties &props)
			: ProjectiveEmitter(props), m_xfov(0.0f) {
		props.markQueried("fov");
		props.markQueried("fovAxis");
		props.markQueried("focalLength");

		if (m_properties.hasProperty("fov") && m_properties.hasProperty("focalLength"))
			Log(EError, "Please specify either a focal length ('focalLength') or a "
					"field of view ('fov')!");
	}

	PerspectiveEmitter::PerspectiveEmitter(Stream *stream, InstanceManager *manager)
			: ProjectiveEmitter(stream, manager), m_xfov(0.0f) {
		setXFov(stream->readFloat());
	}

	PerspectiveEmitter::~PerspectiveEmitter() {
	}

	void PerspectiveEmitter::configure() {
		if (m_xfov != 0)
			return;

		if (m_properties.hasProperty("fov")) {
			Float fov = m_properties.getFloat("fov");

			std::string fovAxis =
					boost::to_lower_copy(m_properties.getString("fovAxis", "x"));

			if (fovAxis == "smaller")
				fovAxis = m_aspect > 1 ? "y" : "x";
			else if (fovAxis == "larger")
				fovAxis = m_aspect > 1 ? "x" : "y";

			if (fovAxis == "x")
				setXFov(fov);
			else if (fovAxis == "y")
				setYFov(fov);
			else if (fovAxis == "diagonal")
				setDiagonalFov(fov);
			else
				Log(EError, "The 'fovAxis' parameter must be set "
						"to one of 'smaller', 'larger', 'diagonal', 'x', or 'y'!");
		} else {
			std::string f = m_properties.getString("focalLength", "50mm");
			if (boost::ends_with(f, "mm"))
				f = f.substr(0, f.length()-2);

			char *end_ptr = NULL;
			Float value = (Float) strtod(f.c_str(), &end_ptr);
			if (*end_ptr != '\0')
				SLog(EError, "Could not parse the focal length (must be of the form "
						"<x>mm, where <x> is a positive integer)!");

			m_properties.removeProperty("focalLength");
			setDiagonalFov(2 * 180/M_PI* std::atan(std::sqrt((Float) (36*36+24*24)) / (2*value)));
		}
	}

	void PerspectiveEmitter::serialize(Stream *stream, InstanceManager *manager) const {
		ProjectiveEmitter::serialize(stream, manager);
		stream->writeFloat(m_xfov);
	}

	void PerspectiveEmitter::setXFov(Float xfov) {
		if (xfov <= 0 || xfov >= 180)
			Log(EError, "The horizontal field of view must be "
					"in the interval (0, 180)!");
		if (xfov != m_xfov) {
			m_xfov = xfov;
			m_properties.setFloat("fov", xfov, false);
			m_properties.setString("fovAxis", "x", false);
		}
	}

	void PerspectiveEmitter::setYFov(Float yfov) {
		setXFov(radToDeg(2*std::atan(
				std::tan(0.5f * degToRad(yfov)) * m_aspect)));
	}

	void PerspectiveEmitter::setDiagonalFov(Float dfov) {
		Float diagonal = 2 * std::tan(0.5f * degToRad(dfov));
		Float width = diagonal / std::sqrt(1.0f + 1.0f / (m_aspect*m_aspect));
		setXFov(radToDeg(2*std::atan(width*0.5f)));
	}


	Float PerspectiveEmitter::getYFov() const {
		return radToDeg(2*std::atan(
				std::tan(0.5f * degToRad(m_xfov)) / m_aspect));
	}

	Float PerspectiveEmitter::getDiagonalFov() const {
		Float width = std::tan(0.5f * degToRad(m_xfov));
		Float diagonal = width * std::sqrt(1.0f + 1.0f / (m_aspect*m_aspect));
		return radToDeg(2*std::atan(diagonal));
	}

	MTS_IMPLEMENT_CLASS(PerspectiveEmitter, true, ProjectiveEmitter)
	MTS_IMPLEMENT_CLASS(ProjectiveEmitter, true, Emitter)

MTS_IMPLEMENT_CLASS(Emitter, false, AbstractEmitter)
MTS_IMPLEMENT_CLASS(AbstractEmitter, true, ConfigurableObject)
MTS_NAMESPACE_END
