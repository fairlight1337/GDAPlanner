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
      } else if(exQueryBound.match("(bound ?a)", mapResolution)) {
            if(solPrior.index() == -1) {
              Expression exA = mapResolution["?a"];

              if(exA.isBound()) {
                solResult = Solution();
                solResult.index() = 0;
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
      } else if(exQueryBound.match("(holds ?goal ?state)", mapResolution)) {
          Expression exGoal = mapResolution["?goal"];
          Expression exState = mapResolution["?state"];
          std::vector<Expression> exGoalAuxVec; exGoalAuxVec.clear(); exGoalAuxVec.push_back(exGoal);
          std::vector<Expression> exStateAuxVec; exStateAuxVec.clear(); exStateAuxVec.push_back(exState);

          if((solPrior.index() == -1) && (exGoal.isBound()) && (exState.isBound()))
          {
              bool allMatch = true;
              std::map<std::string, Expression> bindings; bindings.clear();
              if(exGoal.type() != Expression::List)
                  exGoal = Expression(exGoalAuxVec);
              if(exState.type() != Expression::List)
                  exState = Expression(exStateAuxVec);

              int maxG = exGoal.subExpressions().size();
              int maxS = exState.subExpressions().size();
              for(int g = 0; allMatch && (g < maxG); g++)
              {
                  bool found = false;
                  for(int s = 0; (!found) && (s < maxS); s++)
                  {
                      std::map<std::string, Expression> newBindings; newBindings.clear();
                      newBindings = exGoal.subExpressions()[g].resolve(exState.subExpressions()[s], found);
                      for(int k = 0; found && (k < maxG); k++)
                      {
                          exGoal.subExpressions()[k] = exGoal.subExpressions()[k].parametrize(newBindings);
                      }
                      for(std::map<std::string, Expression>::const_iterator it = newBindings.begin();
                          it != newBindings.end(); it++)
                          bindings.insert(std::pair<std::string, Expression>(it->first, it->second));
                  }
                  allMatch = found;
              }
              if(allMatch)
              {
                  solResult = Solution();
                  /* Should the next line be allowed through?*/
                  //solResult.bindings() = bindings;
                  solResult.index() = 0;
              }
          }
      } else if(exQueryBound.match("(apply ?effects ?state ?newstate)", mapResolution)) {
          Expression exEffects = mapResolution["?effects"];
          Expression exState = mapResolution["?state"];
          Expression exNewState = mapResolution["?newstate"];
          std::vector<Expression> exEffectsAuxVec; exEffectsAuxVec.clear(); exEffectsAuxVec.push_back(exEffects);
          std::vector<Expression> exStateAuxVec; exStateAuxVec.clear(); exStateAuxVec.push_back(exState);

          if((solPrior.index() == -1) && (exEffects.isBound()) && (exState.isBound())
                  && (!exNewState.isBound()))
          {
              if(exNewState.isWildcard())
              {
                  solResult = Solution();
                  solResult.index() = 0;
              }
              else
              {
                  std::map<std::string, Expression> bindings; bindings.clear();

                  if(exEffects.type() != Expression::List)
                      exEffects = Expression(exEffectsAuxVec);
                  if(exState.type() != Expression::List)
                      exState = Expression(exStateAuxVec);

                  int maxE = exEffects.size();
                  for(int e = 0; e < maxE; e++)
                  {
                      Expression crEffect = exEffects.subExpressions()[e];
                      Expression crNegEffect = crEffect.negate();
                      int maxS = exState.subExpressions().size();
                      bool found = false;
                      for(int s = 0; (!found) && (s < maxS); s++)
                      {
                          std::map<std::string, Expression> bindings; bindings.clear();
                          bindings = crEffect.resolve(exState.subExpressions()[s], found);
                          if(found)
                              for(int k = 0; k < maxE; k++)
                                  exEffects.subExpressions()[k] = exEffects.subExpressions()[k].parametrize(bindings);
                          else
                          {
                              bindings.clear();
                              bool negated = false;
                              bindings = crNegEffect.resolve(exState.subExpressions()[s], negated);
                              if(negated)
                              {
                                  crEffect = crEffect.parametrize(bindings);
                                  crNegEffect = crNegEffect.parametrize(bindings);
                                  exState.subExpressions()[s] = Expression("");
                                  for(int k = 0; k < maxE; k++)
                                      exEffects.subExpressions()[k] = exEffects.subExpressions()[k].parametrize(bindings);
                              }
                          }
                      }
                      if(!found)
                          exState.subExpressions().push_back(crEffect);
                  }

                  Expression exNewStateValue("");
                  maxE = exState.subExpressions().size();
                  for(int e = 0; e < maxE; e++)
                      if(exState.subExpressions()[e].toString() != "")
                          exNewStateValue.subExpressions().push_back(exState.subExpressions()[e]);
                  bindings.insert(std::pair<std::string, Expression>(exNewState.toString(), exNewStateValue));

                  solResult = Solution();
                  solResult.index() = 0;
                  solResult.bindings() = bindings;
              }
          }
      } else if(exQueryBound.match("(print-world)", mapResolution)) {
	if(solPrior.index() == -1) {
	  std::cout << *m_wdWorld << std::endl;
	  
	  solResult = Solution();
	  solResult.index() = 0;
	}
      }
    }
    
    if(solResult.valid()) {
      solResult.setValid(solResult.bindings().superImpose(bdgBindings));
    }
    
    return solResult;
  }
}
