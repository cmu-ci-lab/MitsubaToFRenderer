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

#include <mitsuba/render/sensor.h>
#include <mitsuba/render/medium.h>
#include <mitsuba/core/track.h>
#include <mitsuba/core/plugin.h>
#include <boost/algorithm/string.hpp>
#include <mitsuba/core/fresolver.h>
#include <mitsuba/core/fstream.h>
#include <mitsuba/core/mstream.h>

MTS_NAMESPACE_BEGIN

#if SPECTRUM_SAMPLES == 3
# define CODED_PIXELFORMAT Bitmap::ERGB
#else
# define CODED_PIXELFORMAT Bitmap::ESpectrum
#endif

Sensor::Sensor(const Properties &props)
 : AbstractEmitter(props) {
	m_shutterOpen = props.getFloat("shutterOpen", 0.0f);
	Float shutterClose = props.getFloat("shutterClose", 0.0f);
	m_shutterOpenTime = shutterClose - m_shutterOpen;

	if (m_shutterOpenTime < 0)
		Log(EError, "Shutter opening time must be less than "
			         "or equal to the shutter closing time!");

	if (m_shutterOpenTime == 0)
		m_type |= EDeltaTime;
}

Sensor::Sensor(Stream *stream, InstanceManager *manager)
 : AbstractEmitter(stream, manager) {
	m_film = static_cast<Film *>(manager->getInstance(stream));
	m_sampler = static_cast<Sampler *>(manager->getInstance(stream));
	m_shutterOpen = stream->readFloat();
	m_shutterOpenTime = stream->readFloat();
}

Sensor::~Sensor() {
}

void Sensor::serialize(Stream *stream, InstanceManager *manager) const {
	AbstractEmitter::serialize(stream, manager);
	manager->serialize(stream, m_film.get());
	manager->serialize(stream, m_sampler.get());
	stream->writeFloat(m_shutterOpen);
	stream->writeFloat(m_shutterOpenTime);
}

void Sensor::setShutterOpenTime(Float time) {
	m_shutterOpenTime = time;
	if (m_shutterOpenTime == 0)
		m_type |= EDeltaTime;
	else
		m_type &= ~EDeltaTime;
}


Spectrum Sensor::eval(const Intersection &its, const Vector &d,
		Point2 &samplePos) const {
	Log(EError, "%s::eval(const Intersection &, const Vector &, "
		"Point2&) is not implemented!", getClass()->getName().c_str());
	return Spectrum(0.0f);
}

bool Sensor::getSamplePosition(const PositionSamplingRecord &pRec,
		const DirectionSamplingRecord &dRec, Point2 &samplePosition) const {
	Log(EError, "%s::getSamplePosition(const PositionSamplingRecord &, "
		"const DirectionSamplingRecord &, Point2&) is not implemented!",
		getClass()->getName().c_str());
	return false;
}

void Sensor::configure() {
	if (m_film == NULL) {
		/* Instantiate an EXR film by default */
		m_film = static_cast<Film*> (PluginManager::getInstance()->
			createObject(MTS_CLASS(Film), Properties("hdrfilm")));
		m_film->configure();
	}

	if (m_sampler == NULL) {
		/* No sampler has been selected - load an independent filter with 4 samples/pixel by default */
		Properties props("independent");
		props.setInteger("sampleCount", 4);
		m_sampler = static_cast<Sampler *> (PluginManager::getInstance()->
				createObject(MTS_CLASS(Sampler), props));
		m_sampler->configure();
	}

	m_aspect = m_film->getSize().x /
	   (Float) m_film->getSize().y;

	m_resolution = Vector2(m_film->getCropSize());
	m_invResolution = Vector2(
		(Float) 1 / m_resolution.x,
		(Float) 1 / m_resolution.y);
}

