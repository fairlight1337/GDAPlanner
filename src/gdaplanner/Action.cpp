#include <gdaplanner/Action.h>


namespace gdaplanner {
  Action::Action(Expression exAction, Expression exPreconditions, Expression exEffects) : m_exAction(exAction), m_exPreconditions(exPreconditions), m_exEffects(exEffects) {
  }
  
  Action::~Action() {
  }
  
  std::string Action::toString() {
    std::stringstream sts;
    
    sts << "action = " << m_exAction << ", preconditions = " << m_exPreconditions << ", effects = " << m_exEffects;
    
    return sts.str();
  }
}
