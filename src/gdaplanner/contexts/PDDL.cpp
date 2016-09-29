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
      for(Predicate prPred : m_vecPredicates) {
	sts << " * " << prPred << std::endl;
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
    
    bool PDDL::addPredicate(Expression exPredicate) {
      if(exPredicate.type() == Expression::List) {
	if(exPredicate.size() > 0) {
	  std::string strPredicate = exPredicate[0].get<std::string>();
	  
	  Expression exTypedVariable(Expression::parseString("(?name - ?type)"));
	  
	  while(exPredicate.size() > 0) {
	    bool bResolved;
	    
	    if(exPredicate.size() >= 3) {
	      Expression exSub = Expression(exPredicate.subSequence(0, 3));
	      std::map<std::string, Expression> mapResolution = exTypedVariable.resolve(exSub, bResolved);
	      
	      if(bResolved) {
		// ...
		exPredicate = Expression(exPredicate.subSequence(3));
	      }
	    }
	    
	    if(!bResolved) {
	      Expression exSub = exPredicate[0];
	      
	      if(exSub.isVariable()) {
		// ...
		bResolved = true;
	      }
	    }
	    
	    if(!bResolved) {
	      std::cerr << "Couldn't resolve '" << exPredicate << "' into a predicate." << std::endl;
	      return false;
	    }
	  }
	  
	  return true;
	}
      }
      
      return false;
    }
  }
}
