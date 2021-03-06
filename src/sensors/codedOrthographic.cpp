#include <mitsuba/render/sensor.h>
#include <mitsuba/render/medium.h>
#include <mitsuba/core/track.h>
#include <mitsuba/core/frame.h>
#include <mitsuba/core/warp.h>

MTS_NAMESPACE_BEGIN

/*!\plugin{codedOrthographic}{Codede orthographic camera}
 * \order{3}
 * \parameters{
 *     \parameter{toWorld}{\Transform\Or\Animation}{
 *	      Specifies an optional camera-to-world transformation.
 *        \default{none (i.e. camera space $=$ world space)}
 *     }
 *     \parameter{shutterOpen, shutterClose}{\Float}{
 *         Specifies the time interval of the measurement---this
 *         is only relevant when the scene is in motion.
 *         \default{0}
 *     }
 *     \parameter{nearClip, farClip}{\Float}{
 *         Distance to the near/far clip
 *         planes.\default{\code{near\code}-\code{Clip=1e-2} (i.e.
 *         \code{0.01}) and {\code{farClip=1e4} (i.e. \code{10000})}}
 *     }
 *     \parameter{filename}{\String}{
 *       Filename of the coded camera mask image to be loaded;
 *       must be in latitude-longitude format.
 *     }
 * }
 *
 * This plugin implements a simple orthographic camera, i.e. a sensor
 * based on an orthographic projection without any form of perspective.
 * It can be thought of as a planar sensor that measures the radiance
 * along its normal direction. By default, this is the region $[-1, 1]^2$ inside
 * the XY-plane facing along the positive Z direction. The camera's mask is
 * created by scaling the input image to its film size and currently there is
 * no default value for the mask so please provide a white mask for default usage.
 * Transformed versions can be instantiated e.g. as follows:
 *
 * \begin{xml}
 * <sensor type="codedOrthographic">
 *     <transform name="toWorld">
 *         <!-- Resize the sensor plane to 20x20 world space units -->
 *         <scale x="10" y="10"/>
 *
 *         <!-- Move and rotate it so that it contains the point
 *              (1, 1, 1) and faces direction (0, 1, 0) -->
 *         <lookat origin="1, 1, 1" target="1, 2, 1" up="0, 0, 1"/>
 *     </transform>
 *     </string name="filename" value="image.png" />
 * </sensor>
 * \end{xml}
 */
class CodedOrthographicCamera : public CodedProjectiveCamera {
public:
	CodedOrthographicCamera(const Properties &props)
			: CodedProjectiveCamera(props) {
		m_type |= EDeltaDirection | EOrthographicCamera | EPositionSampleMapsToPixels | ECodedOrtho;
	}

	CodedOrthographicCamera(Stream *stream, InstanceManager *manager)
			: CodedProjectiveCamera(stream, manager) {
		configure();
	}

