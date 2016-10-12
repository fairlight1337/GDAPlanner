#include <gdaplanner/Solution.h>
#include <iostream>


namespace gdaplanner {
  void Solution::setBindings(Bindings bdgSet) {
    if(m_vecSubSolutions.size() == 0) {
      m_bdgBindings = bdgSet;
    } else {
      m_vecSubSolutions.back().setBindings(bdgSet);
    }
  }
  
  void Solution::clearBindings() {
    m_bdgBindings = Bindings();
    
    for(Solution& solClearBindings : this->subSolutions()) {
      solClearBindings.clearBindings();
    }
  }
  
  Solution::Bindings Solution::Bindings::sanitize(std::string strSuffix) {
    Solution::Bindings bdgSane;
    
    for(std::pair<std::string, Expression> prBinding : this->bindings()) {
      bdgSane[prBinding.first + strSuffix] = prBinding.second;
    }
    
    return bdgSane;
  }
}
