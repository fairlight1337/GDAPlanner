#pragma once
#ifndef __FORWARDPLANNER_H__
#define __FORWARDPLANNER_H__


#include <memory>
#include <iosfwd>

#include <gdaplanner/Planner.h>
#include <gdaplanner/contexts/PDDL.h>
#include <gdaplanner/problems/PDDL.h>

namespace gdaplanner {
  namespace planners {

  bool holds(Expression const& goal, Expression const& state);
  void updateState(Expression const& effects, Expression const& state, Expression & newState);
  bool holds(Expression const& goal, Expression const& state, std::map<std::string, Expression> & bdgs);
  void updateState(Expression const& effects, Expression const& state, Expression & newState, std::map<std::string, Expression> & bdgs);

    class ForwardPlanner : public Planner {
    public:
      /** Shared pointer to ForwardPlanner class */
      typedef std::shared_ptr<ForwardPlanner> Ptr;
      
    private:
    protected:
    public:
      ForwardPlanner();
      ~ForwardPlanner();
      
      Solution::Ptr plan(problems::PDDL::Ptr prbProblem, contexts::PDDL::Ptr ctxContext, Solution::Ptr solPrior = Solution::create());
      Solution::Ptr plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext, Solution::Ptr solPrior = Solution::create()) override;
      
      template<class ... Args>
	static ForwardPlanner::Ptr create(Args ... args) {
	return std::make_shared<ForwardPlanner>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __FORWARDPLANNER_H__ */
