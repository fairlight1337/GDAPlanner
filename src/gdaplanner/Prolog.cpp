#include <gdaplanner/Prolog.h>


namespace gdaplanner {
  Prolog::Prolog(World::Ptr wdWorld) : m_wdWorld(wdWorld) {
  }
  
  Prolog::~Prolog() {
  }
  
  World::Ptr Prolog::world() {
    return m_wdWorld;
  }
  
  Prolog::Solution::Ptr Prolog::query(std::string strExpression, Solution::Ptr solPrior, World::Ptr wdWorld) {
    return this->queryEx(Expression::parseSingle(strExpression), solPrior, wdWorld);
  }
  
  Prolog::Solution::Ptr Prolog::queryEx(Expression exQuery, Solution::Ptr solPrior, World::Ptr wdWorld) {
    Solution::Ptr solSolution = nullptr;
    
    if(exQuery.type() == Expression::List) {
      std::cout << "Query = " << exQuery << std::endl;
      
      if(!wdWorld) {
	wdWorld = m_wdWorld;
      }
      
      if(wdWorld) {
	solSolution = Solution::create();
	
	if(!this->populateSolution(exQuery, solSolution, solPrior)) {
	  solSolution = nullptr;
	}
      } else {
	throw std::exception();
      }
    }
    
    return solSolution;
  }
  
  bool Prolog::evaluateBuiltinFunction(Expression exQuery, Solution::Ptr solSolution, Solution::Ptr solPrior) {
    bool bReturnvalue = false;
    
    // Parametrize query beforehand
    exQuery = exQuery.parametrize(solSolution->bindings());
    
    if(exQuery.type() == Expression::List && exQuery.size() > 0) {
      std::string strPredicate = exQuery[0].get<std::string>();
      
      if(strPredicate == "member") {
	if(exQuery.size() == 3) {
	  if(exQuery[1].isVariable() && exQuery[2].type() == Expression::List && exQuery[2].size() > 0) {
	    // Case 1: (member ?a (1 2 3))
	    if(!solPrior) {
	      (*solSolution)[exQuery[1].get<std::string>()] = exQuery[2][0];
	      solSolution->setIndex(0, 0);
	      
	      bReturnvalue = true;
	    } else {
	      unsigned int unIndex = solPrior->index(0);
	      
	      if(unIndex < exQuery[2].size() - 1) {
		solSolution->setIndex(0, unIndex + 1);
		(*solSolution)[exQuery[1].get<std::string>()] = exQuery[2][unIndex + 1];
		
		bReturnvalue = true;
	      }
	    }
	  } else if(!(exQuery[1].isVariable() || exQuery[1].isWildcard()) && exQuery[2].type() == Expression::List) {
	    // Case 2: (member 1 (1 2 3))
	    Expression exex = exQuery[2];
	    std::vector<Expression> vecSub = exQuery[2].subExpressions();
	    
	    for(Expression exMember : vecSub) {
	      Expression exExpression = exQuery[1];
	      
	      if(exMember == exExpression) {
		if(!solPrior) {
		  solSolution->setIndex(0, 0);
		  bReturnvalue = true;
		  
		  break;
		}
	      }
	    }
	  }
	}
      } else if(strPredicate == "and") {
	if(exQuery.size() > 1) {
	  std::vector<Expression> vecSub = exQuery.subSequence(1);
	  
	  bReturnvalue = true;
	  unsigned int unIndex = 0;
	  for(Expression exAnded : vecSub) {
	    Solution::Ptr solSub = solSolution->pushSubSolution();
	    
	    if(!populateSolution(exAnded, solSub, (solPrior ? solPrior->subSolution(unIndex) : solPrior))) {
	      bReturnvalue = false;
	      break;
	    }
	    
	    unIndex++;
	  }
	}
      } else if(strPredicate == "=") {
	if(exQuery.size() == 3) {
	  if(!solPrior) {
	    if(exQuery[1].isVariable()) {
	      if(!exQuery[2].isVariable()) {
		(*solSolution)[exQuery[1].get<std::string>()] = exQuery[2];
		solSolution->setIndex(0, 0);
		
		bReturnvalue = true;
	      }
	    } else {
	      if(exQuery[2].isVariable()) {
		(*solSolution)[exQuery[2].get<std::string>()] = exQuery[1];
		solSolution->setIndex(0, 0);
		
		bReturnvalue = true;
	      } else {
		Expression exA = exQuery[1];
		Expression exB = exQuery[2];
		
		if(exA == exB) {
		  solSolution->setIndex(0, 0);
		  bReturnvalue = true;
		}
	      }
	    }
	  }
	}
      }
    }
    
    return bReturnvalue;
  }
  
  bool Prolog::populateSolution(Expression exQuery, Solution::Ptr solSolution, Solution::Ptr solPrior) {
    bool bReturnvalue = false;
    
    if(this->evaluateBuiltinFunction(exQuery, solSolution, solPrior)) {
      bReturnvalue = true;
    } else {
      switch(exQuery.type()) {
      case Expression::List: {
      } break;
	
	// ...
      }
    }
    
    return bReturnvalue;
  }
  
  /* Solution class */
  Prolog::Solution::Solution() {
  }
  
  Prolog::Solution::~Solution() {
  }
  
  bool Prolog::Solution::isBound(std::string strVariable) {
    return m_mapBindings.find(strVariable) != m_mapBindings.end();
  }
  
  bool Prolog::Solution::isBoundEx(Expression exVariable) {
    if(exVariable.isVariable()) {
      return this->isBound(exVariable.get<std::string>());
    }
    
    return false;
  }
  
  std::vector<std::string> Prolog::Solution::boundVariables() {
    std::vector<std::string> vecVariables;
    
    for(std::pair<std::string, Expression> prBound : m_mapBindings) {
      vecVariables.push_back(prBound.first);
    }
    
    return vecVariables;
  }
  
  std::map<std::string, Expression> Prolog::Solution::bindings(bool bRecursive) {
    if(bRecursive) {
      std::map<std::string, Expression> mapBindings = m_mapBindings;
      
      for(Solution::Ptr solSub : m_vecSubSolutions) {
	std::map<std::string, Expression> mapSubBindings = solSub->bindings(true);
	
	for(std::pair<std::string, Expression> prBinding : mapSubBindings) {
	  if(mapBindings.find(prBinding.first) == mapBindings.end()) {
	    mapBindings[prBinding.first] = prBinding.second;
	  }
	}
      }
      
      return mapBindings;
    } else {
      return m_mapBindings;
    }
  }
  
  Expression Prolog::Solution::value(std::string strVariable) {
    if(this->isBound(strVariable)) {
      return m_mapBindings[strVariable];
    }
    
    return Expression();
  }

  std::string Prolog::Solution::toString() {
    std::stringstream sts;
    
    std::map<std::string, Expression> mapBindings = this->bindings();
    
    if(mapBindings.size() > 0) {
      sts << "Bindings:" << std::endl;
      for(std::pair<std::string, Expression> prBound : mapBindings) {
	sts << " * " << prBound.first << " = " << prBound.second << std::endl;
      }
    } else {
      sts << "No bindings";
    }
    
    return sts.str();
  }
}
