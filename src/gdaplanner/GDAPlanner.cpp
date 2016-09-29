#include <gdaplanner/GDAPlanner.h>


namespace gdaplanner {
  GDAPlanner::GDAPlanner() {
  }
  
  GDAPlanner::~GDAPlanner() {
  }
  
  void GDAPlanner::popContext() {
    if(m_dqContexts.size() > 0) {
      m_dqContexts.pop_front();
    } else {
      throw std::exception();
    }
  }
  
  Context::Ptr GDAPlanner::currentContext() {
    if(m_dqContexts.size() > 0) {
      return m_dqContexts.front();
    }
    
    return nullptr;
  }
  
  Expression GDAPlanner::simpleExpression(std::string strSource) {
    unsigned int unPos = 0;
    return Expression(Expression::parseString(strSource, unPos)[0]);
  }
}
