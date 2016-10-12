#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

#include <gdaplanner/GDAPlanner.h>


using namespace gdaplanner;


bool exists(std::string strFile) {
  std::ifstream ifFile(strFile, std::ios::in);
  
  return ifFile.good();
}


int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv) {
  int nReturnvalue = EXIT_FAILURE;
  
  std::vector<std::string> vecTestDataDirectories = {"elevator", "openstacks", "parcprinter", "pegsol", "scanalyzer"};
  
  bool bOK = true;
  for(std::string strDirectory : vecTestDataDirectories) {
    std::string strFullDir = "../data/test/" + strDirectory;
    
    unsigned int unIndex = 1;
    
    while(true) {
      std::stringstream sts;
      sts << strFullDir << "/p" << (unIndex < 10 ? "0" : "") << unIndex;
      
      std::string strDomain = sts.str() + "-domain.pddl";
      std::string strProblem = sts.str() + ".pddl";
      
      if(exists(strDomain) && exists(strProblem)) {
	GDAPlanner gdapPlanner;
	
	if(gdapPlanner.readContextFile<loaders::PDDL>(strDomain)) {
	  contexts::Context::Ptr ctxContext = gdapPlanner.currentContext();
	  
	  if(ctxContext) {
	    problems::Problem::Ptr prbProblem = gdapPlanner.readProblemFile<loaders::PDDL>(strProblem);
	    
	    if(prbProblem) {
	      unIndex++;
	    } else {
	      bOK = false;
	      break;
	    }
	  } else {
	    bOK = false;
	    break;
	  }
	} else {
	  bOK = false;
	  break;
	}
      } else {
	break;
      }
    }
  }
  
  if(bOK) {
    nReturnvalue = EXIT_SUCCESS;
  }
  
  return nReturnvalue;
}
