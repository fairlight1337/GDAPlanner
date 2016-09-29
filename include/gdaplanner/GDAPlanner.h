#ifndef __GDAPLANNER_H__
#define __GDAPLANNER_H__


#include <memory>
#include <iostream>
#include <deque>
#include <fstream>
#include <streambuf>

#include <gdaplanner/Context.h>
#include <gdaplanner/World.h>

#include <gdaplanner/Planner.h>
#include <gdaplanner/planners/ConvexPlanner.h>


namespace gdaplanner {
  class GDAPlanner {
  public:
    typedef std::shared_ptr<GDAPlanner> Ptr;
    
  private:
    std::deque<Context::Ptr> m_dqContexts;
    
  protected:
  public:
    GDAPlanner();
    ~GDAPlanner();
    
    template<class ... Args>
      Context::Ptr pushContext(Args ... args) {
      Context::Ptr ctxNew = Context::create(std::forward<Args>(args)...);
      m_dqContexts.push_front(ctxNew);
      
      return ctxNew;
    }
    
    void popContext();
    
    Context::Ptr currentContext();
    
    bool readFile(std::string strFilepath);
    bool parseString(std::string strSource);
    std::vector<Expression> parseSegments(std::string& strSource, unsigned int& unPos);
    
    bool processExpression(Expression exProcess);
    
    Expression simpleExpression(std::string strSource);
    
    template<class T>
      bool plan(Expression exTargetState) {
      std::vector<State::Ptr> vecMatchingStates = this->currentContext()->matchingStates(exTargetState);
      
      if(vecMatchingStates.size() > 0) {
	for(State::Ptr stGoalState : vecMatchingStates) {
	  State::Ptr stParametrizedGoal = stGoalState->parametrize(exTargetState);
	  
	  Planner::Ptr prPlanner = T::create();
	  prPlanner->plan(stParametrizedGoal, this->currentContext());
	}
	
	return true;
      }
      
      return false;
    }
    
    template<class ... Args>
      static GDAPlanner::Ptr create(Args ... args) {
      return std::make_shared<GDAPlanner>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __GDAPLANNER_H__ */
