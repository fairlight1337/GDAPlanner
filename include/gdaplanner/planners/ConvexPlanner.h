#ifndef __CONVEXPLANNER_H__
#define __CONVEXPLANNER_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Planner.h>


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
      
      void plan(State::Ptr stGoal, StatefulContext::Ptr ctxContext);
      void plan(State::Ptr stGoal, Context::Ptr ctxContext) override;
      
      template<class ... Args>
	static ConvexPlanner::Ptr create(Args ... args) {
	return std::make_shared<ConvexPlanner>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __CONVEXPLANNER_H__ */
