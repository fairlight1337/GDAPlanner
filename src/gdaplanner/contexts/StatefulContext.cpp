#include <gdaplanner/contexts/StatefulContext.h>


namespace gdaplanner {
  StatefulContext::StatefulContext(std::string strIdentifier) : Context(strIdentifier) {
    this->pushWorld();
  }
  
  StatefulContext::~StatefulContext() {
  }
  
  World::Ptr StatefulContext::pushWorld() {
    World::Ptr wdPush = nullptr;
    
    if(m_dqWorlds.size() == 0) {
      wdPush = World::create();
    } else {
      wdPush = this->currentWorld()->copy();
    }
    
    m_dqWorlds.push_front(wdPush);
    
    return wdPush;
  }
  
  bool StatefulContext::popWorld() {
    if(m_dqWorlds.size() > 0) {
      m_dqWorlds.pop_front();
      
      return true;
    }
    
    return false;
  }
  
  World::Ptr StatefulContext::currentWorld() {
    if(m_dqWorlds.size() > 0) {
      return m_dqWorlds.front();
    }
    
    return nullptr;
  }
  
  void StatefulContext::fact(Expression exFact) {
    this->currentWorld()->assertFact(exFact);
  }
  
  std::string StatefulContext::toString() {
    std::stringstream sts;
    
    sts << this->Context::toString();
    
    if(m_dqWorlds.size() > 0) {
      sts << std::endl << "World:" << std::endl;
      sts << *(this->currentWorld()) << std::endl;
    }
    
    return sts.str();
  }
}
