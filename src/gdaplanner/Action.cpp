#include <gdaplanner/Action.h>


namespace gdaplanner {
  Action::Action(Predicate::Ptr pdPredicate, Expression exPreconditions, Expression exEffects) : m_pdPredicate(pdPredicate), m_exPreconditions(exPreconditions), m_exEffects(exEffects) {
  }
  
  Action::~Action() {
  }
  
  std::string Action::toString() {
    std::stringstream sts;
    
    sts << "predicate = " << *m_pdPredicate << std::endl;
    sts << "   preconditions = " << m_exPreconditions << std::endl;
    sts << "   effects = " << m_exEffects;
    
    return sts.str();
  }
}
