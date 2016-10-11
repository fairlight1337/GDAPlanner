#include <gdaplanner/contexts/Context.h>


namespace gdaplanner {
  namespace contexts {
    Context::Context(std::string strIdentifier) : m_strIdentifier(strIdentifier) {
    }
  
    Context::~Context() {
    }
  
    void Context::declare(std::string strIdentifier, Expression expDeclare) {
      m_vecDeclarations.push_back(Declaration::create(strIdentifier, expDeclare));
    }
  
    void Context::alias(Expression exAlias, std::vector<Expression> vecAliased) {
      m_vecAliases.push_back(Alias::create(exAlias, vecAliased));
    }
  
    void Context::action(Expression exPreconditions, Expression exEffects) {
      m_vecActions.push_back(Action::create(nullptr, exPreconditions, exEffects));
    }
  
    void Context::state(Expression exState, Expression exPreconditions) {
      m_vecStates.push_back(State::create(exState, exPreconditions));
    }
    
    unsigned int Context::actionCount() {
      return m_vecActions.size();
    }
    
    Action::Ptr Context::action(unsigned int unIndex) {
      return m_vecActions[unIndex];
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
  
    std::string Context::toString() const {
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
    
      return sts.str();
    }
  }
}
