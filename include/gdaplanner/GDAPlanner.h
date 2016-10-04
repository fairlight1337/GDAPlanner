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
    std::deque<contexts::Context::Ptr> m_dqContexts;
    
  protected:
  public:
    GDAPlanner();
    ~GDAPlanner();
    
    template<class T, class ... Args>
      contexts::Context::Ptr pushContext(Args ... args) {
      contexts::Context::Ptr ctxNew = T::create(std::forward<Args>(args)...);
      
      this->pushContextInstance(ctxNew);
      
      return ctxNew;
    }
    
    void pushContextInstance(contexts::Context::Ptr ctxPush) {
      m_dqContexts.push_front(ctxPush);
    }
    
    void popContext();
    
    contexts::Context::Ptr currentContext();
    
    template<class TLoader>
      bool readContextFile(std::string strFilepath) {
      typename TLoader::Ptr tLoader = TLoader::create();
      contexts::Context::Ptr ctxNew = tLoader->readContextFile(strFilepath);
      
      if(ctxNew) {
	this->pushContextInstance(ctxNew);
	
	return true;
      }
      
      return false;
    }
    
    template<class TLoader>
      problems::Problem::Ptr readProblemFile(std::string strFilepath) {
      return TLoader::create()->readProblemFile(strFilepath);
    }
    
    bool parseString(std::string strSource);
    
    bool processExpression(Expression exProcess);
    
    Expression simpleExpression(std::string strSource);
    
    template<class T>
      bool plan(problems::Problem::Ptr prbProblem) {
      //std::vector<State::Ptr> vecMatchingStates = this->currentContext()->matchingStates(exTargetState);
      
      /*if(vecMatchingStates.size() > 0) {
	for(State::Ptr stGoalState : vecMatchingStates) {
	  State::Ptr stParametrizedGoal = stGoalState->parametrize(exTargetState);
	  
	}
	
	return true;
	}*/
      
      Planner::Ptr prPlanner = T::create();
      prPlanner->plan(prbProblem, this->currentContext());
      
      return true;
    }
    
    template<class ... Args>
      static GDAPlanner::Ptr create(Args ... args) {
      return std::make_shared<GDAPlanner>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __GDAPLANNER_H__ */
