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
      
      Solution solSolution;
      
      while(true) {
	// solSolution = plProlog->query("(and (= ?a ((1 2) (3 4))) \
        //                                     (member ?b ?a) \
        //                                     (member ?c ?b))"
	// 			      , solSolution);
	//solSolution = plProlog->query("(and (= ?a ((1 2) (3 4)))	\
	//                                  (member ?b ?a))"
	//			      , solSolution);
	//solSolution = plProlog->query("(= (?a (?b ?c 4 ?d)) ((1 a) (2 3 4 5)))", solSolution);
	//solSolution = plProlog->query("(member ?a ((1 2) (3 4) (a b)))", solSolution);
	solSolution = plProlog->query("(member (?a ?b) ((1 2) (3 4) (a b)))", solSolution);
	
	if(solSolution.valid()) {
	  std::cout << "\033[1;31m" << std::endl;
	  std::cout << "------------" << std::endl;
	  std::cout << solSolution;
	  std::cout << "------------" << std::endl;
	  std::cout << "\033[0m";
	} else {
	  break;
	}
	
	//break;
      }
    }
    
    void ForwardPlanner::plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext) {
      this->plan(std::dynamic_pointer_cast<problems::PDDL>(prbProblem), std::dynamic_pointer_cast<contexts::PDDL>(ctxContext));
    }
  }
}
