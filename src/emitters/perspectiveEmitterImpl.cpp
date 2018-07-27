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
#include <math.h>

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
            std::cout<<"normalspectrum: "<<m_normalSpectrum<<endl;

            m_sampleToCamera = m_cameraToSample.inverse();

            /* Precompute some data for importance(). Please
               look at that function for further details */
            Point min(m_sampleToCamera(Point(0, 0, 0))),
                    max(m_sampleToCamera(Point(1, 1, 0)));

            m_imageRect.reset();
            m_imageRect.expandBy(Point2(min.x, min.y) / min.z);
            m_imageRect.expandBy(Point2(max.x, max.y) / max.z);
            m_normalization = 1.0f / m_imageRect.getVolume();
            std::cout<<m_imageRect.toString()<<endl;
            /* Clip-space transformation for OpenGL */
            m_clipTransform = Transform::translate(
                    Vector((1-2*relOffset.x)/relSize.x - 1,
                           -(1-2*relOffset.y)/relSize.y + 1, 0.0f)) *
                              Transform::scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f));
        }
        //16 * 16 sample to 512

        /// Helper function that samples a direction from the environment map
        Point2 internalSampleDirection(Point2 sample, Spectrum &value, Float &pdf) const {
            /* Sample a discrete pixel position */
            uint32_t row = sampleReuse(m_cdfRows, m_size.y, sample.y),
                    col = sampleReuse(m_cdfCols + row * (m_size.x+1), m_size.x, sample.x);


            /* Using the remaining bits of precision to shift the sample by an offset
               drawn from a tent function. This effectively creates a sampling strategy
               for a linearly interpolated environment map */
            Point2 pos = Point2((Float) col, (Float) row) + sample;
            /* Bilinearly interpolate colors from the adjacent four neighbors */
            int xPos = math::floorToInt(pos.x), yPos = math::floorToInt(pos.y);

//            Float dx1 = pos.x - xPos, dx2 = 1.0f - dx1,
//                    dy1 = pos.y - yPos, dy2 = 1.0f - dy1;

//            Spectrum v1,v2,v3,v4;
//            v1 = m_mipmap->evalTexel(0, xPos, yPos) * dx2 * dy2;
//            v2 = m_mipmap->evalTexel(0, xPos, yPos + 1) * dx2 * dy1;
//            v3 = m_mipmap->evalTexel(0, xPos + 1, yPos) * dx1 * dy2;
//            v4 = m_mipmap->evalTexel(0, xPos + 1, yPos + 1) * dx1 * dy1;

//            Spectrum value1 = v1 + v3;
//            Spectrum value2 = v2 + v4;
//            stats::filteredLookups.incrementBase();

            /* Compute the final color and probability density of the sample */
//            value = (value1 + value2) * m_scale;
//            value = (value1 + value2);
            value = m_mipmap->evalTexel(0, xPos, yPos);
            pdf = value.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)]  * m_normalSpectrum;

//            pdf = (value1.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)] +
//                   value2.getLuminance() * m_rowWeights[math::clamp(yPos+1, 0, m_size.y-1)]) * m_normalSpectrum;
//            pos.x = (pos.x + 0.5f)/m_size.x; pos.y = (pos.y + 0.5f)/m_size.y;

            pos.x = pos.x/m_size.x; pos.y = pos.y/m_size.y;
            return pos;
        }

        Spectrum internalEvalDirection(const Vector &d) const{
            Float cosTheta = Frame::cosTheta(d);

            /* Compute the position on the plane at distance 1 */
            Float invCosTheta = 1.0f / cosTheta;
            Point2 uv(d.x * invCosTheta, d.y * invCosTheta);

            /* Check if the point lies inside the chosen crop rectangle */
            if (!m_imageRect.contains(uv)) {
                return Spectrum(0.0f);
            }

            Point samplePoint(uv.x,uv.y,1.0f);
            Point sample = m_cameraToSample(samplePoint * 1.0f);

            /* Convert to fractional pixel coordinates on the specified level */
//            Float u = sample.x * m_size.x - 0.5f, v = sample.y * m_size.y - 0.5f;
            Float u = sample.x * m_size.x, v = sample.y * m_size.y;

            /* Bilinearly interpolate colors from the adjacent four neighbors */
            int xPos = math::floorToInt(u), yPos = math::floorToInt(v);
//            Float dx1 = u - xPos, dx2 = 1.0f - dx1,
//                    dy1 = v - yPos, dy2 = 1.0f - dy1;

//            Spectrum v1,v2,v3,v4;
//            v1 = m_mipmap->evalTexel(0, xPos, yPos) * dx2 * dy2;
//            v2 = m_mipmap->evalTexel(0, xPos, yPos + 1) * dx2 * dy1;
//            v3 = m_mipmap->evalTexel(0, xPos + 1, yPos) * dx1 * dy2;
//            v4 = m_mipmap->evalTexel(0, xPos + 1, yPos + 1) * dx1 * dy1;
//
//            Spectrum value1 = v1 + v3;
//            Spectrum value2 = v2 + v4;
//            stats::filteredLookups.incrementBase();
//            Spectrum value = (value1 + value2);
              Spectrum value = m_mipmap->evalTexel(0, xPos, yPos);
//            std::cout<<m_normalSpectrum<<endl;
            return  value.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)] * m_normalSpectrum
                    * m_normalization * invCosTheta * invCosTheta * invCosTheta * value;
