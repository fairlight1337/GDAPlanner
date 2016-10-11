#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


bool processQuery(std::string strQuery) {
  World::Ptr wdWorld = World::create();
  Prolog::Ptr plProlog = Prolog::create(wdWorld);
  plProlog->loadStandardLibrary();
  
  Solution solSolution;
  int nCount = 0;
  
  while(solSolution.valid()) {
    solSolution = plProlog->query(strQuery, solSolution);
    
    if(solSolution.valid()) {
      nCount++;
      std::cout << "---" << std::endl;
      
      std::map<std::string, Expression> mapBindings = solSolution.finalBindings();
      for(std::pair<std::string, Expression> prPair : mapBindings) {
	std::cout << " * " << prPair.first << " = " << prPair.second << std::endl;
      }
    }
  }
  
  return nCount > 0;
}


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  if(argc > 1) {
    std::string strQuery = argv[1];
    nReturnvalue = (processQuery(strQuery) ? EXIT_SUCCESS : EXIT_FAILURE);
    
    if(nReturnvalue == EXIT_FAILURE) {
      std::cerr << "No solutions" << std::endl;
    }
  } else {
    std::cerr << "Usage: " << argv[0] << " <query-string>" << std::endl;
  }
  
  return nReturnvalue;
}
