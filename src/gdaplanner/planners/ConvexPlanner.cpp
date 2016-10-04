#include <gdaplanner/planners/ConvexPlanner.h>


namespace gdaplanner {
  namespace planners {
    ConvexPlanner::ConvexPlanner() {
    }
    
    ConvexPlanner::~ConvexPlanner() {
    }
    
    void ConvexPlanner::plan(State::Ptr stGoal, contexts::StatefulContext::Ptr ctxContext) {
      World::Ptr wdWorld = ctxContext->currentWorld();
      
      std::cout << "This planner isn't implemented yet." << std::endl;
    }
    
    void ConvexPlanner::plan(State::Ptr stGoal, contexts::Context::Ptr ctxContext) {
      this->plan(stGoal, std::dynamic_pointer_cast<contexts::StatefulContext>(ctxContext));
    }
  }
}
