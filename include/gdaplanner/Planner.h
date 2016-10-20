#pragma once
#ifndef __PLANNER_H__
#define __PLANNER_H__


#include <memory>
#include <iosfwd>

#include <gdaplanner/contexts/Context.h>
#include <gdaplanner/contexts/StatefulContext.h>
#include <gdaplanner/problems/Problem.h>
#include <gdaplanner/World.h>
#include <gdaplanner/Prolog.h>


namespace gdaplanner {
  class Planner {
  public:
    /** Shared pointer to Planner class */
    typedef std::shared_ptr<Planner> Ptr;
    
  private:
  protected:
  public:
    Planner();
    ~Planner();
    
    virtual Solution::Ptr plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext, Solution::Ptr solPrior = Solution::create()) = 0;
    
    template<class ... Args>
      static Planner::Ptr create(Args ... args) {
      return std::make_shared<Planner>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PLANNER_H__ */
