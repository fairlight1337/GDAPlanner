#include <gdaplanner/State.h>


namespace gdaplanner {
  State::State(Expression exState, Expression exPreconditions) : m_exState(exState), m_exPreconditions(exPreconditions) {
  }
  
  State::~State() {
  }
  
  Expression State::state() {
    return m_exState;
  }
  
  Expression State::preconditions() {
    return m_exPreconditions;
  }
  
  State::Ptr State::parametrize(Expression exTarget) {
    bool bResolved;
    std::map<std::string, Expression> mapParametrization = m_exState.resolve(exTarget, bResolved);
    
    if(bResolved) {
      Expression exState = m_exState.parametrize(mapParametrization);
      Expression exPreconditions = m_exPreconditions.parametrize(mapParametrization);
      
      return State::create(exState, exPreconditions);
    }
    
    return nullptr;
  }
  
  std::string State::toString() {
    std::stringstream sts;
    
    sts << "state = " << m_exState << ", precondition = " << m_exPreconditions;
    
    return sts.str();
  }
}
