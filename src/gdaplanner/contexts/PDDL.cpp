#include <gdaplanner/contexts/PDDL.h>


namespace gdaplanner {
  namespace contexts {
    std::string PDDL::toString() {
      std::stringstream sts;
      
      sts << "Domain: " << m_strDomain << std::endl;
      
      sts << "Requirements:";
      
      for(std::string strRequirement : m_vecRequirements) {
	sts << " " << strRequirement;
      }
      
      sts << std::endl;

      sts << "Types:";
      
      for(std::string strType : m_vecTypes) {
	sts << " " << strType;
      }
      
      sts << std::endl;
      
      sts << "Predicates:" << std::endl;
      for(Predicate::Ptr prPred : m_vecPredicates) {
	sts << " * " << *prPred << std::endl;
      }
      
      return sts.str();
    }
    
    void PDDL::domain(std::string strDomain) {
      m_strDomain = strDomain;
    }
    
    std::string PDDL::domain() {
      return m_strDomain;
    }
    
    void PDDL::requirement(std::string strRequirement) {
      if(!this->requires(strRequirement)) {
	m_vecRequirements.push_back(strRequirement);
      }
    }
    
    std::vector<std::string> PDDL::requirements() {
      return m_vecRequirements;
    }
    
    bool PDDL::requires(std::string strRequirement) {
      return (std::find(m_vecRequirements.begin(), m_vecRequirements.end(), strRequirement) != m_vecRequirements.end());
    }
    
    void PDDL::type(std::string strType) {
      if(!this->hasType(strType)) {
	m_vecTypes.push_back(strType);
      }
    }
    
    std::vector<std::string> PDDL::types() {
      return m_vecTypes;
    }
    
    bool PDDL::hasType(std::string strType) {
      return (std::find(m_vecTypes.begin(), m_vecTypes.end(), strType) != m_vecTypes.end());
    }
    
    Predicate::Ptr PDDL::parsePredicate(Expression exPredicate) {
      Expression exArguments = exPredicate.subSequence(1);
      
      std::vector<std::pair<Expression, Expression>> vecTypedVariables;
      
      while(exArguments.size() > 0) {
	std::vector<Expression> vecVariables;
	
	while(exArguments.size() > 0 && exArguments[0].isVariable()) {
	  vecVariables.push_back(exArguments[0]);
	  exArguments = exArguments.subSequence(1);
	}
	
	if(exArguments.size() > 0) {
	  if(exArguments[0] == "-") {
	    exArguments = exArguments.subSequence(1);
	    
	    if(exArguments.size() > 0) {
	      for(Expression exVariables : vecVariables) {
		vecTypedVariables.push_back({exVariables, exArguments[0]});
	      }
	      
	      exArguments = exArguments.subSequence(1);
	      vecVariables.clear();
	    } else {
	      std::cerr << "Missing type specifier: " << exPredicate << std::endl;
	      break;
	    }
	  } else {
	    Expression exPartial =  exArguments[0];
	    std::cerr << "Unexpected identifier: " << exPartial << " (context: " << exPredicate << ")" << std::endl;
	    break;
	  }
	} else {
	  // No typing
	  vecVariables.clear();
	}
      }
      
      Expression exAdd = exPredicate[0];
      for(std::pair<Expression, Expression> prPair : vecTypedVariables) {
	exAdd.add(prPair.first);
      }
      
      Predicate::Ptr pdAdd = Predicate::create(exAdd);
      for(std::pair<Expression, Expression> prPair : vecTypedVariables) {
	pdAdd->setType(prPair.first.get<std::string>(), prPair.second.get<std::string>());
      }
      
      return pdAdd;
    }
    
    bool PDDL::addPredicate(Expression exPredicate) {
      if(exPredicate.type() == Expression::List) {
	Predicate::Ptr pdPredicate = this->parsePredicate(exPredicate);
	
	if(pdPredicate) {
	  m_vecPredicates.push_back(pdPredicate);
	} else {
	  return false;
	}
	
	return true;
      }
      
      return false;
    }
    
    bool PDDL::addFunctions(Expression exFunctions) {
      // ...
      
      return true;
    }
    
    bool PDDL::addAction(Expression exAction) {
      if(exAction.size() > 0) {
	Expression exName = exAction.popFront();
	
	Expression exParameters;
	Expression exPrecondition;
	Expression exEffect;
	
	while(exAction.size() > 0) {
	  Expression* exTarget = nullptr;
	  
	  if(exAction[0] == ":parameters") {
	    exTarget = &exParameters;
	  } else if(exAction[0] == ":precondition") {
	    exTarget = &exPrecondition;
	  } else if(exAction[0] == ":effect") {
	    exTarget = &exEffect;
	  }
	  
	  exAction.popFront();
	  
	  if(exTarget) {
	    if(exAction.size() > 0) {
	      *exTarget = exAction.popFront();
	    }
	  }
	}
	
	Expression exPrePredicate = exParameters;
	exPrePredicate.pushFront(exName);
	
	Predicate::Ptr pdPredicate = this->parsePredicate(exPrePredicate);
	
	Action::Ptr acAdd = Action::create(pdPredicate, exPrecondition, exEffect);
	m_vecActions.push_back(acAdd);
	
	return true;
      }
      
      return false;
    }
  }
}
