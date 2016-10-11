#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(int argc, char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  World::Ptr wdWorld = World::create();
  Prolog plProlog(wdWorld);
  
  plProlog.query("(and (= ?a 3) (assert (some-fact ?a)))");
  if(plProlog.query("(holds (some-fact 3))").valid()) {
    plProlog.query("(retract (some-fact 3))");
    
    if(plProlog.query("(not (holds (some-fact 3)))").valid()) {
      nReturnvalue = EXIT_SUCCESS;
    }
  }
  
  return nReturnvalue;
}
