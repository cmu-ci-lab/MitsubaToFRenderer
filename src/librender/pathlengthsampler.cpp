#include <mitsuba/render/pathlengthsampler.h>

MTS_NAMESPACE_BEGIN

Float PathLengthSampler::correlationFunction(Float t) const {
	switch(m_modulationType){
		case ENone:{
			SLog(EError, "Cannot call correlation function when the modulation type is not defined");
			break;
		}
		case ESine:{
			t = t + m_phase*m_lambda*INV_PI/2;
			return cos(t*2*M_PI/m_lambda);
			break;
		}
		case ESquare:{
			t = t + m_phase*m_lambda*INV_PI/2;
			return 4/m_lambda*(fabs(fmod(t, m_lambda)-m_lambda/2) - m_lambda/4);
			break;
		}
		case EHamiltonian:{
			t = t + m_phase*m_lambda*INV_PI/2;
			t = fmod(t, m_lambda);
			if(t < m_lambda/6){
				return 6*t/m_lambda;
			}else if(t < m_lambda/2 	&& t >= m_lambda/6){
				return 1.0;
			}else if(t < 2*m_lambda/3 	&& t >= m_lambda/2){
				return 1 - (t - m_lambda/2)*6/m_lambda;
			}else{
				return 0;
			}
			break;
		}
		case EMSeq:{
			return mSeq(t, m_phase);
			break;
		}
		case EDepthSelective:{
			Float value = 0;
			for(int i = 0; i < m_neighbors; i++){
				value += mSeq(t, m_phase + i*(2*M_PI)/m_P);
			}
			value -= (float)(m_neighbors-1)/m_P;
			return value;
			break;
		}
		default:
			SLog(EError, "Modulation type is not defined");
	}
	return 0;
}

MTS_NAMESPACE_END