	void configure() {
		CodedProjectiveCamera::configure();

		const Vector2i &filmSize   = m_film->getSize();
		const Vector2i &cropSize   = m_film->getCropSize();
		const Point2i  &cropOffset = m_film->getCropOffset();

		Vector2 relSize((Float) cropSize.x / (Float) filmSize.x,
			(Float) cropSize.y / (Float) filmSize.y);
		Point2 relOffset((Float) cropOffset.x / (Float) filmSize.x,
			(Float) cropOffset.y / (Float) filmSize.y);

		/**
		 * These do the following (in reverse order):
		 *
		 * 1. Create transform from camera space to [-1,1]x[-1,1]x[0,1] clip
		 *    coordinates (not taking account of the aspect ratio yet)
		 *
		 * 2+3. Translate and scale to shift the clip coordinates into the
		 *    range from zero to one, and take the aspect ratio into account.
		 *
		 * 4+5. Translate and scale the coordinates once more to account
		 *     for a cropping window (if there is any)
		 */
		m_cameraToSample =
			  Transform::scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f))
			* Transform::translate(Vector(-relOffset.x, -relOffset.y, 0.0f))
			* Transform::scale(Vector(-0.5f, -0.5f*m_aspect, 1.0f))
			* Transform::translate(Vector(-1.0f, -1.0f/m_aspect, 0.0f))
			* Transform::orthographic(m_nearClip, m_farClip);

		m_sampleToCamera = m_cameraToSample.inverse();

		/* Position differentials on the near plane */
		m_dx = m_sampleToCamera(Point(m_invResolution.x, 0.0f, 0.0f))
			 - m_sampleToCamera(Point(0.0f));
		m_dy = m_sampleToCamera(Point(0.0f, m_invResolution.y, 0.0f))
			 - m_sampleToCamera(Point(0.0f));

		/* Clip-space transformation for OpenGL */
		m_clipTransform = Transform::translate(
			Vector((1-2*relOffset.x)/relSize.x - 1,
			      -(1-2*relOffset.y)/relSize.y + 1, 0.0f)) *
			Transform::scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f));

        std::cout<<m_nearClip<<" "<<m_farClip<<"\n";
        std::cout<<m_cameraToSample.toString()<<"\n";
        std::cout<<m_aspect<<" "<<m_resolution.x<<" "<<m_invResolution.x<<"\n";

		const Transform &trafo = m_worldTransform->eval(0.0f);

		m_invSurfaceArea = 1.0f / (
			trafo(m_sampleToCamera(Vector(1, 0, 0))).length() *
			trafo(m_sampleToCamera(Vector(0, 1, 0))).length());

		m_scale = trafo(Vector(0, 0, 1)).length();

	}

	Spectrum sampleRay(Ray &ray, const Point2 &pixelSample,
			const Point2 &otherSample, Float timeSample) const {
		ray.time = sampleTime(timeSample);
		const Transform &trafo = m_worldTransform->eval(ray.time);

		/* Compute the corresponding position on the
		   near plane (in local camera space) */
		Point nearP = m_sampleToCamera.transformAffine(Point(
			pixelSample.x * m_invResolution.x,
			pixelSample.y * m_invResolution.y, 0.0f));

		Point2 uv(pixelSample.x * m_invResolution.x * m_mapRes.x,pixelSample.y * m_invResolution.y * m_mapRes.y);

		ray.setOrigin(trafo.transformAffine(
				Point(nearP.x, nearP.y, 0.0f)));
		ray.setDirection(normalize(trafo(Vector(0, 0, 1))));
		ray.mint = m_nearClip;
		ray.maxt = m_farClip;

		return Spectrum(m_mipmap->evalTexel(0,math::floorToInt(uv.x),math::floorToInt(uv.y)));
	}

	Spectrum sampleRayDifferential(RayDifferential &ray, const Point2 &pixelSample,
			const Point2 &otherSample, Float timeSample) const {
		ray.time = sampleTime(timeSample);
		const Transform &trafo = m_worldTransform->eval(ray.time);

		/* Compute the corresponding position on the
		   near plane (in local camera space) */
		Point nearP = m_sampleToCamera.transformAffine(Point(
			pixelSample.x * m_invResolution.x,
			pixelSample.y * m_invResolution.y, 0.0f));
		nearP.z = 0.0f;

		Point2 uv(pixelSample.x * m_invResolution.x * m_mapRes.x,pixelSample.y * m_invResolution.y * m_mapRes.y);

		ray.setOrigin(trafo.transformAffine(nearP));
		ray.setDirection(normalize(trafo(Vector(0, 0, 1))));
		ray.mint = m_nearClip;
		ray.maxt = m_farClip;
		ray.rxOrigin = trafo(nearP + m_dx);
		ray.ryOrigin = trafo(nearP + m_dy);
		ray.rxDirection = ray.ryDirection = ray.d;
		ray.hasDifferentials = true;

		return Spectrum(m_mipmap->evalTexel(0,math::floorToInt(uv.x),math::floorToInt(uv.y)));
	}

	Spectrum samplePosition(PositionSamplingRecord &pRec,
			const Point2 &sample, const Point2 *extra) const {
		const Transform &trafo = m_worldTransform->eval(pRec.time);

		Point samplePos(sample.x, sample.y, 0.0f);

		if (extra) {
			/* The caller wants to condition on a specific pixel position */
			samplePos.x = (extra->x + sample.x) * m_invResolution.x;
			samplePos.y = (extra->y + sample.y) * m_invResolution.y;
		}

		Point2 uv(samplePos.x * m_mapRes.x,samplePos.y * m_mapRes.y);

		pRec.uv = Point2(samplePos.x * m_resolution.x,
			samplePos.y * m_resolution.y);

		Point nearP = m_sampleToCamera.transformAffine(samplePos);

		nearP.z = 0.0f;
		pRec.p = trafo.transformAffine(nearP);
		pRec.n = trafo(Vector(0.0f, 0.0f, 1.0f));
		pRec.pdf = m_invSurfaceArea;
		pRec.measure = EArea;

        return m_mipmap->evalTexel(0,math::floorToInt(uv.x),math::floorToInt(uv.y));

	}

	Spectrum evalPosition(const PositionSamplingRecord &pRec) const {
	    Point2 uv(pRec.uv.x * m_invResolution.x * m_mapRes.x, pRec.uv.y * m_invResolution.y * m_mapRes.y);
		return (pRec.measure == EArea) ? m_mipmap->evalTexel(0,math::floorToInt(uv.x),math::floorToInt(uv.y)) * m_invSurfaceArea : Spectrum(0.0f);
	}

	Float pdfPosition(const PositionSamplingRecord &pRec) const {
		return (pRec.measure == EArea) ? m_invSurfaceArea : 0.0f;
	}

	Spectrum sampleDirection(DirectionSamplingRecord &dRec,
			PositionSamplingRecord &pRec, const Point2 &sample,
			const Point2 *extra) const {

		dRec.d = pRec.n;
		dRec.measure = EDiscrete;
		dRec.pdf = 1.0f;

		return Spectrum(1.0f);
	}

	Float pdfDirection(const DirectionSamplingRecord &dRec,
			const PositionSamplingRecord &pRec) const {
		return (dRec.measure == EDiscrete) ? 1.0f : 0.0f;
	}

	Spectrum evalDirection(const DirectionSamplingRecord &dRec,
			const PositionSamplingRecord &pRec) const {
		return Spectrum((dRec.measure == EDiscrete) ? 1.0f : 0.0f);
	}

	Spectrum sampleDirect(DirectSamplingRecord &dRec, const Point2 &) const {
		const Transform &trafo = m_worldTransform->eval(dRec.time);

		dRec.n = trafo(Vector(0, 0, 1));
		Float scale = dRec.n.length();

		Point localP = trafo.inverse().transformAffine(dRec.ref);
		localP.z *= scale;

		Point sample = m_cameraToSample.transformAffine(localP);

		if (sample.x < 0 || sample.x > 1 || sample.y < 0 ||
			sample.y > 1 || sample.z < 0 || sample.z > 1) {
			dRec.pdf = 0.0f;
			return Spectrum(0.0f);
		}

		Point2 uv(sample.x * m_mapRes.x,sample.y * m_mapRes.y);

		dRec.p = trafo.transformAffine(Point(localP.x, localP.y, 0.0f));
		dRec.n /= scale;
		dRec.d = -dRec.n;
		dRec.dist = localP.z;
		dRec.uv = Point2(sample.x * m_resolution.x,
		                 sample.y * m_resolution.y);
		dRec.pdf = 1.0f;
		dRec.measure = EDiscrete;

		return m_invSurfaceArea * m_mipmap->evalTexel(0,math::floorToInt(uv.x),math::floorToInt(uv.y));
	}

	Float pdfDirect(const DirectSamplingRecord &dRec) const {
		return (dRec.measure == EDiscrete) ? 1.0f : 0.0f;
	}

	bool getSamplePosition(const PositionSamplingRecord &pRec,
			const DirectionSamplingRecord &dRec, Point2 &samplePosition) const {
		const Transform &trafo = m_worldTransform->eval(pRec.time);

		Point localP = trafo.inverse()(pRec.p);
		Point sample = m_cameraToSample.transformAffine(localP);

		if (sample.x < 0 || sample.x > 1 || sample.y < 0 || sample.y > 1)
			return false;

		samplePosition = Point2(sample.x * m_resolution.x,
		                        sample.y * m_resolution.y);
		return true;
	}

	Transform getProjectionTransform(const Point2 &apertureSample,
			const Point2 &aaSample) const {
		Point2 offset(
			2.0f * m_invResolution.x * (aaSample.x-0.5f),
			2.0f * m_invResolution.y * (aaSample.y-0.5f));

		return m_clipTransform *
			Transform::translate(Vector(offset.x, offset.y, 0.0f)) *
			Transform::scale(Vector(1.0f, m_aspect, 1.0f)) *
			Transform::glOrthographic(m_nearClip, m_farClip)*
			Transform::scale(Vector(1.0f, 1.0f, m_scale));
	}

	AABB getAABB() const {
		AABB bounds;
		bounds.expandBy(m_sampleToCamera(Point(0, 0, 0)));
		bounds.expandBy(m_sampleToCamera(Point(1, 1, 0)));

		return m_worldTransform->getSpatialBounds(bounds);
	}

	std::string toString() const {
		std::ostringstream oss;
		oss << "CodedOrthographicCamera[" << endl
			<< "  nearClip = " << m_nearClip << "," << endl
			<< "  farClip = " << m_farClip << "," << endl
			<< "  worldTransform = " << indent(m_worldTransform.toString()) << "," << endl
			<< "  sampler = " << indent(m_sampler->toString()) << "," << endl
			<< "  film = " << indent(m_film->toString()) << "," << endl
			<< "  medium = " << indent(m_medium.toString()) << "," << endl
			<< "  shutterOpen = " << m_shutterOpen << "," << endl
			<< "  shutterOpenTime = " << m_shutterOpenTime << endl
			<< "]";
		return oss.str();
	}

	MTS_DECLARE_CLASS()
private:
	Transform m_cameraToSample;
	Transform m_sampleToCamera;
	Transform m_clipTransform;
	Float m_invSurfaceArea, m_scale;
	Vector m_dx, m_dy;
};

MTS_IMPLEMENT_CLASS_S(CodedOrthographicCamera, false, CodedProjectiveCamera)
MTS_EXPORT_PLUGIN(CodedOrthographicCamera, "CodedOrthographics camera");
MTS_NAMESPACE_END
