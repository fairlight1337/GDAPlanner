#include <gdaplanner/Prolog.h>


namespace gdaplanner {
  Prolog::Prolog(World::Ptr wdWorld) : m_wdWorld(wdWorld) {
  }
  
  Prolog::~Prolog() {
  }
  
  World::Ptr Prolog::world() {
    return m_wdWorld;
  }
  
  Solution Prolog::query(std::string strExpression, Solution solPrior, World::Ptr wdWorld) {
    return this->queryEx(Expression::parseSingle(strExpression), solPrior, wdWorld);
  }
  
  Solution Prolog::queryEx(Expression exQuery, Solution solPrior, World::Ptr wdWorld) {
    Solution solSolution;
    solSolution.setValid(false);
    
    if(exQuery.type() == Expression::List) {
      std::cout << std::endl << " ==== Query: \033[1;33m" << exQuery << "\033[0m ====" << std::endl;
      
      if(!wdWorld) {
	wdWorld = m_wdWorld;
      }
      
      if(wdWorld && solPrior.valid()) {
	try {
	  solSolution = this->unify(exQuery, solPrior);
	} catch(SolutionsExhausted seException) {}
      } else {
	throw std::exception();
      }
    }
    
    return solSolution;
  }

  Solution Prolog::unify(Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) {
    Solution solResult;
    solResult.setValid(false);
    
    Expression exQueryBound = exQuery.parametrize(bdgBindings.bindings());
    
    if(exQueryBound.predicateName() == "and") {
      std::deque<Solution> dqSolutionStack;
      
      while(dqSolutionStack.size() < exQueryBound.size() - 1) {
	std::cout << exQueryBound << std::endl;
	unsigned int unIndex = dqSolutionStack.size();
	
	Expression exOperand = exQueryBound[unIndex + 1]; // Leap jump the predicate name
	std::cout << exOperand << std::endl;
	
	Solution solOperand = this->unify(exOperand,
					  solPrior.subSolution(unIndex),
					  (unIndex > 0 ? dqSolutionStack.back().bindings().bindings() : bdgBindings));
	
	
      }
    } else {
      std::map<std::string, Expression> mapResolution;
      
      if(exQueryBound.match("(= ?a ?b)", mapResolution)) {
	Expression exA = mapResolution["?a"];
	Expression exB = mapResolution["?b"];
	
	if(solPrior.index() == -1) {
	  std::map<std::string, Expression> mapR;
	  if(exA.matchEx(exB, mapR)) {
	    solResult = Solution();
	    
	    solResult.bindings() = Solution::Bindings(mapR);
	    solResult.index() = 0;
	  }
	}
      } else if(exQueryBound.match("(member ?member ?list)", mapResolution)) {
	Expression exMember = mapResolution["?member"];
	Expression exList = mapResolution["?list"];
	
	if(exList == Expression::List) {
	  unsigned int unIndex = solPrior.index();
	  
	  if(unIndex + 1 < exList.size()) {
	    unIndex++;
	    std::map<std::string, Expression> mapR;
	    
	    if(exMember.matchEx(exList[unIndex], mapR)) {
	      solResult = Solution();
	      solResult.bindings() = Solution::Bindings(mapR);
	      solResult.index() = unIndex;
	    }
	  }
	}
      }
    }
    
    return solResult;
  }
}
