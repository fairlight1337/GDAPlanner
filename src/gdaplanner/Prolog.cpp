#include <gdaplanner/Prolog.h>


namespace gdaplanner {
  Prolog::Prolog(World::Ptr wdWorld) : m_wdWorld(wdWorld) {
    this->addDefaultLambdaPredicates();
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
      if(solPrior.index() == -1) {
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
	  
	    solResult = Solution();
	    solResult.index() = 0;
	  }
	} else {
	  std::cerr << "Error: No format string specified in '" << exQueryBound << "'." << std::endl;
	}
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
      } else if(exQueryBound.match("(holds ?a)", mapResolution)) {
	Expression exA = mapResolution["?a"];
	std::vector<std::map<std::string, Expression>> vecSolutions = m_wdWorld->holds(exA);
	
	unsigned int unIndex = solPrior.index() + 1;
	if(vecSolutions.size() > unIndex) {
	  solResult = Solution();
	  solResult.bindings() = Solution::Bindings(vecSolutions[unIndex]);
	  solResult.index() = unIndex;
	}
      } else {
	solResult = this->matchLambdaPredicates(exQueryBound, solPrior, bdgBindings);
      }
    }
    
    if(solResult.valid()) {
      solResult.setValid(solResult.bindings().superImpose(bdgBindings));
    }
    
    return solResult;
  }
  
  Solution Prolog::matchLambdaPredicates(Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) {
    Solution solResult;
    solResult.setValid(false);
    
    for(LambdaPredicate lpPredicate : m_vecLambdaPredicates) {
      Solution solTemp = lpPredicate(exQuery, solPrior, bdgBindings);
      
      if(solTemp.valid()) {
	solResult = solTemp;
	break;
      }
    }
    
    return solResult;
  }
  
  void Prolog::addLambdaPredicate(std::string strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda) {
    this->addLambdaPredicate(this->makeLambdaPredicate(strPredicate, fncLambda));
  }
  
  void Prolog::addSimpleLambdaPredicate(std::string strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda) {
    this->addLambdaPredicate(this->makeSimpleLambdaPredicate(strPredicate, fncLambda));
  }
  
  void Prolog::addLambdaPredicate(LambdaPredicate lpAdd) {
    m_vecLambdaPredicates.push_back(lpAdd);
  }
  
  void Prolog::addLazyListPredicate(std::string strPredicate, std::vector<Expression> vecList) {
    m_vecLambdaPredicates.push_back(this->makeLazyListPredicate(strPredicate, vecList));
  }
  
  void Prolog::addCallbackPredicate(std::string strPredicate, std::function<Expression(unsigned int)> fncLambda) {
    m_vecLambdaPredicates.push_back(this->makeCallbackPredicate(strPredicate, fncLambda));
  }
  
  Prolog::LambdaPredicate Prolog::makeCallbackPredicate(std::string strPredicate, std::function<Expression(unsigned int)> fncLambda) {
    return [strPredicate, fncLambda](Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) -> Solution {
      Solution solResult;
      solResult.setValid(false);
      
      std::map<std::string, Expression> mapBindings;
      
      if(exQuery.match(strPredicate, mapBindings)) {
	int nIndex = solPrior.index() + 1;
	
	if(exQuery == Expression::List && exQuery.size() == 2 && !exQuery[1].isBound()) {
	  try {
	    Expression exCallback = fncLambda(nIndex);
	    
	    solResult.setValid(true);
	    solResult.bindings()[exQuery[1].get<std::string>()] = exCallback;
	    solResult.index() = nIndex;
	  } catch(const SolutionsExhausted& seException) {}
	}
      }
      
      return solResult;
    };
  }
  
  Prolog::LambdaPredicate Prolog::makeLambdaPredicate(std::string strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda) {
    return [strPredicate, fncLambda](Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) -> Solution {
      Solution solResult;
      solResult.setValid(false);
      
      std::map<std::string, Expression> mapBindings;
      
      if(exQuery.match(strPredicate, mapBindings)) {
	if(solPrior.index() == -1) {
	  bool bResult = fncLambda(mapBindings);
	  
	  if(bResult) {
	    solResult = Solution();
	    solResult.index() = 0;
	  }
	}
      }
      
      return solResult;
    };
  }
  
  Prolog::LambdaPredicate Prolog::makeSimpleLambdaPredicate(std::string strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda) {
    return this->makeLambdaPredicate(strPredicate, [fncLambda](std::map<std::string, Expression> mapBindings) -> bool { fncLambda(mapBindings); return true; });
  }
  
  Prolog::LambdaPredicate Prolog::makeLazyListPredicate(std::string strPredicate, std::vector<Expression> vecList) {
    return [strPredicate, vecList](Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) -> Solution {
      Solution solResult;
      solResult.setValid(false);
      
      std::map<std::string, Expression> mapBindings;
      
      if(exQuery == Expression::List && exQuery.size() == 2 && exQuery[0] == Expression::String) {
	if(exQuery.match(strPredicate, mapBindings)) {
	  unsigned int unIndex = solPrior.index() + 1;
	  
	  if(exQuery[1].isBound()) { // We're looking for the element in the list
	    if(unIndex == 0) {
	      for(Expression exCheck : vecList) {
		if(exCheck == exQuery[1]) {
		  solResult.setValid(true);
		  solResult.index() = unIndex;
		  
		  break;
		}
	      }
	    }
	  } else { // We're returning the contents of the list
	    if(vecList.size() > unIndex) {
	      solResult.setValid(true);
	      solResult.index() = unIndex;
	      solResult.bindings()[exQuery[1].get<std::string>()] = vecList[unIndex];
	    }
	  }
	}
      }
      
      return solResult;
    };
  }
  
  void Prolog::addDefaultLambdaPredicates() {
    this->addSimpleLambdaPredicate("(print-world)", [this](std::map<std::string, Expression> mapBindings) {
	std::cout << *m_wdWorld << std::endl;
      });
    
    this->addLambdaPredicate("(assert ?a)", [this](std::map<std::string, Expression> mapBindings) {
	  Expression exA = mapBindings["?a"];
	  
	  if(exA.isBound()) {
	    m_wdWorld->assertFact(exA);
	    
	    return true;
	  } else {
	    return false;
	  }
      });
    
    this->addLambdaPredicate("(retract ?a)", [this](std::map<std::string, Expression> mapBindings) {
	  Expression exA = mapBindings["?a"];
	  
	  if(exA.isBound()) {
	    m_wdWorld->retractFact(exA);
	    
	    return true;
	  } else {
	    return false;
	  }
      });
    
    this->addLambdaPredicate("(bound ?a)", [](std::map<std::string, Expression> mapBindings) {
	return mapBindings["?a"].isBound();
      });
    
    this->addLambdaPredicate([](Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) -> Solution {
	Solution solResult;
	solResult.setValid(false);
	
	std::map<std::string, Expression> mapBindings;
	
	if(exQuery.match("(length ?a ?b)", mapBindings)) {
	  if(solPrior.index() == -1) {
	    Expression exA = mapBindings["?a"];
	    Expression exB = mapBindings["?b"];
	    
	    if(exA.isBound() && exA == Expression::List) { // This is a must
	      unsigned int unLength = exA.size();
	      
	      if(exB.isBound()) { // We're checking the length
		bool bTransformed;
		unsigned int unTransformed = exB.transformTo<unsigned int>(bTransformed);
		
		if(bTransformed) {
		  if(unLength == unTransformed) {
		    solResult = Solution();
		    solResult.index() = 0;
		  }
		}
	      } else { // We're getting the length
		solResult = Solution();
		solResult.index() = 0;
		solResult.bindings()[exB.get<std::string>()] = Expression(unLength);
	      }
	    }
	  }
	}
	
	return solResult;
      });
    
    this->addLambdaPredicate([this](Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) -> Solution {
	Solution solResult;
	solResult.setValid(false);
	
	std::map<std::string, Expression> mapBindings;
	
	if(exQuery.match("(foreach ?a ?b ?c)", mapBindings)) {
	  if(solPrior.index() == -1) {
	    Expression exA = mapBindings["?a"];
	    Expression exB = mapBindings["?b"];
	    Expression exC = mapBindings["?c"];
	    
	    Expression exD; // Final container
	    
	    if(!exA.isBound() && exB == Expression::List && !exC.isBound()) {
	      Solution solTemp;
	      std::string strVariable = exA.get<std::string>();
	      
	      // Run through solutions for ?b
	      bool bAnyGood = false;
	      while(solTemp.valid()) {
		solTemp = this->unify(exB, solTemp, bdgBindings);
		
		if(solTemp.valid()) {
		  bAnyGood = true;
		  exD.add(solTemp.bindings()[strVariable]);
		}
	      }
	      
	      if(bAnyGood) {
		solResult.setValid(true);
		solResult.index() = 0;
		solResult.bindings()[exC.get<std::string>()] = exD;
	      }
	    }
	  }
	}
	
	return solResult;
      });
    
    // std::vector<Expression> vecList = {Expression("some-1"), Expression("some-2"), Expression(1), Expression(2), Expression(3)};
    // this->addLazyListPredicate("(init-predicates ?a)", vecList);
  }
}
