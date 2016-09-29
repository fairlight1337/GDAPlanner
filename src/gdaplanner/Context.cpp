#include <gdaplanner/Context.h>


namespace gdaplanner {
  Context::Context(std::string strIdentifier) : m_strIdentifier(strIdentifier) {
    this->pushWorld();
  }
  
  Context::~Context() {
  }
  
  void Context::declare(std::string strIdentifier, Expression expDeclare) {
    m_vecDeclarations.push_back(Declaration::create(strIdentifier, expDeclare));
  }
  
  void Context::alias(Expression exAlias, std::vector<Expression> vecAliased) {
    m_vecAliases.push_back(Alias::create(exAlias, vecAliased));
  }
  
  void Context::action(Expression exAction, Expression exPreconditions, Expression exEffects) {
    m_vecActions.push_back(Action::create(exAction, exPreconditions, exEffects));
  }
  
  void Context::state(Expression exState, Expression exPreconditions) {
    m_vecStates.push_back(State::create(exState, exPreconditions));
  }
  
  void Context::fact(Expression exFact) {
    this->currentWorld()->assertFact(exFact);
  }
  
  std::vector<State::Ptr> Context::matchingStates(Expression exMatch) {
    std::vector<State::Ptr> vecMatches;
    
    for(State::Ptr stState : m_vecStates) {
      if(stState->state() == exMatch) {
	vecMatches.push_back(stState);
      }
    }
    
    return vecMatches;
  }
  
  
  World::Ptr Context::pushWorld() {
    World::Ptr wdPush = nullptr;
    
    if(m_dqWorlds.size() == 0) {
      wdPush = World::create();
    } else {
      wdPush = this->currentWorld()->copy();
    }
    
    m_dqWorlds.push_front(wdPush);
    
    return wdPush;
  }
  
  bool Context::popWorld() {
    if(m_dqWorlds.size() > 0) {
      m_dqWorlds.pop_front();
      
      return true;
    }
    
    return false;
  }
  
  World::Ptr Context::currentWorld() {
    if(m_dqWorlds.size() > 0) {
      return m_dqWorlds.front();
    }
    
    return nullptr;
  }
  
  std::string Context::toString() {
    std::stringstream sts;
    
    sts << "Context: " << m_strIdentifier << std::endl;
    
    if(m_vecDeclarations.size() > 0) {
      sts << std::endl << "Declarations:" << std::endl;
      for(Declaration::Ptr dclCurrent : m_vecDeclarations) {
	sts << " * " << *dclCurrent << std::endl;
      }
    }
    
    if(m_vecAliases.size() > 0) {
      sts << std::endl << "Aliases:" << std::endl;
      for(Alias::Ptr alsCurrent : m_vecAliases) {
	sts << " * " << *alsCurrent << std::endl;
      }
    }
    
    if(m_vecActions.size() > 0) {
      sts << std::endl << "Actions:" << std::endl;
      for(Action::Ptr actCurrent : m_vecActions) {
	sts << " * " << *actCurrent << std::endl;
      }
    }
    
    if(m_vecStates.size() > 0) {
      sts << std::endl << "States:" << std::endl;
      for(State::Ptr stCurrent : m_vecStates) {
	sts << " * " << *stCurrent << std::endl;
      }
    }
    
    if(m_dqWorlds.size() > 0) {
      sts << std::endl << "World:" << std::endl;
      sts << *(this->currentWorld()) << std::endl;
    }
    
    return sts.str();
  }
}
