//
// Created by Jiatian Sun on 5/16/18.
//

#include <mitsuba/render/emitter.h>
#include <mitsuba/render/medium.h>
#include <mitsuba/render/scene.h>
#include <mitsuba/core/track.h>
#include <mitsuba/core/frame.h>
#include <mitsuba/core/warp.h>

MTS_NAMESPACE_BEGIN

/*!\plugin{orthographic}{Orthographic Emitter}
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
 * }
 * \renderings{
 * \rendering{The material test ball viewed through an orthographic camera.
 * Note the complete lack of perspective.}{sensor_orthographic}
 * \medrendering{A rendering of the Cornell box}{sensor_orthographic_2}
 * }
 *
 * This plugin implements a simple orthographic camera, i.e. a sensor
 * based on an orthographic projection without any form of perspective.
 * It can be thought of as a planar sensor that measures the radiance
 * along its normal direction. By default, this is the region $[-1, 1]^2$ inside
 * the XY-plane facing along the positive Z direction. Transformed versions
 * can be instantiated e.g. as follows:
 *
 * \begin{xml}
 * <sensor type="orthographic">
 *     <transform name="toWorld">
 *         <!-- Resize the sensor plane to 20x20 world space units -->
 *         <scale x="10" y="10"/>
 *
 *         <!-- Move and rotate it so that it contains the point
 *              (1, 1, 1) and faces direction (0, 1, 0) -->
 *         <lookat origin="1, 1, 1" target="1, 2, 1" up="0, 0, 1"/>
 *     </transform>
 * </sensor>
 * \end{xml}
 */
    class OrthographicEmitter : public ProjectiveEmitter {
    public:
        typedef TSpectrum<half, SPECTRUM_SAMPLES> SpectrumHalf;
        typedef TMIPMap<Spectrum, SpectrumHalf> MIPMap;
        OrthographicEmitter(const Properties &props)
                : ProjectiveEmitter(props) {
            m_type |= EDeltaDirection | EOrthographicEmitter | EPositionSampleMapsToPixels;
            m_intensity =props.getFloat("intensity", 1.0f);
        }

        OrthographicEmitter(Stream *stream, InstanceManager *manager)
                : ProjectiveEmitter(stream, manager) {
            m_intensity = stream->readFloat();
            configure();
        }

        void configure() {
            ProjectiveEmitter::configure();

            Vector2 relSize(1.0,1.0);
            Point2 relOffset(0.0,0.0);

            const MIPMap::Array2DType &array = m_mipmap->getArray();
            m_size = array.getSize();
            m_aspect = (Float)m_size.x / (Float) m_size.y;

            m_resolution = Vector2(m_size.x,m_size.y);
            m_invResolution = Vector2(
                    (Float) 1 / m_resolution.x,
                    (Float) 1 / m_resolution.y);

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
            ray.setTime(timeSample);
            const Transform &trafo = m_worldTransform->eval(ray.time);

            /* Compute the corresponding position on the
               near plane (in local camera space) */
            Point nearP = m_sampleToCamera.transformAffine(Point(
                    pixelSample.x * m_invResolution.x,
                    pixelSample.y * m_invResolution.y, 0.0f));

            Point2 uv(nearP.x,nearP.y);

            ray.setOrigin(trafo.transformAffine(
                    Point(nearP.x, nearP.y, 0.0f)));
            ray.setDirection(normalize(trafo(Vector(0, 0, 1))));
            ray.mint = m_nearClip;
            ray.maxt = m_farClip;

            return Spectrum(m_intensity * m_mipmap->evalBilinear(0,uv));
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
            Point2 uv(samplePos.x,samplePos.y);

            pRec.uv = Point2(samplePos.x * m_resolution.x,
                             samplePos.y * m_resolution.y);

            Point nearP = m_sampleToCamera.transformAffine(samplePos);

            nearP.z = 0.0f;
            pRec.p = trafo.transformAffine(nearP);
            pRec.n = trafo(Vector(0.0f, 0.0f, 1.0f));
            pRec.pdf = m_invSurfaceArea;
            pRec.measure = EArea;
            return Spectrum(m_intensity * m_mipmap->evalBilinear(0,uv));
        }

        Spectrum evalPosition(const PositionSamplingRecord &pRec) const {
            return  (pRec.measure == EArea) ? Spectrum(m_invSurfaceArea * m_intensity): Spectrum(0.0f);
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

            Point2 uv(sample.x,sample.y);

            dRec.p = trafo.transformAffine(Point(localP.x, localP.y, 0.0f));
            dRec.n /= scale;
            dRec.d = -dRec.n;
            dRec.dist = localP.z;
            dRec.uv = Point2(sample.x * m_resolution.x,
                             sample.y * m_resolution.y);
            dRec.pdf = 1.0f;
            dRec.measure = EDiscrete;

            return Spectrum(m_invSurfaceArea * m_intensity * m_mipmap->evalBilinear(0,uv));
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
            oss << "OrthographicEmitter[" << endl
                << "  nearClip = " << m_nearClip << "," << endl
                << "  farClip = " << m_farClip << "," << endl
                << "  worldTransform = " << indent(m_worldTransform.toString()) << "," << endl
                << "  medium = " << indent(m_medium.toString()) << "," << endl
                << "]";
            return oss.str();
        }

        MTS_DECLARE_CLASS()
    private:
        Float m_intensity;
        Transform m_cameraToSample;
        Transform m_sampleToCamera;
        Transform m_clipTransform;
        Float m_invSurfaceArea, m_scale;
    };

    MTS_IMPLEMENT_CLASS_S(OrthographicEmitter, false, ProjectiveEmitter)
    MTS_EXPORT_PLUGIN(OrthographicEmitter, "Orthographics emitter");
MTS_NAMESPACE_END

