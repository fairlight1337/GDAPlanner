#include <gdaplanner/planners/ConvexPlanner.h>


namespace gdaplanner {
  namespace planners {
    ConvexPlanner::ConvexPlanner() {
    }
    
    ConvexPlanner::~ConvexPlanner() {
    }
    
    void ConvexPlanner::plan(State::Ptr stGoal, Context::Ptr ctxContext) {
      World::Ptr wdWorld = ctxContext->currentWorld();
      
      std::cout << "This planner isn't implemented yet." << std::endl;
    }
  }
}
