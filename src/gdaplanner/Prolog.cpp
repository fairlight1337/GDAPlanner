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
  
  Solution Prolog::query(std::string const& strExpression, Solution const& solPrior, World::Ptr wdWorld) {
    return this->queryEx(Expression::parseSingle(strExpression), solPrior, wdWorld);
  }
  
  Solution Prolog::queryEx(Expression const& exQuery, Solution const& solPrior, World::Ptr wdWorld) {
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
  
  Solution Prolog::unify(Expression const& exQuery, Solution solPrior, Solution::Bindings const& bdgBindings) {
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
	
	  size_t szSpecifier = 0, szSpecifierOld = -2;
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
      } else {
        solResult = this->matchLambdaPredicates(exQueryBound, solPrior, bdgBindings);
      }
    }
    
    if(solResult.valid()) {
      solResult.setValid(solResult.bindings().superImpose(bdgBindings));
    }
    
    return solResult;
  }

  Solution Prolog::matchLambdaPredicates(Expression const& exQuery, Solution const& solPrior, Solution::Bindings const& bdgBindings) {
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

  void Prolog::addLambdaPredicate(std::string const& strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda) {
    this->addLambdaPredicate(this->makeLambdaPredicate(strPredicate, fncLambda));
  }
  
  void Prolog::addSimpleLambdaPredicate(std::string const& strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda) {
    this->addLambdaPredicate(this->makeSimpleLambdaPredicate(strPredicate, fncLambda));
  }
  
  void Prolog::addLambdaPredicate(LambdaPredicate const& lpAdd) {
    m_vecLambdaPredicates.push_back(lpAdd);
  }
  
  void Prolog::addLazyListPredicate(std::string const& strPredicate, std::vector<Expression> const& vecList) {
    m_vecLambdaPredicates.push_back(this->makeLazyListPredicate(strPredicate, vecList));
  }
  
  void Prolog::addCallbackPredicate(std::string const& strPredicate, std::function<Expression(unsigned int)> fncLambda) {
    m_vecLambdaPredicates.push_back(this->makeCallbackPredicate(strPredicate, fncLambda));
  }

  Prolog::LambdaPredicate Prolog::makeCallbackPredicate(std::string const& strPredicate, std::function<Expression(unsigned int)> fncLambda) {
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
  
  Prolog::LambdaPredicate Prolog::makeLambdaPredicate(std::string const& strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda) {
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
  
  Prolog::LambdaPredicate Prolog::makeSimpleLambdaPredicate(std::string const& strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda) {
    return this->makeLambdaPredicate(strPredicate, [fncLambda](std::map<std::string, Expression> mapBindings) -> bool { fncLambda(mapBindings); return true; });
  }

  Prolog::LambdaPredicate Prolog::makeLazyListPredicate(std::string const& strPredicate, std::vector<Expression> const& vecList) {
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
    
    this->addLambdaPredicate("(true)", [this](std::map<std::string, Expression> mapBindings) {
	return true;
      });
    
    this->addLambdaPredicate("(false)", [this](std::map<std::string, Expression> mapBindings) {
	return false;
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
  }

  void Prolog::addPredicate(std::string const& strPredicate, std::vector<std::string> const& vecElements) {
    Expression exAnd;
    exAnd.add("and");
    
    for(std::string strElement : vecElements) {
      exAnd.add(Expression::parseSingle(strElement));
    }
    
    m_vecLambdaPredicates.push_back([this, strPredicate, exAnd](Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings) -> Solution {
	Solution solResult;
	solResult.setValid(false);
	
	std::map<std::string, Expression> mapBindings;
	
	if(exQuery.match(strPredicate, mapBindings)) {
	  Solution solTemp = this->unify(exAnd, solPrior, mapBindings);
	  
	  if(solTemp.valid()) {
	    solResult = solTemp;
	  }
	}
	
	return solResult;
      });
  }
}
