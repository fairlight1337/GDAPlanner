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
      
      Solution solFlyingPrior;
      solFlyingPrior.setValid(false);
      
      while(dqSolutionStack.size() < exQueryBound.size()) {
	unsigned int unIndex = dqSolutionStack.size();
	Expression exOperand = exQueryBound[unIndex];
	
	Solution solThisPrior = solPrior.subSolution(unIndex);
	Solution solOperand = this->unify(exOperand,
					  solThisPrior,
					  (unIndex > 0 ? dqSolutionStack.back().finalBindings() : bdgBindings));
	
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
      
      while(solSolution.subSolutions().size() < exQueryBound.size()) {
	Solution solNew;
	solNew.index() = -1;
	
	solSolution.addSubSolution(solNew);
      }
      
      bool bOneFound = false;
      unsigned int unI;
      for(unI = 0; unI < solSolution.subSolutions().size(); ++unI) {
	if(solSolution.subSolution(unI).valid()) {
	  Solution solTemp = this->unify(exQueryBound[unI], solSolution.subSolution(unI), bdgBindings);
	  
	  if(solTemp.valid()) {
	    solSolution.subSolution(unI) = solTemp;
	    bOneFound = true;
	    break;
	  }
	}
      }
      
      if(!bOneFound) {
	solSolution.setValid(false);
	
	throw SolutionsExhausted();
      } else {
	solSolution.subSolutions().resize(unI + 1);
	solResult = solSolution;
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
      } else if(exQueryBound.match("(1- ?value ?newvalue)", mapResolution)) {
          Expression exValue = mapResolution["?value"];
          Expression exNewValue = mapResolution["?newvalue"];

          int intValue;
          int intNewValue;
          bool valueIsInt = true;
          bool newValueIsInt = true;

          try
          {
              intValue = std::stol(exValue.toString());
          }
          catch(const std::invalid_argument &e)
          {
              valueIsInt = false;
          }
          try
          {
              intNewValue = std::stol(exNewValue.toString());
          }
          catch(const std::invalid_argument &e)
          {
              newValueIsInt = false;
          }

          int nIndex = solPrior.index();

          std::map<std::string, Expression> mapR;

          solResult.index() = 0;

          if(0 <= nIndex)
              solResult.setValid(false);
          else if(valueIsInt && (intValue != 0))
          {
              mapR.insert(std::pair<std::string, Expression>(exNewValue.toString(), Expression(intValue - 1)));
              if(exNewValue.isVariable())
              {
                  solResult.setValid(true);
                  solResult.bindings() = Solution::Bindings(mapR);
              }
              else if(exNewValue.isWildcard())
                  solResult.setValid(true);
              else if(exNewValue == Expression::Integer)
                  solResult.setValid(exNewValue == Expression(intValue - 1));
          }
          else if((newValueIsInt))
          {
              mapR.insert(std::pair<std::string, Expression>(exValue.toString(), Expression(intNewValue + 1)));
              if(exValue.isWildcard())
                  solResult.setValid(true);
              else if(exValue.isVariable())
              {
                  solResult.bindings() = mapR;
                  solResult.setValid(true);
              }
          }
      } else if(exQueryBound.match("(append ?object ?list ?newlist)", mapResolution)) {
          Expression exObject = mapResolution["?object"];
          Expression exList = mapResolution["?list"];
          Expression exNewList = mapResolution["?newlist"];
          int nIndex = solPrior.index();

          solResult.index() = 0;
          if(0 <= nIndex)
              solResult.setValid(false);
          else if((!exObject.isVariable()) && (!exObject.isWildcard()))
          {
              if((exList == Expression::List) && (exNewList == Expression::List))
              {
                  int maxK = exList.size();
                  int maxJ = exNewList.size();
                  bool allEqual = ((maxK + 1) == maxJ);
                  for(int k = 0; allEqual && (k < maxK); k++)
                      allEqual = (exList[k] == exNewList[k]);
                  allEqual = (allEqual && (exNewList[maxJ - 1] == exObject));
                  solResult.setValid(allEqual);
              }
              else if(exList == Expression::List)
              {
                  if(exNewList.isVariable())
                  {
                      solResult.setValid(true);
                      std::map<std::string, Expression> mapR;
                      Expression exAppended = exList;
                      exAppended.add(exObject);
                      mapR.insert(std::pair<std::string, Expression>(exNewList.toString(), exAppended));
                      solResult.bindings() = mapR;
                  }
                  else if(exNewList.isWildcard())
                      solResult.setValid(true);
              }
              else if(exNewList == Expression::List)
              {
                  if(exList.isVariable())
                  {
                      int maxK = exNewList.size();
                      bool popped = (exObject == exNewList[maxK - 1]);
                      solResult.setValid(popped);
                      std::map<std::string, Expression> mapR;
                      Expression exPopped;
                      for(int k = 0; popped && (k < (maxK - 1)); k++)
                          exPopped.add(exNewList[k]);
                      mapR.insert(std::pair<std::string, Expression>(exList.toString(), exPopped));
                      if(popped)
                          solResult.bindings() = mapR;
                  }
                  else if(exList.isWildcard())
                      solResult.setValid(true);
              }
          }
          else if((exList == Expression::List) && (exNewList == Expression::List))
          {
              int maxK = exList.size();
              int maxJ = exNewList.size();
              std::map<std::string, Expression> mapR;
              bool allEqual = ((maxK + 1) == maxJ);
              for(int k = 0; allEqual && (k < maxK); k++)
                  allEqual = (exList[k] == exNewList[k]);
              if(maxJ)
                  mapR.insert(std::pair<std::string, Expression>(exObject.toString(), exNewList[maxJ - 1]));
              solResult.setValid(allEqual);
              if(allEqual && exObject.isVariable())
                  solResult.bindings() = mapR;
          }

      }
    }
    
    if(solResult.valid()) {
      solResult.setValid(solResult.bindings().superImpose(bdgBindings));
    }
    
    return solResult;
  }
}
