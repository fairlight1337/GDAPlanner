#include <gdaplanner/Declaration.h>


namespace gdaplanner {
  Declaration::Declaration(std::string strIdentifier, Expression expDeclare) : m_strIdentifier(strIdentifier), m_expDeclare(expDeclare) {
  }
  
  Declaration::~Declaration() {
  }
  
  std::string Declaration::toString() {
    std::stringstream sts;
    
    sts << m_strIdentifier << " = " << m_expDeclare;
    
    return sts.str();
  }
}
