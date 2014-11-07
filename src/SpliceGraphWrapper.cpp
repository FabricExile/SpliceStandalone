#include "SpliceGraphWrapper.h"
#include "macros.h"
#include <boost/version.hpp>
#include <boost/filesystem/path.hpp>

using namespace FabricSplice;

SpliceGraphWrapper::SpliceGraphWrapper(const std::string & path) :
	m_path(path) , m_operatorIsValid(false)
{ 
  FABRIC_TRY("SpliceGraphWrapper::SpliceGraphWrapper", 

    m_dgGraph = DGGraph(name().c_str());
    m_isTimeDependent = -1;

  );

  if (m_path != "")
  {
    reload();
  }

}

std::string SpliceGraphWrapper::name()
{
#if BOOST_VERSION == 105500
  return boost::filesystem::path(m_path).stem().string();
#else
  return boost::filesystem::path(m_path).stem();
#endif
}

void SpliceGraphWrapper::transferParamsToSplice(FabricCore::RTVal params)
{
	m_dgGraph.getDGPort("params").setRTVal(params);
}

FabricSplice::DGGraph SpliceGraphWrapper::getGraph()
{
  return m_dgGraph;
}

bool SpliceGraphWrapper::reload()
{
  FABRIC_TRY_RETURN("SpliceGraphWrapper::reload", false,

    FabricSplice::PersistenceInfo info;
    info.hostAppName = FabricCore::Variant::CreateString("Splice Standalone");
    info.hostAppVersion = FabricCore::Variant::CreateString("1.0");;
    info.filePath = FabricCore::Variant::CreateString(m_path.c_str());

		// build the operator
    // m_dgGraph = DGGraph(name().c_str());
		m_dgGraph.clear();
    m_dgGraph.loadFromFile(m_path.c_str(), &info);
  );

	m_operatorIsValid = true;
  m_isTimeDependent = -1;

	return evaluate();
}

bool SpliceGraphWrapper::isValid() const
{
	return m_operatorIsValid;
}

void SpliceGraphWrapper::setPath(const std::string & path)
{
	m_path = path;
	if (m_path != "")
	{
		reload();
	}
}

bool SpliceGraphWrapper::evaluate(bool force)
{
	// Execute the KL operator to generate a dkScene and return an RTVal to it
  FABRIC_TRY_RETURN("SpliceGraphWrapper::evaluate", false,

		if (!isValid())
		{
			std::cout << "wrapperOp operator cannot be evaluated ..." << std::endl;
			return false;
		}

    if(force)
    {
      DGPort paramsPort = m_dgGraph.getDGPort("params");
      paramsPort.setRTVal(paramsPort.getRTVal());
    }

		m_dgGraph.evaluate();

  );

	return true;
}

bool SpliceGraphWrapper::setFrame(int frame) 
{
  // update the evaluation context's time
  FABRIC_TRY_RETURN("SpliceGraphWrapper::setFrame", false,
    FabricCore::RTVal context = m_dgGraph.getEvalContext();
    context.setMember("time", FabricSplice::constructFloat32RTVal(float(frame) / 24.0));
  );

  if(m_isTimeDependent == -1) {
    FABRIC_TRY_RETURN("SpliceGraphWrapper::setFrame", false,

      FabricSplice::DGPort port = m_dgGraph.getDGPort("time");
      std::string dataType = port.getDataType();
      m_isTimeDependent = (dataType == "Scalar" || dataType == "Float32") && !port.isArray();
    );
  }
  if(m_isTimeDependent == 0)
    return false;

  DGPort port = m_dgGraph.getDGPort("time");

  FABRIC_TRY_RETURN("SpliceGraphWrapper::setFrame", false,

    FabricCore::RTVal timeVal = constructFloat32RTVal(float(frame) / 24.0);
    port.setRTVal(timeVal);

  );
  return evaluate();
}

void SpliceGraphWrapper::dirtyInput(std::string input)
{
  FABRIC_TRY("SpliceGraphWrapper::dirtyInput", 
  
    FabricCore::RTVal context = m_dgGraph.getEvalContext();
    FabricCore::RTVal inputVal = FabricSplice::constructStringRTVal(input.c_str());
    context.callMethod("", "_addDirtyInput", 1, &inputVal);

  );
}
