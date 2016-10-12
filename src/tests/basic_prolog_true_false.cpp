#include <iostream>
#include <cstdlib>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  World::Ptr wdWorld = World::create();
  Prolog plProlog(wdWorld);
  
  if(plProlog.query("(false)").valid() == false) {
    if(plProlog.query("(true)").valid() == true) {
      nReturnvalue = EXIT_SUCCESS;
    }
  }
  
  return nReturnvalue;
}
