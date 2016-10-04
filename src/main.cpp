#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_SUCCESS;
  
  GDAPlanner::Ptr gdapPlanner = GDAPlanner::create();
  gdapPlanner->readContextFile<loaders::PDDL>("../data/openstacks-domain.pddl");
  contexts::Context::Ptr ctxContext = gdapPlanner->currentContext();
  
  if(ctxContext) {
    std::cout << *ctxContext << std::endl;
    
    problems::Problem::Ptr prbProblem = gdapPlanner->readProblemFile<loaders::PDDL>("../data/openstacks-problem.pddl");
    
    if(prbProblem) {
      std::cout << *prbProblem << std::endl;
    } else {
      std::cerr << "Failed to parse PDDL problem" << std::endl;
    }
  } else {
    std::cerr << "Failed to parse PDDL context" << std::endl;
  }
  
  // Expression exSimple = gdapPlanner->simpleExpression("(table-set \"table-1\")");
  // gdapPlanner->plan<planners::ConvexPlanner>(exSimple);
  
  return nReturnvalue;
}
