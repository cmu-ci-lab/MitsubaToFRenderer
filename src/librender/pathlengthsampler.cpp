#include <mitsuba/render/pathlengthsampler.h>
#include <boost/algorithm/string.hpp>

MTS_NAMESPACE_BEGIN

PathLengthSampler::PathLengthSampler(const Properties &props)
	: ConfigurableObject(props){
	std::string modulationType = boost::to_lower_copy(
					props.getString("modulation", "none"));

	m_decompositionMinBound = props.getFloat("minBound", 0.0f);
	m_decompositionMaxBound = props.getFloat("maxBound", 0.0f);
	m_lambda 				= props.getFloat("lambda",1);
	m_phase 				= props.getFloat("phase",0)*M_PI/180;
	m_P						= props.getInteger("P",32);
	m_neighbors				= props.getInteger("neighbors",3);

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
PathLengthSampler::PathLengthSampler(Stream *stream, InstanceManager *manager)
	: ConfigurableObject(stream, manager){
	m_decompositionMinBound = stream->readFloat();
	m_decompositionMaxBound = stream->readFloat();
	m_modulationType		= (EModulationType)stream->readUInt();
	m_lambda				= stream->readFloat();
	m_phase					= stream->readFloat();
	m_P						= stream->readUInt();
	m_neighbors				= stream->readUInt();
}

void PathLengthSampler::addChild(const std::string &name, ConfigurableObject *child) {  }

void PathLengthSampler::serialize(Stream *stream, InstanceManager *manager) const {
	ConfigurableObject::serialize(stream, manager);

	stream->writeFloat(m_decompositionMinBound);
	stream->writeFloat(m_decompositionMaxBound);
	stream->writeUInt(m_modulationType);
	stream->writeFloat(m_lambda);
	stream->writeFloat(m_phase);
	stream->writeUInt(m_P);
	stream->writeUInt(m_neighbors);
}

void PathLengthSampler::configure() {  }

PathLengthSampler::~PathLengthSampler() { }

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

MTS_IMPLEMENT_CLASS(PathLengthSampler, true, ConfigurableObject)

MTS_NAMESPACE_END
