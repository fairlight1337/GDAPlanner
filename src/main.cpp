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
    problems::Problem::Ptr prbProblem = gdapPlanner->readProblemFile<loaders::PDDL>("../data/openstacks-problem.pddl");
    
    if(prbProblem) {
      gdapPlanner->plan<planners::ConvexPlanner>(prbProblem);
    } else {
      std::cerr << "Failed to parse PDDL problem" << std::endl;
    }
  } else {
    std::cerr << "Failed to parse PDDL context" << std::endl;
  }
  
  return nReturnvalue;
}
