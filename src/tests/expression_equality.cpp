#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


GDAPlanner::Ptr g_gdapPlanner = nullptr;


bool testEqualityEx(Expression exA, Expression exB, unsigned int unExpectedResolutions) {
  bool bResolved;
  std::map<std::string, Expression> mapResolution = exA.resolve(exB, bResolved);
  
  return bResolved && (mapResolution.size() == unExpectedResolutions);
}


bool testEquality(std::string strA, std::string strB, unsigned int unExpectedResolutions) {
  return testEqualityEx(Expression(Expression::parseString(strA)),
			Expression(Expression::parseString(strB)),
			unExpectedResolutions);
}


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  g_gdapPlanner = GDAPlanner::create();
  g_gdapPlanner->pushContext<contexts::PDDL>("test");
  
  if(// Positive
     testEquality("(?a)", "(A)", 1) &&
     testEquality("(?a ?b)", "(A B)", 2) &&
     testEquality("(?a B)", "(A ?b)", 2) &&
     testEquality("(A B)", "(A B)", 0) &&
     // Negative
     !testEquality("(A)", "(B)", 0) &&
     !testEquality("(?a)", "(?a ?b)", 0) &&
     !testEquality("(?a ?b)", "(?a)", 0) &&
     !testEquality("(table-set)", "(A)", 0) &&
     // Wildcard
     testEquality("(?_)", "(A)", 0) &&
     testEquality("(?a ?_)", "(A)", 1) &&
     testEquality("(?a ?_)", "(A B)", 1) &&
     testEquality("(?a ?b C Intermediate D ?_ E)", "(A B C ?_)", 2) &&
     // Multiple occurrences
     testEquality("(A A B)", "(A A B)", 0) &&
     testEquality("(?a A)", "(A A)", 1) &&
     testEquality("(A A)", "(?_ A)", 0)) {
    nReturnvalue = EXIT_SUCCESS;
  }
  
  return nReturnvalue;
}
