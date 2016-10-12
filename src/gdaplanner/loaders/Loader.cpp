#include <gdaplanner/loaders/Loader.h>
#include <iostream>


namespace gdaplanner {
  namespace loaders {
    Loader::Loader() {
    }
    
    Loader::~Loader() {
    }
    
    contexts::Context::Ptr Loader::readContextFile(std::string strFilepath) {
      std::ifstream ifFile(strFilepath, std::ios::in);
      
      if(ifFile.good()) {
	std::string strContents((std::istreambuf_iterator<char>(ifFile)),
				std::istreambuf_iterator<char>());
	
      	try {
	  std::vector<Expression> vecExpressions = Expression::parseString(strContents);
	  contexts::Context::Ptr ctxContext = this->makeContext();
	  
	  bool bAllGood = true;
	  for(Expression exExpression : vecExpressions) {
	    if(!this->processExpression(exExpression, ctxContext)) {
	      bAllGood = false;
	      
	      break;
	    }
	  }
	  
	  return (bAllGood ? ctxContext : nullptr);
	} catch(std::exception& exCaught) {
	  return nullptr;
	}
      }
      
      return nullptr;
    }
    
    problems::Problem::Ptr Loader::readProblemFile(std::string strFilepath) {
      std::ifstream ifFile(strFilepath, std::ios::in);
      
      if(ifFile.good()) {
	std::string strContents((std::istreambuf_iterator<char>(ifFile)),
				std::istreambuf_iterator<char>());
	
      	try {
	  std::vector<Expression> vecExpressions = Expression::parseString(strContents);
	  problems::Problem::Ptr prbProblem = this->makeProblem();
	  
	  bool bAllGood = true;
	  for(Expression exExpression : vecExpressions) {
	    if(!this->processExpression(exExpression, prbProblem)) {
	      bAllGood = false;
	      break;
	    }
	  }
	  
	  return (bAllGood ? prbProblem : nullptr);
	} catch(std::exception& exCaught) {
	  return nullptr;
	}
      }
      
      return nullptr;
    }
  }
}