Spectrum Sensor::sampleRayDifferential(RayDifferential &ray,
		const Point2 &samplePosition,
		const Point2 &apertureSample,
		Float timeSample) const {
	Spectrum result = sampleRay(ray, samplePosition,
		apertureSample, timeSample);

	/* Sample a ray for X+1 */
	Ray tempRay;
	sampleRay(tempRay, samplePosition + Vector2(1, 0),
			apertureSample, timeSample);
	ray.rxOrigin = tempRay.o;
	ray.rxDirection = tempRay.d;

	/* Sample a ray for Y+1 */
	sampleRay(tempRay, samplePosition + Vector2(0, 1),
			apertureSample, timeSample);
	ray.ryOrigin = tempRay.o;
	ray.ryDirection = tempRay.d;
	ray.hasDifferentials = true;

	return result;
}

Float Sensor::pdfTime(const Ray &ray, EMeasure measure) const {
	if (ray.time < m_shutterOpen || ray.time > m_shutterOpenTime + m_shutterOpenTime)
		return 0.0f;

	if (m_shutterOpenTime == 0 && measure == EDiscrete)
		return 1.0f;
	else if (m_shutterOpenTime > 0 && measure == ELength)
		return 1.0f / m_shutterOpenTime;
	else
		return 0.0f;
}

void Sensor::addChild(const std::string &name, ConfigurableObject *child) {
	if (child->getClass()->derivesFrom(MTS_CLASS(Sampler))) {
		m_sampler = static_cast<Sampler *>(child);
	} else if (child->getClass()->derivesFrom(MTS_CLASS(Film))) {
		m_film = static_cast<Film *>(child);
	} else {
		AbstractEmitter::addChild(name, child);
	}
}

ProjectiveCamera::ProjectiveCamera(const Properties &props) : Sensor(props) {
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

	m_type |= EProjectiveCamera;
}

ProjectiveCamera::ProjectiveCamera(Stream *stream, InstanceManager *manager)
	: Sensor(stream, manager) {
	m_nearClip = stream->readFloat();
	m_farClip = stream->readFloat();
	m_focusDistance = stream->readFloat();
}

void ProjectiveCamera::serialize(Stream *stream, InstanceManager *manager) const {
	Sensor::serialize(stream, manager);
	stream->writeFloat(m_nearClip);
	stream->writeFloat(m_farClip);
	stream->writeFloat(m_focusDistance);
}

void ProjectiveCamera::setFocusDistance(Float focusDistance) {
	if (m_focusDistance != focusDistance) {
		m_focusDistance = focusDistance;
		m_properties.setFloat("focusDistance", focusDistance, false);
	}
}

void ProjectiveCamera::setNearClip(Float nearClip) {
	if (m_nearClip != nearClip) {
		m_nearClip = nearClip;
		m_properties.setFloat("nearClip", nearClip, false);
	}
}

void ProjectiveCamera::setFarClip(Float farClip) {
	if (m_farClip != farClip) {
		m_farClip = farClip;
		m_properties.setFloat("farClip", farClip, false);
	}
}

ProjectiveCamera::~ProjectiveCamera() {
}

void ProjectiveCamera::setWorldTransform(const Transform &trafo) {
	m_worldTransform = new AnimatedTransform(trafo);
	m_properties.setTransform("toWorld", trafo, false);
}

void ProjectiveCamera::setWorldTransform(AnimatedTransform *trafo) {
	m_worldTransform = trafo;
	m_properties.setAnimatedTransform("toWorld", trafo, false);
}

PerspectiveCamera::PerspectiveCamera(const Properties &props)
	: ProjectiveCamera(props), m_xfov(0.0f) {
	props.markQueried("fov");
	props.markQueried("fovAxis");
	props.markQueried("focalLength");

	if (m_properties.hasProperty("fov") && m_properties.hasProperty("focalLength"))
		Log(EError, "Please specify either a focal length ('focalLength') or a "
			"field of view ('fov')!");
}

PerspectiveCamera::PerspectiveCamera(Stream *stream, InstanceManager *manager)
	: ProjectiveCamera(stream, manager), m_xfov(0.0f) {
	setXFov(stream->readFloat());
}

PerspectiveCamera::~PerspectiveCamera() {
}

