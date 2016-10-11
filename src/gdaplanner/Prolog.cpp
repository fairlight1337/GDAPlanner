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
      exQueryBound.popFront();
      std::deque<Solution> dqSolutionStack;
      
      if(solPrior.index() > -1) {
	for(Solution solSub : solPrior.subSolutions()) {
	  dqSolutionStack.push_back(solSub);
	}
	
	if(dqSolutionStack.size() > 0) {
	  dqSolutionStack.pop_back();
	}
      }
      
      while(dqSolutionStack.size() < exQueryBound.size()) {
	unsigned int unIndex = dqSolutionStack.size();
	Expression exOperand = exQueryBound[unIndex];
	Solution solOperand;
	
	try {
	  solOperand = this->unify(exOperand,
				   solPrior.subSolution(unIndex),
				   (unIndex > 0 ? dqSolutionStack.back().finalBindings() : bdgBindings));
	} catch(SolutionsExhausted seException) {
	  solOperand.setValid(false);
	}
	
	solPrior.subSolution(unIndex) = solOperand;
	
	if(solOperand.valid()) {
	  dqSolutionStack.push_back(solOperand);
	} else {
	  if(dqSolutionStack.size() > 0) {
	    dqSolutionStack.pop_back();
	    
	    for(unsigned int unI = unIndex; unI < solPrior.subSolutions().size(); ++unI) {
	      solPrior.subSolution(unI).resetIndices();
	    }
	  } else {
	    throw SolutionsExhausted();
	  }
	}
      }
      
      solResult = Solution();
      solResult.index() = solPrior.index() + 1;
      
      for(Solution solSub : dqSolutionStack) {
	solResult.addSubSolution(solSub);
      }
    } else if(exQueryBound.predicateName() == "or") {
      exQueryBound.popFront();
      Solution solSolution = solPrior;
      solSolution.index()++;
      
      bool bOneFound = false;
      unsigned int unI;
      
      for(unI = 0; unI < exQueryBound.size(); ++unI) {
	if(solSolution.subSolutions().size() <= unI) {
	  Solution solNew;
	  solNew.index() = -1;
	  
	  solSolution.addSubSolution(solNew);
	}
	
	if(solSolution.subSolution(unI).valid()) {
	  Solution solTemp = this->unify(exQueryBound[unI], solSolution.subSolution(unI), bdgBindings);
	  
	  if(solTemp.valid()) {
	    solSolution.setValid(true);
	    solSolution.subSolution(unI) = solTemp;
	    
	    bOneFound = true;
	    break;
	  } else {
	    solSolution.subSolution(unI).setValid(false);
	  }
	}
      }
      
      if(bOneFound) {
	solResult = solSolution;
      } else {
	solSolution.setValid(false);
      }
    } else if(exQueryBound.predicateName() == "format") {
      Expression exFormat = exQueryBound;
      exFormat.popFront();
      exFormat = exFormat.parametrize(bdgBindings.bindings());
      
      if(exFormat.size() > 0) {
	std::string strFormat = exFormat[0].get<std::string>();
	exFormat.popFront();
	std::stringstream stsOut;
        
	bool bAllOK = true;
	
	size_t szSpecifier, szSpecifierOld = -2;
	while(szSpecifier != std::string::npos) {
	  szSpecifier = strFormat.find("~", szSpecifier + 1);
	  
	  if(szSpecifier != std::string::npos) {
	    if(szSpecifier != szSpecifierOld) {
	      stsOut << strFormat.substr(szSpecifierOld + 2, szSpecifier - szSpecifierOld - 2);
	      
	      szSpecifierOld = szSpecifier;
	    }
	    
	    if(szSpecifier < strFormat.length() - 1) {
	      char cType = strFormat[szSpecifier + 1];
	      
	      switch(cType) {
	      case 'a': {
		if(exFormat.size() > 0) {
		  if(exFormat[0] == Expression::String) {
		    std::string strContent = exFormat[0].get<std::string>();
		    stsOut << strContent;
		  } else {
		    stsOut << exFormat[0];
		  }
		  
		  exFormat.popFront();
		} else {
		  std::cerr << "Error: Too manh format specifiers for the arguments given in '" << exQueryBound << "'." << std::endl;
		  bAllOK = false;
		}
	      } break;
		
	      case '%': {
		stsOut << std::endl;
	      } break;
		
	      default: {
		std::cerr << "Error: Invalid format specifier '~" << cType << "' in '" << exQueryBound << "'." << std::endl;
		bAllOK = false;
	      } break;
	      }
	    } else {
	      std::cerr << "Error: Format string end with typeless specifier in '" << exQueryBound << "'." << std::endl;
	      bAllOK = false;
	    }
	  }
	}
	
	if(bAllOK) {
	  if(szSpecifierOld + 2 < strFormat.size()) {
	    stsOut << strFormat.substr(szSpecifierOld + 2);
	  }
	  
	  std::cout << stsOut.str();
	}
      } else {
	std::cerr << "Error: No format string specified in '" << exQueryBound << "'." << std::endl;
      }
    } else {
      std::map<std::string, Expression> mapResolution;
      
      Expression exQuerySanitized = exQueryBound.sanitize("_");
      if(exQuerySanitized.match("(= ?a ?b)", mapResolution)) {
	Expression exA = mapResolution["?a"];
	Expression exB = mapResolution["?b"];
	
	if(solPrior.index() == -1) {
	  std::map<std::string, Expression> mapR;
	  
	  if(exA.matchEx(exB, mapR)) {
	    solResult = Solution();
	    
	    solResult.bindings() = Solution::Bindings(mapR).desanitize("_");
	    solResult.index() = 0;
	  }
	}
      } else if(exQueryBound.match("(member ?member ?list)", mapResolution)) {
	Expression exMember = mapResolution["?member"];
	Expression exList = mapResolution["?list"];
	
	if(exList == Expression::List) {
	  int nIndex = solPrior.index();
	  
	  while(nIndex + 1 < exList.size()) {
	    nIndex++;
	    std::map<std::string, Expression> mapR;
	    
	    if(exMember.matchEx(exList[nIndex], mapR)) {
	      solResult = Solution();
	      solResult.bindings() = Solution::Bindings(mapR);
	      solResult.index() = nIndex;
	      
	      break;
	    }
	  }
	}
      } else if(exQueryBound.match("(not ?a)", mapResolution)) {
	if(solPrior.index() == -1) {
	  Expression exA = mapResolution["?a"];
	  Solution solTemp;
	  
	  try {
	    solTemp = this->unify(exA, solPrior, bdgBindings);
	  } catch(SolutionsExhausted seException) {
	    solTemp.setValid(false);
	  }
	  
	  if(!solTemp.valid()) {
	    solResult = Solution();
	    solResult.index() = 0;
	  }
	}
      } else if(exQueryBound.match("(bound ?a)", mapResolution)) {
	if(solPrior.index() == -1) {
	  Expression exA = mapResolution["?a"];
	  
	  if(exA.isBound()) {
	    solResult = Solution();
	    solResult.index() = 0;
	  }
	}
      } else if(exQueryBound.match("(assert ?a)", mapResolution)) {
	if(solPrior.index() == -1) {
	  Expression exA = mapResolution["?a"];
	  
	  if(exA.isBound()) {
	    m_wdWorld->assertFact(exA);
	    
	    solResult = Solution();
	    solResult.index() = 0;
	  }
	}
      } else if(exQueryBound.match("(retract ?a)", mapResolution)) {
	if(solPrior.index() == -1) {
	  Expression exA = mapResolution["?a"];
	  
	  if(exA.isBound()) {
	    m_wdWorld->retractFact(exA);
	    
	    solResult = Solution();
	    solResult.index() = 0;
	  }
	}
      } else if(exQueryBound.match("(holds ?a)", mapResolution)) {
	Expression exA = mapResolution["?a"];
	std::vector<std::map<std::string, Expression>> vecSolutions = m_wdWorld->holds(exA);
	
	unsigned int unIndex = solPrior.index() + 1;
	if(vecSolutions.size() > unIndex) {
	  solResult = Solution();
	  solResult.bindings() = Solution::Bindings(vecSolutions[unIndex]);
	  solResult.index() = unIndex;
	}
      }
    }
    
    if(solResult.valid()) {
      solResult.setValid(solResult.bindings().superImpose(bdgBindings));
    }
    
    return solResult;
  }
}
