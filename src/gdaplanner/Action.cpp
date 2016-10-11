#include <gdaplanner/Action.h>


namespace gdaplanner {
  Action::Action(Predicate::Ptr pdPredicate, Expression exPreconditions, Expression exEffects) : m_pdPredicate(pdPredicate), m_exPreconditions(exPreconditions), m_exEffects(exEffects) {
  }
  
  Action::~Action() {
  }
  
  Expression Action::expression() {
    Expression exAction;
    
    Expression exPredicate;
    exPredicate.add("predicate");
    exPredicate.add(m_pdPredicate->expression());
    // TODO: Add the types
    exAction.add(exPredicate);
    
    Expression exPrecondition;
    exPrecondition.add("precondition");
    exPrecondition.add(m_exPreconditions);
    exAction.add(exPrecondition);
    
    Expression exEffect;
    exEffect.add("effect");
    exEffect.add(m_exEffects);
    exAction.add(exEffect);
    
    return exAction;
  }
  
  std::string Action::toString() {
    std::stringstream sts;
    
    sts << "predicate = " << *m_pdPredicate << std::endl;
    sts << "   preconditions = " << m_exPreconditions << std::endl;
    sts << "   effects = " << m_exEffects;
    
    return sts.str();
  }
}
