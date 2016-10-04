#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_SUCCESS;
  
  GDAPlanner::Ptr gdapPlanner = GDAPlanner::create();
  gdapPlanner->readFile<loaders::PDDL>("../data/openstacks-example.pddl");//TableSetting.gda");
  
  Context::Ptr ctxContext = gdapPlanner->currentContext();
  
  if(ctxContext) {
    std::cout << *ctxContext << std::endl;
  } else {
    std::cerr << "Failed to parse PDDL context" << std::endl;
  }
  
  // Expression exSimple = gdapPlanner->simpleExpression("(table-set \"table-1\")");
  // gdapPlanner->plan<planners::ConvexPlanner>(exSimple);
  
  return nReturnvalue;
}
