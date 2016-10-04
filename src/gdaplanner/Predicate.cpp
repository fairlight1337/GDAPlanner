#include <gdaplanner/Predicate.h>


namespace gdaplanner {
  Predicate::Predicate(Expression exExpression) : m_exExpression(exExpression) {
  }
  
  Predicate::~Predicate() {
  }
  
  Expression& Predicate::expression() {
    return m_exExpression;
  }
  
  std::string& Predicate::type(std::string strVariable) {
    return m_mapTypes[strVariable];
  }
  
  bool Predicate::isTyped(std::string strVariable) {
    return (m_mapTypes.find(strVariable) != m_mapTypes.end());
  }
  
  void Predicate::setType(std::string strVariable, std::string strType) {
    m_mapTypes[strVariable] = strType;
  }
  
  std::vector<std::string> Predicate::variables(Expression& exExpression) {
    std::vector<std::string> vecVariables;
    
    if(exExpression.type() == Expression::String) {
      if(!exExpression.isWildcard()) {
	std::string strVariable = exExpression.get<std::string>();
	
	if(strVariable.size() > 1) {
	  if(strVariable[0] == '?') {
	    if(std::find(vecVariables.begin(), vecVariables.end(), strVariable) == vecVariables.end()) {
	      vecVariables.push_back(strVariable);
	    }
	  }
	}
      }
    } else if(exExpression.type() == Expression::List) {
      for(Expression exSub : exExpression.subExpressions()) {
	std::vector<std::string> vecVariablesSub = Predicate::variables(exSub);
	
	for(std::string strVariable : vecVariablesSub) {
	  if(std::find(vecVariables.begin(), vecVariables.end(), strVariable) == vecVariables.end()) {
	    vecVariables.push_back(strVariable);
	  }
	}
      }
    }
    
    return vecVariables;
  }
  
  std::vector<std::string> Predicate::variables() {
    return Predicate::variables(m_exExpression);
  }
  
  std::string Predicate::toString() {
    std::stringstream sts;
    
    sts << m_exExpression;
    
    for(std::map<std::string, std::string>::iterator itType = m_mapTypes.begin();
	itType != m_mapTypes.end(); ++itType) {
      sts << ", " << itType->first << " = " << itType->second;
    }
    
    return sts.str();
  }
}
