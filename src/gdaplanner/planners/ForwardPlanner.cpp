#include <gdaplanner/planners/ForwardPlanner.h>


namespace gdaplanner {
  namespace planners {
    ForwardPlanner::ForwardPlanner() {
    }
    
    ForwardPlanner::~ForwardPlanner() {
    }
    
    void ForwardPlanner::plan(problems::PDDL::Ptr prbProblem, contexts::PDDL::Ptr ctxContext) {
      //std::cout << "This PDDL planner isn't implemented yet. Supplied information:" << std::endl;
      
      /*std::cout << *ctxContext << std::endl;
	std::cout << *prbProblem << std::endl;*/
      
      World::Ptr wdWorld = World::create();
      Prolog::Ptr plProlog = Prolog::create(wdWorld);
      
      Prolog::Solution::Ptr solSolution = nullptr;
      
      while(true) {
	solSolution = plProlog->query("(and \
                                            (= ?b (1 2 3)) \
                                            (member ?a ?b))", solSolution);
	
	if(solSolution) {
	  std::cout << "--" << std::endl;
	  std::cout << *solSolution << std::endl;
	} else {
	  break;
	}
      }
    }
    
    void ForwardPlanner::plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext) {
      this->plan(std::dynamic_pointer_cast<problems::PDDL>(prbProblem), std::dynamic_pointer_cast<contexts::PDDL>(ctxContext));
    }
  }
}
