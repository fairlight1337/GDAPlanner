#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_SUCCESS;
  
  GDAPlanner::Ptr gdapPlanner = GDAPlanner::create();
  gdapPlanner->pushContext("table_setting");
  
  gdapPlanner->readFile("../data/TableSetting.gda");
  
  std::cout << *(gdapPlanner->currentContext()) << std::endl;
  
  Expression exSimple = gdapPlanner->simpleExpression("(table-set \"table-1\")");
  gdapPlanner->plan<planners::ConvexPlanner>(exSimple);
  
  return nReturnvalue;
}
