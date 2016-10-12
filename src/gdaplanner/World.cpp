#include <gdaplanner/World.h>
#include <iostream>


namespace gdaplanner {
  World::World() {
  }
  
  World::~World() {
  }
  
  bool World::assertFact(Expression exFact) {
    if(this->holds(exFact, true).size() == 0) {
      Expression exFactNot = exFact.negate();
      
      if(this->holds(exFactNot, true).size() > 0) {
	this->retractFact(exFactNot);
      }
      
      m_lstFacts.push_back(exFact);
      
      return true;
    }
    
    return false;
  }
  
  std::vector<std::map<std::string, Expression>> World::retractFact(Expression exFact, unsigned int unLimit, bool bExact) {
    std::vector<std::map<std::string, Expression>> vecResolutions;
    bool bNotDone = true;
    
    while(bNotDone) {
      std::list<Expression>::iterator itFact = m_lstFacts.begin();
      
      for(; itFact != m_lstFacts.end(); ++itFact) {
	bool bResolved;
	std::map<std::string, Expression> mapResolution = exFact.resolve(*itFact, bResolved, bExact);
	
	if(bResolved) {
	  vecResolutions.push_back(mapResolution);
	  m_lstFacts.erase(itFact);
	  
	  break;
	}
      }
      
      bNotDone = (itFact != m_lstFacts.end() && (unLimit == 0 || vecResolutions.size() < unLimit));
    }
    
    return vecResolutions;
  }
  
  std::vector<std::map<std::string, Expression>> World::holds(Expression exFact, bool bExact) {
    std::vector<std::map<std::string, Expression>> vecResolutions;
    
    for(Expression exWorldFact : m_lstFacts) {
      bool bResolved;
      std::map<std::string, Expression> mapResolution = exFact.resolve(exWorldFact, bResolved, bExact);
      
      if(bResolved) {
	vecResolutions.push_back(mapResolution);
	
	if(bExact) {
	  break;
	}
      }
    }
    
    return vecResolutions;
  }
  
  World::Ptr World::copy() {
    return World::create(*this);
  }
  
  std::string World::toString() const {
    std::stringstream sts;
    
    if(m_lstFacts.size() > 0) {
      sts << "Facts:" << std::endl;
      
      for(Expression exFact : m_lstFacts) {
	sts << " * " << exFact << std::endl;
      }
    } else {
      sts << "Empty world" << std::endl;
    }
    
    return sts.str();
  }
}
