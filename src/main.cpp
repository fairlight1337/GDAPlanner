#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_SUCCESS;
  
  GDAPlanner::Ptr gdapPlanner = GDAPlanner::create();
  gdapPlanner->readContextFile<loaders::PDDL>("../data/simple-ts-domain.pddl");
  contexts::Context::Ptr ctxContext = gdapPlanner->currentContext();
  
  if(ctxContext) {
    problems::Problem::Ptr prbProblem = gdapPlanner->readProblemFile<loaders::PDDL>("../data/simple-tablesetting.pddl");
    
    if(prbProblem) {
      std::cout << "Problem parsed, forwarding it to the planner" << std::endl;
      Solution::Ptr solSolution = gdapPlanner->plan<planners::ForwardPlanner>(prbProblem);
      std::cout << "Planner concluded" << std::endl;
      
      if(solSolution && solSolution->valid()) {
	std::cout << "Solution:" << std::endl;
	std::cout << *solSolution << std::endl;
      } else {
	std::cerr << "No solution" << std::endl;
      }
    } else {
      std::cerr << "Failed to parse PDDL problem" << std::endl;
    }
  } else {
    std::cerr << "Failed to parse PDDL context" << std::endl;
  }
  
  return nReturnvalue;
}
