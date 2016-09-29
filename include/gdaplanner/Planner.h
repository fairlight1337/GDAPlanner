#ifndef __PLANNER_H__
#define __PLANNER_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Context.h>


namespace gdaplanner {
  class Planner {
  public:
    typedef std::shared_ptr<Planner> Ptr;
    
  private:
  protected:
  public:
    Planner();
    ~Planner();
    
    virtual void plan(State::Ptr stGoal, Context::Ptr ctxContext) = 0;
    
    template<class ... Args>
      static Planner::Ptr create(Args ... args) {
      return std::make_shared<Planner>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PLANNER_H__ */
