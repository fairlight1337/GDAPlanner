#include <gdaplanner/Alias.h>


namespace gdaplanner {
  Alias::Alias(Expression exAlias, std::vector<Expression> vecAliased) : m_exAlias(exAlias), m_vecAliased(vecAliased) {
  }
  
  Alias::~Alias() {
  }
  
  std::string Alias::toString() const {
    std::stringstream sts;
    
    sts << "alias = " << m_exAlias.toString() << ", aliased = [";
    
    bool bFirst = true;
    for(Expression exAliased : m_vecAliased) {
      if(bFirst) {
	bFirst = false;
      } else {
	sts << ", ";
      }
      
      sts << exAliased;
    }
    
    sts << "]";
    
    return sts.str();
  }
}
