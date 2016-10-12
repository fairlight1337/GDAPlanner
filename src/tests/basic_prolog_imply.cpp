#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  World::Ptr wdWorld = World::create();
  Prolog plProlog(wdWorld);
  
  Solution solTemp = plProlog.query("(and (= ?a 1) (once (or (imply (> ?a 0) (= ?b a)) (= ?b b))))");
  
  if(solTemp.valid()) {
    std::map<std::string, Expression> mapBindings = solTemp.finalBindings();
    
    if(mapBindings["?a"] == 1 && mapBindings["?b"] == "a") {
      nReturnvalue = EXIT_SUCCESS;
    }
  }
  
  return nReturnvalue;
}
