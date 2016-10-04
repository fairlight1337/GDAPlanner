#include <gdaplanner/problems/PDDL.h>


namespace gdaplanner {
  namespace problems {
    PDDL::PDDL() {
    }
    
    PDDL::~PDDL() {
    }
    
    void PDDL::setIdentifier(std::string strIdentifier) {
      m_strIdentifier = strIdentifier;
    }
    
    void PDDL::setDomain(std::string strDomain) {
      m_strDomain = strDomain;
    }
    
    void PDDL::addObject(std::string strName, std::string strType) {
      m_vecObjects.push_back({strName, strType});
    }
    
    void PDDL::addInitExpression(Expression exInit) {
      m_vecInit.push_back(exInit);
    }
    
    void PDDL::setInitExpressions(std::vector<Expression> vecInitExpressions) {
      m_vecInit = vecInitExpressions;
    }
    
    void PDDL::setGoal(Expression exGoal) {
      m_exGoal = exGoal;
    }
    
    void PDDL::setMetric(Expression exMetric) {
      m_exMetric = exMetric;
    }
    
    std::string PDDL::identifier() {
      return m_strIdentifier;
    }
    
    std::string PDDL::domain() {
      return m_strDomain;
    }
    
    std::vector<PDDL::Object> PDDL::objects() {
      return m_vecObjects;
    }
    
    std::vector<Expression> PDDL::initExpressions() {
      return m_vecInit;
    }
    
    Expression PDDL::goal() {
      return m_exGoal;
    }
    
    Expression PDDL::metric() {
      return m_exMetric;
    }
    
    std::string PDDL::toString() {
      std::stringstream sts;
      
      sts << "Problem '" << m_strIdentifier << "'" << std::endl;
      sts << " * Domain: " << m_strDomain << std::endl;
      
      if(m_vecObjects.size() > 0) {
	sts << " * Objects:" << std::endl;
	
	for(Object obObject : m_vecObjects) {
	  sts << "    * " << obObject.strName << " (" << obObject.strType << ")" << std::endl;
	}
      }
      
      if(m_vecInit.size() > 0) {
	sts << " * Init Expressions:" << std::endl;
	
	for(Expression exInit : m_vecInit) {
	  sts << "    * " << exInit << std::endl;
	}
      }
      
      sts << " * Goal: " << m_exGoal << std::endl;
      sts << " * Metric: " << m_exMetric << std::endl;
      
      return sts.str();
    }
  }
}
