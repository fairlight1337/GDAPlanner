#include <iostream>
#include <cstdlib>
#include <string>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  World::Ptr wdWorld = World::create();
  
  Expression exA = Expression::parseSingle("(table-set table-1");
  
  if(wdWorld->holds(exA).size() == 0) {
    if(wdWorld->assertFact(exA)) {
      if(wdWorld->holds(exA).size() == 1) {
	wdWorld->retractFact(exA);
	
	if(wdWorld->holds(exA).size() == 0) {
	  Expression exB = Expression::parseSingle("(table-set table-2");
	  Expression exC = Expression::parseSingle("(table-set ?a)");
	  
	  if(wdWorld->assertFact(exA) && wdWorld->assertFact(exB)) {
	    std::vector<std::map<std::string, Expression>> vecHolds = wdWorld->holds(exC);
	    
	    if(vecHolds.size() > 0) {
	      bool bA = false, bB = false;
	      
	      for(std::map<std::string, Expression> mapHolds : vecHolds) {
		if(mapHolds["?a"] == "table-1") {
		  bA = true;
		} else if(mapHolds["?a"] == "table-2") {
		  bB = true;
		}
	      }
	      
	      if(bA && bB) {
		wdWorld->retractFact(exA);
		
		if(wdWorld->holds(exC).size() == 1) {
		  wdWorld->retractFact(exC);
		  
		  if(wdWorld->holds(exC).size() == 0) {
		    if(wdWorld->assertFact(exA) && wdWorld->assertFact(exB)) {
		      wdWorld->retractFact(exC, 0, true);
		      
		      if(wdWorld->holds(exC).size() == 2) {
			nReturnvalue = EXIT_SUCCESS;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
  
  return nReturnvalue;
}
