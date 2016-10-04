#ifndef __CONVEXPLANNER_H__
#define __CONVEXPLANNER_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Planner.h>
#include <gdaplanner/contexts/PDDL.h>
#include <gdaplanner/problems/PDDL.h>


namespace gdaplanner {
  namespace planners {
    class ConvexPlanner : public Planner {
    public:
      typedef std::shared_ptr<ConvexPlanner> Ptr;
      
    private:
    protected:
    public:
      ConvexPlanner();
      ~ConvexPlanner();
      
      void plan(problems::PDDL::Ptr prbProblem, contexts::PDDL::Ptr ctxContext);
      void plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext) override;
      
      template<class ... Args>
	static ConvexPlanner::Ptr create(Args ... args) {
	return std::make_shared<ConvexPlanner>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __CONVEXPLANNER_H__ */
