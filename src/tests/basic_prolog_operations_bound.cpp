#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;

typedef std::map<std::string, std::vector<std::map<std::string, Expression>>> Collection;


bool satisfiesRequirements(std::map<std::string, Expression> mapSolution, std::map<std::string, Expression> mapRequirements) {
  for(std::pair<std::string, Expression> prRequired : mapRequirements) {
    bool bFound = false;
    
    if(mapSolution.find(prRequired.first) != mapSolution.end() &&
       mapSolution[prRequired.first] == prRequired.second) {
      bFound = true;
    }
    
    if(!bFound) {
      return false;
    }
  }
  
  return true;
}


bool anySolutionSatisfiesRequirements(std::vector<std::map<std::string, Expression>> vecSolutions, std::map<std::string, Expression> mapRequirements) {
  bool bOneFound = false;
  
  for(std::map<std::string, Expression> mapSolution : vecSolutions) {
    if(satisfiesRequirements(mapSolution, mapRequirements)) {
      bOneFound = true;
      break;
    }
  }
  
  return bOneFound;
}


bool checkQueries(Collection colData) {
  for(std::pair<std::string, std::vector<std::map<std::string, Expression>>> prQuery : colData) {
    World::Ptr wdWorld = World::create();
    Prolog::Ptr plProlog = Prolog::create(wdWorld);
    
    Solution solSolution;
    
    std::vector<std::map<std::string, Expression>> vecAllSolutionBindings;
    
    // Generate them
    while(true) {
      solSolution = plProlog->query(prQuery.first, solSolution);
      
      if(solSolution.valid()) {
	vecAllSolutionBindings.push_back(solSolution.finalBindings());
      } else {
	break;
      }
    }
    
    // Check them
    bool bAllOK = true;
    for(std::map<std::string, Expression> mapRequiredBindings : prQuery.second) {
      std::cout << prQuery.first << std::endl;
      if(!anySolutionSatisfiesRequirements(vecAllSolutionBindings, mapRequiredBindings)) {
	bAllOK = false;
	std::cout << "Not OK" << std::endl;
	break;
      } else {
	std::cout << "OK" << std::endl;
      }
    }
    
    if(!bAllOK) {
      return false;
    }
  }
  
  return true;
}


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  Collection colData = {{"(and (= ?a 1) (bound ?a))", {}},
			{"(not (bound ?a))", {}}};
  
  if(checkQueries(colData)) {
    nReturnvalue = EXIT_SUCCESS;
  }
  
  return nReturnvalue;
}
