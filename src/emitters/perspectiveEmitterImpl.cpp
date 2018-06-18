//
// Created by Jiatian Sun on 5/15/18.
//

#include <mitsuba/render/scene.h>
#include <iostream>
#include <mitsuba/core/warp.h>
#include <mitsuba/render/emitter.h>
#include <mitsuba/render/medium.h>
#include <mitsuba/core/track.h>
#include <mitsuba/hw/renderer.h>
#include <mitsuba/core/fresolver.h>
#include <mitsuba/core/fstream.h>
#include <mitsuba/core/mstream.h>
#include <mitsuba/core/plugin.h>
#include <mitsuba/core/timer.h>
#include <mitsuba/render/mipmap.h>
#include <mitsuba/hw/gpuprogram.h>
#include <mitsuba/hw/gputexture.h>

MTS_NAMESPACE_BEGIN

    class PerspectiveEmitterImpl : public PerspectiveEmitter{
    public:
        typedef TSpectrum<half, SPECTRUM_SAMPLES> SpectrumHalf;
        typedef TMIPMap<Spectrum, SpectrumHalf> MIPMap;
        PerspectiveEmitterImpl(const Properties &props) : PerspectiveEmitter(props) {
            m_type |=  EDeltaPosition | EPerspectiveEmitter | EOnSurface | EDirectionSampleMapsToPixels;

            m_scale = props.getFloat("scale", 1.0f);
            if (props.getTransform("toWorld", Transform()).hasScale())
                Log(EError, "Scale factors in the emitter-to-world "
                        "transformation are not allowed!");
        }

        PerspectiveEmitterImpl(Stream *stream, InstanceManager *manager)
                : PerspectiveEmitter(stream, manager) {
            m_scale = stream->readFloat();
            configure();
        }

        void serialize(Stream *stream, InstanceManager *manager) const {
            PerspectiveEmitter::serialize(stream, manager);
            stream->writeFloat(m_scale);
        }

        virtual ~PerspectiveEmitterImpl(){
            if (m_mipmap)
                delete m_mipmap;
        }

        void configure() {
            PerspectiveEmitter::configure();

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
            std::cout<<"projector";
            m_cameraToSample =
                    Transform::scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f))
                    * Transform::translate(Vector(-relOffset.x, -relOffset.y, 0.0f))
                    * Transform::scale(Vector(-0.5f, -0.5f*m_aspect, 1.0f))
                    * Transform::translate(Vector(-1.0f, -1.0f/m_aspect, 0.0f))
                    * Transform::perspective(m_xfov, m_nearClip, m_farClip);
            std::cout<<m_xfov<<" "<<m_nearClip<<" "<<m_farClip<<"\n";
            std::cout<<m_cameraToSample.toString()<<"\n";
            std::cout<<m_aspect<<" "<<m_resolution.x<<" "<<m_invResolution.x<<"\n";
            std::cout<<m_scale<<" "<<"scale\n";
            std::cout<<m_mipmap->evalBilinear(0, Point2(0,0)).toString()<<" "<<"color\n";

            m_sampleToCamera = m_cameraToSample.inverse();

            /* Precompute some data for importance(). Please
               look at that function for further details */
            Point min(m_sampleToCamera(Point(0, 0, 0))),
                    max(m_sampleToCamera(Point(1, 1, 0)));

            m_imageRect.reset();
            m_imageRect.expandBy(Point2(min.x, min.y) / min.z);
            m_imageRect.expandBy(Point2(max.x, max.y) / max.z);
            m_normalization = 1.0f / m_imageRect.getVolume();

            /* Clip-space transformation for OpenGL */
            m_clipTransform = Transform::translate(
                    Vector((1-2*relOffset.x)/relSize.x - 1,
                           -(1-2*relOffset.y)/relSize.y + 1, 0.0f)) *
                              Transform::scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f));
        }

        /**
         * \brief Compute the directional sensor response function
         * of the camera multiplied with the cosine foreshortening
         * factor associated with the image plane
         *
         * \param d
         *     A normalized direction vector from the aperture position to the
         *     reference point in question (all in local camera space)
         */
        inline Float importance(const Vector &d) const {
            /* How is this derived? Imagine a hypothetical image plane at a
               distance of d=1 away from the pinhole in camera space.

               Then the visible rectangular portion of the plane has the area

                  A = (2 * tan(0.5 * xfov in radians))^2 / aspect

               Since we allow crop regions, the actual visible area is
               potentially reduced:

                  A' = A * (cropX / filmX) * (cropY / filmY)

               Perspective transformations of such aligned rectangles produce
               an equivalent scaled (but otherwise undistorted) rectangle
               in screen space. This means that a strategy, which uniformly
               generates samples in screen space has an associated area
               density of 1/A' on this rectangle.

               To compute the solid angle density of a sampled point P on
               the rectangle, we can apply the usual measure conversion term:

                  d_omega = 1/A' * distance(P, origin)^2 / cos(theta)

               where theta is the angle that the unit direction vector from
               the origin to P makes with the rectangle. Since

                  distance(P, origin)^2 = Px^2 + Py^2 + 1

               and

                  cos(theta) = 1/sqrt(Px^2 + Py^2 + 1),

               we have

                  d_omega = 1 / (A' * cos^3(theta))
            */

            Float cosTheta = Frame::cosTheta(d);

            /* Check if the direction points behind the camera */
            if (cosTheta <= 0)
                return 0.0f;

            /* Compute the position on the plane at distance 1 */
            Float invCosTheta = 1.0f / cosTheta;
            Point2 p(d.x * invCosTheta, d.y * invCosTheta);

            /* Check if the point lies inside the chosen crop rectangle */
            if (!m_imageRect.contains(p))
                return 0.0f;

            return m_normalization * invCosTheta
                   * invCosTheta * invCosTheta;
        }

        Transform getProjectionTransform(const Point2 &apertureSample,
                                         const Point2 &aaSample) const {
            Float right = std::tan(m_xfov * M_PI/360) * m_nearClip, left = -right;
            Float top = right / m_aspect, bottom = -top;

            Vector2 offset(
                    (right-left)/10 * (aaSample.x-0.5f),
                    (top-bottom)/10* (aaSample.y-0.5f));

            return m_clipTransform *
                   Transform::glFrustum(left+offset.x, right+offset.x,
                                        bottom+offset.y, top+offset.y, m_nearClip, m_farClip);
        }
        Spectrum samplePosition(PositionSamplingRecord &pRec,
                                const Point2 &sample, const Point2 *extra) const {
//            std::cout<<pRec.p.toString()<<"\n";importance(localD) * invDist * invDist * m_scale * m_mipmap->evalBilinear(0, uv);
            const Transform &trafo = m_worldTransform->eval(pRec.time);
            pRec.p = trafo(Point(0.0f));
            pRec.n = trafo(Vector(0.0f, 0.0f, 1.0f));
            pRec.pdf = 1.0f;
            pRec.measure = EDiscrete;
//            return Spectrum(1.0f);
            return Spectrum(1.0f);
        }

        Spectrum evalPosition(const PositionSamplingRecord &pRec) const {
            return Spectrum((pRec.measure == EDiscrete) ? 1.0f : 0.0f);
        }

        Float pdfPosition(const PositionSamplingRecord &pRec) const {
            return (pRec.measure == EDiscrete) ? 1.0f : 0.0f;
        }

        //first 2 bytes -> x position
        //second 2 bytes -> y position
        Spectrum sampleDirection(DirectionSamplingRecord &dRec,
                                 PositionSamplingRecord &pRec,
                                 const Point2 &sample, const Point2 *extra) const {
            const Transform &trafo = m_worldTransform->eval(pRec.time);

            Point samplePos(sample.x, sample.y, 0.0f);

            if (extra) {
                /* The caller wants to condition on a specific pixel position */
                samplePos.x = (extra->x + sample.x) * m_invResolution.x;
                samplePos.y = (extra->y + sample.y) * m_invResolution.y;
            }

            pRec.uv = Point2(samplePos.x * m_resolution.x,
                             samplePos.y * m_resolution.y);
            Point2 uv(samplePos.x,samplePos.y);

            /* Compute the corresponding position on the
               near plane (in local camera space) */
            Point nearP = m_sampleToCamera(samplePos);

            /* Turn that into a normalized ray direction */
            Vector d = normalize(Vector(nearP));
            dRec.d = trafo(d);
            dRec.measure = ESolidAngle;
            dRec.pdf = m_normalization / (d.z * d.z * d.z);
//            return m_mipmap->evalBilinear(0,pRec.uv) * m_normalization /(m_scale * dRec.pdf);
//            std::cout<<"sampledirection "<< m_mipmap->evalBilinear(0,uv).toString()<<endl;

            return m_mipmap->evalBilinear(0,uv) * m_scale;
        }

        Float pdfDirection(const DirectionSamplingRecord &dRec,
                           const PositionSamplingRecord &pRec) const {
            if (dRec.measure != ESolidAngle)
                return 0.0f;

            const Transform &trafo = m_worldTransform->eval(pRec.time);

            return importance(trafo.inverse()(dRec.d));
        }

        Spectrum evalDirection(const DirectionSamplingRecord &dRec,
                               const PositionSamplingRecord &pRec) const {
            if (dRec.measure != ESolidAngle)
                return Spectrum(0.0f);

            const Transform &trafo = m_worldTransform->eval(pRec.time);

            Vector v = trafo.inverse()(dRec.d);
            Point2 uv(pRec.uv.x/m_resolution.x,pRec.uv.y/m_resolution.y);


            Float cosTheta = Frame::cosTheta(v);

            /* Compute the position on the plane at distance 1 */
            Float invCosTheta = 1.0f / cosTheta;
            Point2 p(v.x * invCosTheta, v.y * invCosTheta);
//            std::cout<<"evaldirection "<<dRec.d.toString()<<endl;
//            std::cout<<"evaldirection "<< m_mipmap->evalBilinear(0, pRec.uv).toString()<<endl;
            return importance(v) * m_mipmap->evalBilinear(0, uv) * m_scale;
        }

        Spectrum sampleRay(Ray &ray, const Point2 &pixelSample,
                           const Point2 &otherSample, Float timeSample) const {
            ray.setTime(timeSample);

            /* Compute the corresponding position on the
               near plane (in local camera space) */
            Point nearP = m_sampleToCamera(Point(
                    pixelSample.x * m_invResolution.x,
                    pixelSample.y * m_invResolution.y, 0.0f));

            Point2 uv(nearP.x,nearP.y);

            /* Turn that into a normalized ray direction, and
               adjust the ray interval accordingly */
            Vector d = normalize(Vector(nearP));
            Float invZ = 1.0f / d.z;
            ray.mint = m_nearClip * invZ;
            ray.maxt = m_farClip * invZ;

            const Transform &trafo = m_worldTransform->eval(ray.time);
            ray.setOrigin(trafo.transformAffine(Point(0.0f)));
            ray.setDirection(trafo(d));
            return Spectrum(m_scale * m_mipmap->evalBilinear(0,uv));
//            return Spectrum(1.0f);
//            return importance(localD) * invDist * invDist * m_scale * m_mipmap->evalBilinear(0, uv);
        }

        Spectrum sampleDirect(DirectSamplingRecord &dRec, const Point2 &sample) const {
            const Transform &trafo = m_worldTransform->eval(dRec.time);

            /* Transform the reference point into the local coordinate system */
            Point refP = trafo.inverse().transformAffine(dRec.ref);

            /* Check if it is outside of the clip range */
            if (refP.z < m_nearClip || refP.z > m_farClip) {
                dRec.pdf = 0.0f;
                return Spectrum(0.0f);
            }

            Point screenSample = m_cameraToSample(refP);
            dRec.uv = Point2(screenSample.x, screenSample.y);
            if (dRec.uv.x < 0 || dRec.uv.x > 1 ||
                dRec.uv.y < 0 || dRec.uv.y > 1) {
                dRec.pdf = 0.0f;
                return Spectrum(0.0f);
            }
            //extra point added by myself
            Point2 uv(dRec.uv.x,dRec.uv.y);
            dRec.uv.x *= m_resolution.x;
            dRec.uv.y *= m_resolution.y;

            Vector localD(refP);
            Float dist = localD.length(),
                    invDist = 1.0f / dist;
            localD *= invDist;

            dRec.p = trafo.transformAffine(Point(0.0f));
            dRec.d = (dRec.p - dRec.ref) * invDist;
            dRec.dist = dist;
            dRec.n = trafo(Vector(0.0f, 0.0f, 1.0f));
            dRec.pdf = 1;
            dRec.measure = EDiscrete;

            return importance(localD) * invDist * invDist * m_scale * m_mipmap->evalBilinear(0, uv);

        }

        Float pdfDirect(const DirectSamplingRecord &dRec) const {
            return (dRec.measure == EDiscrete) ? 1.0f : 0.0f;
        }

        AABB getAABB() const {
            return AABB();
        }

//    Shader *createShader(Renderer *renderer) const;

        std::string toString() const {
            std::ostringstream oss;
            oss << "PerspectiveEmitter[" << endl
                << "  scale = " << m_scale << "," << endl
                << "  samplingWeight = " << m_samplingWeight << "," << endl
                << "  worldTransform = " << indent(m_worldTransform.toString()) << "," << endl
                << "  medium = " << indent(m_medium.toString()) << endl
                << "]";
            return oss.str();
        }

        MTS_DECLARE_CLASS()
    private:
        Float m_scale;
        Float m_invSurfaceArea;
        Transform m_cameraToSample;
        Transform m_sampleToCamera;
        Transform m_clipTransform;
        AABB2 m_imageRect;
        Float m_normalization;

    };

    MTS_IMPLEMENT_CLASS_S(PerspectiveEmitterImpl, false, PerspectiveEmitter)
    MTS_EXPORT_PLUGIN(PerspectiveEmitterImpl, "Perspective emitter");

MTS_NAMESPACE_END