void PerspectiveCamera::configure() {
	ProjectiveCamera::configure();
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

void PerspectiveCamera::serialize(Stream *stream, InstanceManager *manager) const {
	ProjectiveCamera::serialize(stream, manager);
	stream->writeFloat(m_xfov);
}

void PerspectiveCamera::setXFov(Float xfov) {
	if (xfov <= 0 || xfov >= 180)
		Log(EError, "The horizontal field of view must be "
			"in the interval (0, 180)!");
	if (xfov != m_xfov) {
		m_xfov = xfov;
		m_properties.setFloat("fov", xfov, false);
		m_properties.setString("fovAxis", "x", false);
	}
}

void PerspectiveCamera::setYFov(Float yfov) {
	setXFov(radToDeg(2*std::atan(
		std::tan(0.5f * degToRad(yfov)) * m_aspect)));
}

void PerspectiveCamera::setDiagonalFov(Float dfov) {
	Float diagonal = 2 * std::tan(0.5f * degToRad(dfov));
	Float width = diagonal / std::sqrt(1.0f + 1.0f / (m_aspect*m_aspect));
	setXFov(radToDeg(2*std::atan(width*0.5f)));
}


Float PerspectiveCamera::getYFov() const {
	return radToDeg(2*std::atan(
		std::tan(0.5f * degToRad(m_xfov)) / m_aspect));
}

Float PerspectiveCamera::getDiagonalFov() const {
	Float width = std::tan(0.5f * degToRad(m_xfov));
	Float diagonal = width * std::sqrt(1.0f + 1.0f / (m_aspect*m_aspect));
	return radToDeg(2*std::atan(diagonal));
}

	CodedProjectiveCamera::CodedProjectiveCamera(const Properties &props) : Sensor(props) {
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

        m_type |= EProjectiveCamera;

		if (m_nearClip <= 0)
			Log(EError, "The 'nearClip' parameter must be greater than zero!");
		if (m_nearClip >= m_farClip)
			Log(EError, "The 'nearClip' parameter must be smaller than 'farClip'.");

		m_type |= EProjectiveCamera;

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
													   CODED_PIXELFORMAT, ReconstructionFilter::ERepeat,
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

			m_mipmap = new MIPMap(bitmap, CODED_PIXELFORMAT, Bitmap::EFloat,
								  rfilter, ReconstructionFilter::ERepeat, ReconstructionFilter::EClamp,
								  filterType, maxAnisotropy, createCache ? cacheFile : fs::path(), timestamp,
								  std::numeric_limits<Float>::infinity(), Spectrum::EIlluminant);
		}
		/// Build CDF tables to sample the camera map
		const MIPMap::Array2DType &array = m_mipmap->getArray();
		m_size = array.getSize();
		m_aspect = (Float)m_size.x / (Float) m_size.y;

		m_mapRes = Vector2(m_size.x,m_size.y);
		m_invMapRes = Vector2(
				(Float) 1 / m_mapRes.x,
				(Float) 1 / m_mapRes.y);


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
			Log(EError, "The coded projective camera map is completely black -- this is not allowed.");
		else if (!std::isfinite(rowSum))
			Log(EError, "The coded projective camera contains an invalid floating"
					" point value (nan/inf) -- giving up.");

		m_normalSpectrum = 1.0f / (Float) rowSum ;

		Log(EInfo, "Done (took %i ms)", timer->getMilliseconds());



	}

	CodedProjectiveCamera::CodedProjectiveCamera(Stream *stream, InstanceManager *manager)
			: Sensor(stream, manager) {
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

		m_mipmap = new MIPMap(bitmap, CODED_PIXELFORMAT, Bitmap::EFloat, rfilter,
							  ReconstructionFilter::ERepeat, ReconstructionFilter::EClamp, EEWA, 10.0f,
							  fs::path(), 0, std::numeric_limits<Float>::infinity(), Spectrum::EIlluminant);
	}

    void CodedProjectiveCamera::serialize(Stream *stream, InstanceManager *manager) const {
        Sensor::serialize(stream, manager);
        stream->writeFloat(m_nearClip);
        stream->writeFloat(m_farClip);
        stream->writeFloat(m_focusDistance);
    }

    void CodedProjectiveCamera::setFocusDistance(Float focusDistance) {
        if (m_focusDistance != focusDistance) {
            m_focusDistance = focusDistance;
            m_properties.setFloat("focusDistance", focusDistance, false);
        }
    }

    void CodedProjectiveCamera::setNearClip(Float nearClip) {
        if (m_nearClip != nearClip) {
            m_nearClip = nearClip;
            m_properties.setFloat("nearClip", nearClip, false);
        }
    }

    void CodedProjectiveCamera::setFarClip(Float farClip) {
        if (m_farClip != farClip) {
            m_farClip = farClip;
            m_properties.setFloat("farClip", farClip, false);
        }
    }

	CodedProjectiveCamera::~CodedProjectiveCamera(){
		if (m_mipmap)
			delete m_mipmap;
		if (m_cdfRows)
			delete[] m_cdfRows;
		if (m_cdfCols)
			delete[] m_cdfCols;
		if (m_rowWeights)
			delete[] m_rowWeights;
	}

	CodedPerspectiveCamera::CodedPerspectiveCamera(const Properties &props)
			: CodedProjectiveCamera(props), m_xfov(0.0f) {
		props.markQueried("fov");
		props.markQueried("fovAxis");
		props.markQueried("focalLength");

		if (m_properties.hasProperty("fov") && m_properties.hasProperty("focalLength"))
			Log(EError, "Please specify either a focal length ('focalLength') or a "
					"field of view ('fov')!");
	}

	CodedPerspectiveCamera::CodedPerspectiveCamera(Stream *stream, InstanceManager *manager)
			: CodedProjectiveCamera(stream, manager), m_xfov(0.0f) {
		setXFov(stream->readFloat());
	}

	CodedPerspectiveCamera::~CodedPerspectiveCamera() {
	}

	void CodedPerspectiveCamera::configure() {
        CodedProjectiveCamera::configure();
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

	void CodedPerspectiveCamera::serialize(Stream *stream, InstanceManager *manager) const {
		CodedProjectiveCamera::serialize(stream, manager);
		stream->writeFloat(m_xfov);
	}

	void CodedPerspectiveCamera::setXFov(Float xfov) {
		if (xfov <= 0 || xfov >= 180)
			Log(EError, "The horizontal field of view must be "
					"in the interval (0, 180)!");
		if (xfov != m_xfov) {
			m_xfov = xfov;
			m_properties.setFloat("fov", xfov, false);
			m_properties.setString("fovAxis", "x", false);
		}
	}

	void CodedPerspectiveCamera::setYFov(Float yfov) {
		setXFov(radToDeg(2*std::atan(
				std::tan(0.5f * degToRad(yfov)) * m_aspect)));
	}

	void CodedPerspectiveCamera::setDiagonalFov(Float dfov) {
		Float diagonal = 2 * std::tan(0.5f * degToRad(dfov));
		Float width = diagonal / std::sqrt(1.0f + 1.0f / (m_aspect*m_aspect));
		setXFov(radToDeg(2*std::atan(width*0.5f)));
	}


	Float CodedPerspectiveCamera::getYFov() const {
		return radToDeg(2*std::atan(
				std::tan(0.5f * degToRad(m_xfov)) / m_aspect));
	}

	Float CodedPerspectiveCamera::getDiagonalFov() const {
		Float width = std::tan(0.5f * degToRad(m_xfov));
		Float diagonal = width * std::sqrt(1.0f + 1.0f / (m_aspect*m_aspect));
		return radToDeg(2*std::atan(diagonal));
	}

MTS_IMPLEMENT_CLASS(CodedPerspectiveCamera, true, CodedProjectiveCamera)
MTS_IMPLEMENT_CLASS(PerspectiveCamera, true, ProjectiveCamera)
MTS_IMPLEMENT_CLASS(CodedProjectiveCamera, true, Sensor)
MTS_IMPLEMENT_CLASS(ProjectiveCamera, true, Sensor)
MTS_IMPLEMENT_CLASS(Sensor, true, AbstractEmitter)
MTS_NAMESPACE_END
