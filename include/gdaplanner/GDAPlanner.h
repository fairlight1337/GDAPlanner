#ifndef __GDAPLANNER_H__
#define __GDAPLANNER_H__


#include <memory>
#include <iostream>
#include <deque>
#include <fstream>
#include <streambuf>

#include <gdaplanner/contexts/Context.h>
#include <gdaplanner/World.h>

#include <gdaplanner/contexts/PDDL.h>

#include <gdaplanner/loaders/PDDL.h>

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
    
    template<class T, class ... Args>
      Context::Ptr pushContext(Args ... args) {
      Context::Ptr ctxNew = T::create(std::forward<Args>(args)...);
      
      this->pushContextInstance(ctxNew);
      
      return ctxNew;
    }
    
    void pushContextInstance(Context::Ptr ctxPush) {
      m_dqContexts.push_front(ctxPush);
    }
    
    void popContext();
    
    Context::Ptr currentContext();
    
    template<class TLoader>
      bool readFile(std::string strFilepath) {
      typename TLoader::Ptr tLoader = TLoader::create();
      Context::Ptr ctxNew = tLoader->readFile(strFilepath);
      
      if(ctxNew) {
	this->pushContextInstance(ctxNew);
	
	return true;
      }
      
      return false;
    }
    
    bool parseString(std::string strSource);
    
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