//            return m_normalization * invCosTheta * invCosTheta * invCosTheta * value;

//            return  (value1.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)] +
//                   value2.getLuminance() * m_rowWeights[math::clamp(yPos+1, 0, m_size.y-1)]) * m_normalSpectrum
//                    * m_normalization * invCosTheta * invCosTheta * invCosTheta * value;
//            return m_normalization * invCosTheta * invCosTheta * invCosTheta * value;

        }

        /// Helper function that computes the solid angle density of \ref internalSampleDirection()
        Float internalPdfDirection(const Vector &d) const {
            Float cosTheta = Frame::cosTheta(d);

            /* Check if the direction points behind the camera */
            if (cosTheta <= 0)
                return 0.0f;

            /* Compute the position on the plane at distance 1 */
            Float invCosTheta = 1.0f / cosTheta;
            Point2 uv(d.x * invCosTheta, d.y * invCosTheta);


            /* Check if the point lies inside the chosen crop rectangle */
            if (!m_imageRect.contains(uv)) {
                return 0.0f;
            }

            Point samplePoint(uv.x,uv.y,1.0f);
            Point sample = m_cameraToSample(samplePoint * m_nearClip);
            /* Convert to fractional pixel coordinates on the specified level */
//            Float u = sample.x * m_size.x - 0.5f, v = sample.y * m_size.y - 0.5f;
            Float u = sample.x * m_size.x, v = sample.y * m_size.y;
//            std::cout<<uv.toString()<<endl;

            /* Bilinearly interpolate colors from the adjacent four neighbors */
            int xPos = math::floorToInt(u), yPos = math::floorToInt(v);
            Float dx1 = u - xPos, dx2 = 1.0f - dx1,
                    dy1 = v - yPos, dy2 = 1.0f - dy1;

//            Spectrum v1,v2,v3,v4;
//            v1 = m_mipmap->evalTexel(0, xPos, yPos) * dx2 * dy2;
//            v2 = m_mipmap->evalTexel(0, xPos, yPos + 1) * dx2 * dy1;
//            v3 = m_mipmap->evalTexel(0, xPos + 1, yPos) * dx1 * dy2;
//            v4 = m_mipmap->evalTexel(0, xPos + 1, yPos + 1) * dx1 * dy1;
//
//            Spectrum value1 = v1 + v3;
//            Spectrum value2 = v2 + v4;
//            stats::filteredLookups.incrementBase();
            Spectrum value = m_mipmap->evalTexel(0,xPos,yPos);

            return value.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)]
                   * m_normalSpectrum * m_normalization * invCosTheta * invCosTheta * invCosTheta;
