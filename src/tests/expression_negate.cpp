#include <iostream>
#include <cstdlib>
#include <string>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  Expression ex = Expression::parseString("(test)")[0];
  Expression exPlus = Expression::parseString("(not test)")[0];
  Expression exPlusPlus = Expression::parseString("(not test test2)")[0];
  Expression exPlusNot = exPlus.negate();
  Expression exPlusPlusNot = exPlusPlus.negate();
  Expression exNot = ex.negate();
  Expression exNotNot = exNot.negate();
  
  std::vector<Expression> vecExpressions = {ex, exPlus, exPlusPlus, exPlusNot, exPlusPlusNot, exNot, exNotNot};
  std::vector<std::string> vecValidators = {"(test)", "(not test)", "(not test test2)", "test", "(not (not test test2))", "(not (test))", "(test)"};
  
  bool bOK = false;
  if(vecExpressions.size() == vecValidators.size()) {
    bOK = true;
    
    for(unsigned int unI = 0; unI < vecExpressions.size(); ++unI) {
      Expression exValidator = Expression::parseString(vecValidators[unI])[0];
      bool bResolved;
      vecExpressions[unI].resolve(exValidator, bResolved);
      
      if(!bResolved) {
	std::cout << "Invalid: " << vecExpressions[unI] << " vs. " << vecValidators[unI] << std::endl;
	bOK = false;
	break;
      }
    }
  }
  
  if(bOK) {
    nReturnvalue = EXIT_SUCCESS;
  }
  
  return nReturnvalue;
}
