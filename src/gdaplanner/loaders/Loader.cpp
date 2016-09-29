#include <gdaplanner/loaders/Loader.h>


namespace gdaplanner {
  namespace loaders {
    Loader::Loader() {
    }
    
    Loader::~Loader() {
    }
    
    Context::Ptr Loader::readFile(std::string strFilepath) {
      std::ifstream ifFile(strFilepath, std::ios::in);
      
      if(ifFile.good()) {
	std::string strContents((std::istreambuf_iterator<char>(ifFile)),
				std::istreambuf_iterator<char>());
	
      	try {
	  std::vector<Expression> vecExpressions = Expression::parseString(strContents);
	  Context::Ptr ctxContext = this->makeContext();
	  
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
  }
}
