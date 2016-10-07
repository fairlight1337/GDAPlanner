#include <gdaplanner/Declaration.h>


namespace gdaplanner {
  Declaration::Declaration(std::string strIdentifier, Expression expDeclare) : m_strIdentifier(strIdentifier), m_expDeclare(expDeclare) {
  }
  
  Declaration::~Declaration() {
  }
  
  std::string Declaration::toString() const {
    std::stringstream sts;
    
    sts << m_strIdentifier << " = " << m_expDeclare.toString();
    
    return sts.str();
  }
}
