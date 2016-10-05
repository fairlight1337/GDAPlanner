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
#include <gdaplanner/planners/ForwardPlanner.h>


namespace gdaplanner {
  /** \brief Main GDAPlanner management class
      
      This class manages all loaded contexts and supplies convenience
      functions and function wrappers for reading context and problem
      files, accessing active contexts, and starting planning.
  
      \since 0.1.0 */
  class GDAPlanner {
  public:
    /** \brief Shared pointer to GDAPlanner class */
    typedef std::shared_ptr<GDAPlanner> Ptr;
    
  private:
    /** \brief Deque holding the loaded Context instances */
    std::deque<contexts::Context::Ptr> m_dqContexts;
    
  protected:
  public:
    /** \brief Default constructor */
    GDAPlanner();
    /** \brief Default destructor */
    ~GDAPlanner();
    
    template<class T, class ... Args>
      contexts::Context::Ptr pushContext(Args ... args) {
      contexts::Context::Ptr ctxNew = T::create(std::forward<Args>(args)...);
      
      this->pushContextInstance(ctxNew);
      
      return ctxNew;
    }
    
    /** \brief Push a Context instance onto the stack and make it current */
    void pushContextInstance(contexts::Context::Ptr ctxPush) {
      m_dqContexts.push_front(ctxPush);
    }
    
    /** \brief Remove the topmost context from the stack
	
	If the stack is empty, a std::exception is thrown.
	
	\throws std::exception Thrown when called with an empty stack */
    void popContext();
    
    /** \brief Return the topmost Context instance from the stack
	
	Returns `NULL` if the Context stack is empty.
	
	\return Currently topmost Context instance from the stack, or NULL */
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
