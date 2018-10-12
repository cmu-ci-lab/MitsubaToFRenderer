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

/*!\plugin{orthographicProjector}{Orthographic Projector}
 * \order{3}
 * \parameters{
 *     \parameter{toWorld}{\Transform\Or\Animation}{
 *	      Specifies an optional camera-to-world transformation.
 *        \default{none (i.e. camera space $=$ world space)}
 *     }
 *     \parameter{nearClip, farClip}{\Float}{
 *         Distance to the near/far clip
 *         planes.\default{\code{near\code}-\code{Clip=1e-2} (i.e.
 *         \code{0.01}) and {\code{farClip=1e4} (i.e. \code{10000})}}
 *     }
 *     \parameter{irradiance}{\Float}{
 *         Specifies a scale to amount of power per unit area received
 *         by a hypothetical surface normal to the specified direction
 *         \default{1}
 *     }
 *     \parameter{filename}{\String}{
 *         Filename of the irradiance-valued input image to be loaded;
 *         must be in latitude-longitude format(the image to be
 *         projected into the scene).
 *     }
 * }
 *
 * This plugin implements a orthographic emitter, i.e. an emitter
 * based on an orthographic projection without any form of perspective.
 * It can be thought of as a planar emitter that projects light
 * along its normal direction. By default, this is the region $[-1, 1]^2$ inside
 * the XY-plane facing along the positive Z direction. The emitter requires an
 * input image whose resolution and color specifies irradiance value of the plane
 * and currently there is no default value for the projection image so please
 * create a white image for default usage. If you need to adjust the threshold of
 * irrdiance of the plane, you can specify it by \code{irradiance}, which is by
 * default 1. Transformed versions can be instantiated.
 * e.g. as follows:
 *
 * \begin{xml}
 * <sensor type="orthographicProjector">
 *     <transform name="toWorld">
 *         <!-- Resize the sensor plane to 20x20 world space units -->
 *         <scale x="10" y="10"/>
 *
 *         <!-- Move and rotate it so that it contains the point
 *              (1, 1, 1) and faces direction (0, 1, 0) -->
 *         <lookat origin="1, 1, 1" target="1, 2, 1" up="0, 0, 1"/>
 *     </transform>
 *     </string name="filename" value="image.png" />
 *     <float name="irradiance" value="10"/>
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
            m_irradiance =props.getFloat("irradiance", 1.0f);
        }

        OrthographicEmitter(Stream *stream, InstanceManager *manager)
                : ProjectiveEmitter(stream, manager) {
            m_irradiance = stream->readFloat();
            configure();
        }

        void configure() {
            ProjectiveEmitter::configure();

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
             * 1. Create transform from emitter space to [-1,1]x[-1,1]x[0,1] clip
             *    coordinates (not taking account of the aspect ratio yet)
             *
             * 2+3. Translate and scale to shift the clip coordinates into the
             *    range from zero to one, and take the aspect ratio into account.
             *
             */
            m_emitterToSample =
                    Transform::scale(Vector(-0.5f, -0.5f*m_aspect, 1.0f))
                    * Transform::translate(Vector(-1.0f, -1.0f/m_aspect, 0.0f))
                    * Transform::orthographic(m_nearClip, m_farClip);

            m_sampleToEmitter = m_emitterToSample.inverse();

            const Transform &trafo = m_worldTransform->eval(0.0f);

            m_invSurfaceArea = 1.0f / (
                    trafo(m_sampleToEmitter(Vector(1, 0, 0))).length() *
                    trafo(m_sampleToEmitter(Vector(0, 1, 0))).length());

            m_scale = trafo(Vector(0, 0, 1)).length();

            /* calculate power of the emitter when using uniformly random sampling */
            m_power = m_irradiance / (m_invSurfaceArea * m_normalSpectrum);

            /* calcluate power of the emitter when using importance sampling */
            m_im_power = m_irradiance / (m_invSurfaceArea * m_normalSpectrum * m_resolution.x * m_resolution.y);

        }

        /// Helper function that samples a direction from the environment map
        Point2 internalSamplePosition(Point2 sample, Spectrum &value, Float &pdf) const {
            /* Sample a discrete pixel position */
            uint32_t row = sampleReuse(m_cdfRows, m_size.y, sample.y),
                    col = sampleReuse(m_cdfCols + row * (m_size.x+1), m_size.x, sample.x);


            /* Using the remaining bits of precision to get the exact position of the sample */
            Point2 pos = Point2((Float) col, (Float) row) + sample;

            /* Take color from the pixel that sample resides */
            int xPos = math::floorToInt(pos.x), yPos = math::floorToInt(pos.y);

            value = m_mipmap->evalTexel(0, xPos, yPos);
            pdf = value.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)]  * m_normalSpectrum;

            pos.x = pos.x/m_size.x; pos.y = pos.y/m_size.y;
            return pos;
        }

        Spectrum internalEvalPosition(const Point2 &uv) const{

            /* Take color from the pixel that sample resides */
            int xPos = math::floorToInt(uv.x), yPos = math::floorToInt(uv.y);

            Spectrum value = m_mipmap->evalTexel(0, xPos, yPos);

            return  Spectrum(value.getLuminance()
                             * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)] * m_normalSpectrum
                             * m_im_power);

        }

        /// Helper function that computes the
        Float internalPdfPosition(const Point2 &uv) const {

            Point samplePoint(uv.x,uv.y,1.0f);
            Point sample = m_emitterToSample(samplePoint * m_nearClip);
            Float u = sample.x * m_size.x, v = sample.y * m_size.y;

            /* Bilinearly interpolate colors from the adjacent four neighbors */
            int xPos = math::floorToInt(u), yPos = math::floorToInt(v);

            Spectrum value = m_mipmap->evalTexel(0,xPos,yPos);

            if(value.isZero())
                return Epsilon;
            else
                return value.getLuminance() * m_rowWeights[math::clamp(yPos,   0, m_size.y-1)]
                   *  m_normalSpectrum;
        }

        Spectrum sampleRay(Ray &ray, const Point2 &pixelSample,
                           const Point2 &otherSample, Float timeSample) const {
            ray.setTime(timeSample);
            const Transform &trafo = m_worldTransform->eval(ray.time);

            /* Compute the corresponding position on the
               near plane (in local camera space) */
            Point nearP = m_sampleToEmitter.transformAffine(Point(
                    pixelSample.x * m_invResolution.x,
                    pixelSample.y * m_invResolution.y, 0.0f));

            ray.setOrigin(trafo.transformAffine(
                    Point(nearP.x, nearP.y, 0.0f)));
            ray.setDirection(normalize(trafo(Vector(0, 0, 1))));
            ray.mint = m_nearClip;
            ray.maxt = m_farClip;

            return Spectrum(m_power * m_mipmap->evalTexel(0,math::floorToInt(pixelSample.x),math::floorToInt(pixelSample.y)));
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

            /* Sample a direction from the texture map */
            Spectrum value; Float pdf;
            Point2 pos = internalSamplePosition(Point2(samplePos.x,samplePos.y), value, pdf);
            samplePos.x = pos.x;
            samplePos.y = pos.y;

            pRec.uv = Point2(samplePos.x * m_resolution.x,
                             samplePos.y * m_resolution.y);

            Point nearP = m_sampleToEmitter.transformAffine(samplePos);

            nearP.z = 0.0f;
            pRec.p = trafo.transformAffine(nearP);
            pRec.n = trafo(Vector(0.0f, 0.0f, 1.0f));
            pRec.pdf = pdf;
            pRec.measure = EArea;

            return Spectrum(m_im_power);
        }

        Spectrum evalPosition(const PositionSamplingRecord &pRec) const {
            return  (pRec.measure == EArea) ? internalEvalPosition(pRec.uv):
                    Spectrum(0.0f);
        }

        Float pdfPosition(const PositionSamplingRecord &pRec) const {
            return (pRec.measure == EArea) ? internalPdfPosition(pRec.uv) : 0.0f;
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

            Point sample = m_emitterToSample.transformAffine(localP);

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

            return Spectrum(m_irradiance* m_mipmap->evalTexel(0,math::floorToInt(dRec.uv.x),math::floorToInt(dRec.uv.y)));
        }

        Float pdfDirect(const DirectSamplingRecord &dRec) const {
            return (dRec.measure == EDiscrete) ? 1.0f : 0.0f;
        }

        Transform getProjectionTransform(const Point2 &apertureSample,
                                         const Point2 &aaSample) const {
            Point2 offset(
                    2.0f * m_invResolution.x * (aaSample.x-0.5f),
                    2.0f * m_invResolution.y * (aaSample.y-0.5f));

            return Transform::translate(Vector(offset.x, offset.y, 0.0f)) *
                   Transform::scale(Vector(1.0f, m_aspect, 1.0f)) *
                   Transform::glOrthographic(m_nearClip, m_farClip)*
                   Transform::scale(Vector(1.0f, 1.0f, m_scale));
        }

        AABB getAABB() const {
            AABB bounds;
            bounds.expandBy(m_sampleToEmitter(Point(0, 0, 0)));
            bounds.expandBy(m_sampleToEmitter(Point(1, 1, 0)));

            return m_worldTransform->getSpatialBounds(bounds);
        }

        std::string toString() const {
            std::ostringstream oss;
            oss << "OrthographicEmitter[" << endl
                << "  irradiance = "<<m_irradiance<<","<<endl
                << "  nearClip = " << m_nearClip << "," << endl
                << "  farClip = " << m_farClip << "," << endl
                << "  worldTransform = " << indent(m_worldTransform.toString()) << "," << endl
                << "  medium = " << indent(m_medium.toString()) << "," << endl
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

        Float m_irradiance;
        Float m_power;
        Float m_im_power;
        Transform m_emitterToSample;
        Transform m_sampleToEmitter;
        Float m_invSurfaceArea, m_scale;
    };

    MTS_IMPLEMENT_CLASS_S(OrthographicEmitter, false, ProjectiveEmitter)
    MTS_EXPORT_PLUGIN(OrthographicEmitter, "Orthographics emitter");
MTS_NAMESPACE_END