//            return (value1.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)] +
//                    value2.getLuminance() * m_rowWeights[math::clamp(yPos+1, 0, m_size.y-1)])
//                   * m_normalSpectrum * m_normalization * invCosTheta * invCosTheta * invCosTheta;
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
            const Transform &trafo = m_worldTransform->eval(pRec.time);
            pRec.p = trafo(Point(0.0f));
            pRec.n = trafo(Vector(0.0f, 0.0f, 1.0f));
            pRec.pdf = 1.0f;
            pRec.measure = EDiscrete;

            return Spectrum(1.0f * m_scale /(m_normalSpectrum * m_resolution.x * m_resolution.y));
        }

        Spectrum evalPosition(const PositionSamplingRecord &pRec) const {
            return Spectrum((pRec.measure == EDiscrete) ? 1.0f* m_scale /(m_normalSpectrum * m_resolution.x * m_resolution.y) : 0.0f);
        }

        Float pdfPosition(const PositionSamplingRecord &pRec) const {
            return (pRec.measure == EDiscrete) ? 1.0f  : 0.0f;
        }

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

            /* Sample a direction from the texture map */
            Spectrum value; Float pdf;
            Point2 pos = internalSampleDirection(Point2(samplePos.x,samplePos.y), value, pdf);

            samplePos.x = pos.x;
            samplePos.y = pos.y;
            Point2 uv(samplePos.x, samplePos.y);

            pRec.uv = Point2(samplePos.x * m_resolution.x, samplePos.y * m_resolution.y);

            /* Compute the corresponding position on the
               near plane (in local camera space) */
            Point nearP = m_sampleToCamera(samplePos);

            /* Turn that into a normalized ray direction */
            Vector d = normalize(Vector(nearP));

            dRec.d = trafo(d);
            dRec.measure = ESolidAngle;
            dRec.pdf = m_normalization / (d.z * d.z * d.z) * pdf;

            if (value.isZero() || pdf == 0)
                return Spectrum(0.0f);
            else{
                return value;

            }
        }

        Float pdfDirection(const DirectionSamplingRecord &dRec,
                           const PositionSamplingRecord &pRec) const {
            if (dRec.measure != ESolidAngle)
                return 0.0f;

            const Transform &trafo = m_worldTransform->eval(pRec.time);

            return internalPdfDirection(trafo.inverse()(dRec.d));
        }

        Spectrum evalDirection(const DirectionSamplingRecord &dRec,
                               const PositionSamplingRecord &pRec) const {
            if (dRec.measure != ESolidAngle)
                return Spectrum(0.0f);

            const Transform &trafo = m_worldTransform->eval(pRec.time);

            Vector v = trafo.inverse()(dRec.d);

            return internalEvalDirection(v);
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
            std::string str = m_mipmap->evalTexel(0,math::floorToInt(uv.x),math::floorToInt(uv.y)).toString() + " " + uv.toString();
            std::cout<<str<<endl;
            return Spectrum(m_scale * m_mipmap->evalTexel(0,math::floorToInt(uv.x * m_resolution.x),
                                                          math::floorToInt(uv.y * m_resolution.y)));
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
            Point2 uv(dRec.uv.x, dRec.uv.y);
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

            return importance(localD) * invDist * invDist * m_scale *
                    m_mipmap->evalTexel(0,math::floorToInt(dRec.uv.x),math::floorToInt(dRec.uv.y));
//            return importance(localD) * invDist * invDist * m_scale * evalBilinear(m_mipmap,uv);

        }

        Float pdfDirect(const DirectSamplingRecord &dRec) const {
            return (dRec.measure == EDiscrete) ? 1.0f : 0.0f;
        }

        AABB getAABB() const {
            return AABB();
        }


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
        /// Sample from an array using the inversion method
        inline uint32_t sampleReuse(float *cdf, uint32_t size, Float &sample) const {
            float *entry = std::lower_bound(cdf, cdf+size+1, (float) sample);
            uint32_t index = std::min((uint32_t) std::max((ptrdiff_t) 0, entry - cdf - 1), size-1);
            sample = (sample - (Float) cdf[index]) / (Float) (cdf[index+1] - cdf[index]);
            return index;
        }

        inline Spectrum evalBilinear(MIPMap *mipmap, const Point2 &uv) const {
            if (EXPECT_NOT_TAKEN(!std::isfinite(uv.x) || !std::isfinite(uv.y))) {
                Log(EWarn, "evalBilinear(): encountered a NaN!");
                return Spectrum(0.0f);
            }

            /* Convert to fractional pixel coordinates on the specified level */
//            Float u = uv.x * m_size.x - 0.5f, v = uv.y * m_size.y - 0.5f;
            Float u = uv.x * m_size.x, v = uv.y * m_size.y;

            int xPos = math::floorToInt(u), yPos = math::floorToInt(v);
            Float dx1 = u - xPos, dx2 = 1.0f - dx1,
                    dy1 = v - yPos, dy2 = 1.0f - dy1;

            Spectrum v1,v2,v3,v4;
            v1 = mipmap->evalTexel(0, xPos, yPos) * dx2 * dy2;
            v2 = mipmap->evalTexel(0, xPos, yPos + 1) * dx2 * dy1;
            v3 = mipmap->evalTexel(0, xPos + 1, yPos) * dx1 * dy2;
            v4 =  mipmap->evalTexel(0, xPos + 1, yPos + 1) * dx1 * dy1;
            if(yPos<0 || yPos>=m_size.y){
                v1 = Spectrum(0.0f);
                v3 = Spectrum(0.0f);
            }
            if(yPos + 1<0 || yPos + 1>=m_size.y){
                v2 = Spectrum(0.0f);
                v4 = Spectrum(0.0f);
            }
            if(xPos<0 || xPos>=m_size.x){
                v1 = Spectrum(0.0f);
                v2 = Spectrum(0.0f);
            }
            if(xPos + 1<0 || xPos + 1>=m_size.x){
                v3 = Spectrum(0.0f);
                v4 = Spectrum(0.0f);
            }
            return v1 + v2 + v3 + v4;
        }
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