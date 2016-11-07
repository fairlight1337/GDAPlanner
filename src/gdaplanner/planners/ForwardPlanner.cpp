#include <gdaplanner/planners/ForwardPlanner.h>
#include <gdaplanner/problems/PDDL.h>

#include <iostream>


namespace gdaplanner {
  namespace planners {
    ForwardPlanner::ForwardPlanner() {
    }
    
    ForwardPlanner::~ForwardPlanner() {
    }

    bool holds(Expression const& goal, Expression const& state, std::map<std::string, Expression> & bdgs)
    {
        Expression goalP = goal.parametrize(bdgs);
        Expression stateP = state;// = state.parametrize(bdgs);

        std::vector<Expression> exGoalAuxVec; exGoalAuxVec.clear(); exGoalAuxVec.push_back(goalP);
        std::vector<Expression> exStateAuxVec; exStateAuxVec.clear(); exStateAuxVec.push_back(stateP);

        if((goalP.isBound()) && (stateP.isBound()))
        {
            bool allMatch = true;
            if(goalP.type() != Expression::List)
                goalP = Expression(exGoalAuxVec);
            if(stateP.type() != Expression::List)
                stateP = Expression(exStateAuxVec);

            int maxG = goalP.subExpressions().size();
            int maxS = stateP.subExpressions().size();
            std::map<std::string, Expression> collBindings; collBindings.clear();
            for(int g = 0; allMatch && (g < maxG); g++)
            {
                bool found = false;
                for(int s = 0; (!found) && (s < maxS); s++)
                {
                    std::map<std::string, Expression> newBindings; newBindings.clear();
                    newBindings = goalP.subExpressions()[g].resolveTo(stateP.subExpressions()[s], found);
                    for(int k = 0; found && (k < maxG); k++)
                    {
                        goalP.subExpressions()[k] = goalP.subExpressions()[k].parametrize(newBindings);
                    }
                    for(std::map<std::string, Expression>::const_iterator it = newBindings.begin();
                        it != newBindings.end(); it++)
                        collBindings.insert(std::pair<std::string, Expression>(it->first, it->second));
                }
                allMatch = found;
            }
            if(allMatch)
                for(std::map<std::string, Expression>::const_iterator it = collBindings.begin();
                    it != collBindings.end(); it++)
                    bdgs.insert(std::pair<std::string, Expression>(it->first, it->second));

            return allMatch;
        }
        return false;
    }
    bool holds(Expression const& goal, Expression const& state)
    {
        std::map<std::string, Expression> bdgs;
        return holds(goal, state, bdgs);
    }

    void updateState(Expression const& effects, Expression const& state, Expression & newState, std::map<std::string, Expression> & bdgs)
    {
        //Expression stateP = state.parametrize(bdgs);
        Expression stateP = state;
        Expression effectsP = effects.parametrize(bdgs);
        std::vector<Expression> exEffectsAuxVec; exEffectsAuxVec.clear(); exEffectsAuxVec.push_back(effectsP);
        std::vector<Expression> exStateAuxVec; exStateAuxVec.clear(); exStateAuxVec.push_back(stateP);

        if((effectsP.isBound()) && (stateP.isBound()))
        {
            std::map<std::string, Expression> bindings; bindings.clear();

            if(effectsP.type() != Expression::List)
                effectsP = Expression(exEffectsAuxVec);
            if(stateP.type() != Expression::List)
                stateP = Expression(exStateAuxVec);

            int maxE = effectsP.size();
            for(int e = 0; e < maxE; e++)
            {
                Expression crEffect = effectsP.subExpressions()[e];
                Expression crNegEffect = crEffect.negate();
                bool positive = (crEffect.subExpressions().size() && crEffect.subExpressions()[0].toString() != "not");
                int maxS = stateP.subExpressions().size();
                bool found = false;
                for(int s = 0; (!found) && (s < maxS); s++)
                {
                    std::map<std::string, Expression> bindings; bindings.clear();
                    bindings = crEffect.resolveTo(stateP.subExpressions()[s], found);
                    if(found)
                        for(int k = 0; k < maxE; k++)
                            effectsP.subExpressions()[k] = effectsP.subExpressions()[k].parametrize(bindings);
                    else
                    {
                        bindings.clear();
                        bool negated = false;
                        bindings = crNegEffect.resolveTo(stateP.subExpressions()[s], negated);
                        if(negated)
                        {
                            crEffect = crEffect.parametrize(bindings);
                            crNegEffect = crNegEffect.parametrize(bindings);
                            stateP.subExpressions()[s] = Expression("");
                            for(int k = 0; k < maxE; k++)
                                effectsP.subExpressions()[k] = effectsP.subExpressions()[k].parametrize(bindings);
                        }
                        else
                            bindings.clear();
                    }
                    if(bindings.size())
                        for(std::map<std::string, Expression>::iterator it = bindings.begin();
                            it != bindings.end(); it++)
                            bdgs.insert(std::pair<std::string, Expression>(it->first, it->second));
                }
                if((!found) && (positive))
                    stateP.subExpressions().push_back(crEffect);
            }

            maxE = stateP.subExpressions().size();
            Expression newStateAux;
            for(int e = 0; e < maxE; e++)
                if(stateP.subExpressions()[e].toString() != "")
                    newStateAux.add(stateP.subExpressions()[e]);
            newState = newStateAux;
        }
    }
    
    void updateState(Expression const& effects, Expression const& state, Expression & newState)
    {
        std::map<std::string, Expression> bdgs;
        updateState(effects, state, newState, bdgs);
    }

    bool fp(Expression const& exStart, Expression const& exGoal, Expression & exFinal, Expression const& exCrPlan, Expression & exPlan, Expression & exIStates, int depth,
            std::vector<Expression> const& availableActions, std::vector<Expression> const& constructedActions)
    {
        /* Base case: goal holds at start*/
        if(holds(exGoal, exStart))
        {
            exFinal = exStart;
            exPlan = exCrPlan;
            return true;
        }
        else if(depth)
        {
            unsigned int maxK = constructedActions.size();
            bool resolved = false;
            for(unsigned int k = 0; (!resolved) && (k < maxK); k++)
            {
                Expression pattern = Expression::parseString("(available-constructed-action ?act ?prec ?eff)")[0];
                std::map<std::string, Expression> bdgsActMatch;
                bdgsActMatch = pattern.resolve(constructedActions[k], resolved);
                if(resolved)
                {
                    std::map<std::string, Expression> bdgsPrecMatch;
                    resolved = holds(bdgsActMatch["?prec"], exStart, bdgsPrecMatch);
                    if(resolved)
                    {
                        Expression exNewState;
                        updateState(bdgsActMatch["?eff"], exStart, exNewState, bdgsPrecMatch);
                        resolved = holds(exGoal, exNewState);
                        if(resolved)
                        {
                            exFinal = exNewState;
                            exPlan = exCrPlan;
                            exPlan.add(bdgsActMatch["?act"]);
                            exIStates.add(exFinal);
                        }
                    }
                }
            }
            if(resolved)
                return true;
            depth--;
            maxK = availableActions.size();
            for(unsigned int k = 0; (!resolved) && (k < maxK); k++)
            {
                Expression pattern = Expression::parseString("(available-action ?act ?prec ?eff)")[0];
                std::map<std::string, Expression> bdgsActMatch;
                bdgsActMatch = pattern.resolve(availableActions[k], resolved);
                if(resolved)
                {
                    std::map<std::string, Expression> bdgsPrecMatch;
                    resolved = holds(bdgsActMatch["?prec"], exStart, bdgsPrecMatch);
                    if(resolved)
                    {
                        Expression exNewState;
                        updateState(bdgsActMatch["?eff"], exStart, exNewState, bdgsPrecMatch);
                        Expression exNewPlan = exCrPlan;
                        exNewPlan.add(bdgsActMatch["?act"]);
                        Expression exNewIStates = exIStates;
                        exNewIStates.add(exNewState);
                        resolved = fp(exNewState, exGoal, exFinal, exNewPlan, exPlan, exNewIStates, depth, availableActions, constructedActions);
                        if(resolved)
                            exIStates = exNewIStates;
                    }
                }
            }
            if(resolved)
                return true;
        }
        return false;
    }

    Solution::Ptr ForwardPlanner::plan(problems::PDDL::Ptr prbProblem, contexts::PDDL::Ptr ctxContext, __attribute__((unused)) Solution::Ptr solPrior) {
      World::Ptr wdWorld = World::create();
      Prolog::Ptr plProlog = Prolog::create(wdWorld);
      /* TODO: incorporate object type constraints into action parametrization*/
      std::vector<problems::PDDL::Object> objects = prbProblem->objects();

      std::vector<Expression> availableActions;
      /* TODO: populate the constructed actions somehow.*/
      std::vector<Expression> constructedActions;

      /* Loop over available actions*/
      unsigned int maxA = ctxContext->actionCount();
      for(unsigned int a = 0; a < maxA; a++)
      {
          Expression exPrExp = ctxContext->action(a)->predicateExpression();
          Expression exPrecs = ctxContext->action(a)->preconditions().conjunctionToList();
          Expression exEffs = ctxContext->action(a)->effects().conjunctionToList();
          std::vector<std::string> varNames;
          exPrExp.getVarNames(varNames);
          unsigned int maxV = varNames.size();
          std::vector<unsigned int> objIndex; objIndex.resize(maxV);
          std::vector<unsigned int> upLims; upLims.resize(maxV, objects.size());
          bool ovrFlw = false;
          for(; !ovrFlw; )
          {
              std::map<std::string, Expression> mapBdgs;
              for(unsigned int v = 0; v < maxV; v++)
                  mapBdgs[varNames[v]] = Expression::parseString(objects[objIndex[v]].strName)[0];

              if(ctxContext->actionArgumentsValid(exPrExp.parametrize(mapBdgs), prbProblem)) {
                  std::string factString = "(available-action ";
                  factString += exPrExp.parametrize(mapBdgs).toString() + " ";
                  factString += exPrecs.parametrize(mapBdgs).toString() + " ";
                  factString += exEffs.parametrize(mapBdgs).toString() + " ";
                  factString += ")";
                  plProlog->addFact(factString);

                  availableActions.push_back(Expression::parseString(factString)[0]);
                }

              bool done = false;
              for(unsigned int v = maxV; !done; )
                  if(!v)
                      ovrFlw = done = true;
                  else
                  {
                      v--;
                      objIndex[v]++;
                      if(upLims[v] <= objIndex[v])
                          objIndex[v] = 0;
                      else
                          done = true;
                  }
          }
      }

      /*Using the C++ implementation for now*/
#if 0
      /* Define the fp predicate*/
      plProlog->addPredicate("(fp ?s ?g ?f ?cp ?p ?d)",
                             "(holds ?g ?s)",
                             "(= ?s ?f)",
                             "(= ?cp ?p)");
      plProlog->addPredicate("(fp ?s ?g ?f ?cp ?p ?d)",
                             "(available-constructed-action ?act ?prec ?eff)",
                             "(holds ?prec ?s)",
                             "(apply ?eff ?s ?f)",
                             "(holds ?g ?f)",
                             "(append ?act ?cp ?p)");
      plProlog->addPredicate("(fp ?s ?g ?f ?cp ?p ?d)",
                             "(1- ?d ?nd)",
                             "(available-action ?act ?prec ?eff)",
                             "(holds ?prec ?s)",
                             "(apply ?eff ?s ?ns)",
                             "(append ?act ?cp ?ncp)",
                             "(fp ?ns ?g ?f ?ncp ?p ?nd)");
      plProlog->addPredicate("(fp ?s ?g ?f ?p ?d)",
                             "(fp ?s ?g ?f () ?p ?d)");
#endif

      /* Run fp queries*/
      std::vector<Expression> initExpressions = prbProblem->initExpressions();
      unsigned int maxS = initExpressions.size();
      Expression exStart;
      for(unsigned int s = 0; s < maxS; s++)
          exStart.add(initExpressions[s]);
      Expression exGoals = prbProblem->goal().conjunctionToList();
      Expression exFinal;
      Expression exPlan;
      Expression exIStates;

      std::string strPlanQuery = "(fp (";
      for(unsigned int s = 0; s < maxS; s++)
          strPlanQuery += initExpressions[s].toString() + " ";
      strPlanQuery += ") ";
      strPlanQuery += exGoals.toString();
      strPlanQuery += " ?f ?p ";

      /* TODO: Need to decide on a depth limit to cut-off at.*/
      /* Currently, set to 5. Note that because the branching factor is
         going to be in the order of several tens, we can expect the search space to grow
         very fast. At a depth of 5 and branching factor of 100, there are 10^10 possible
         plans!*/
      /* TODO: Need to decide on a criterion to stop looking for solutions.*/
      /* Currently, planner stops when the first planner is found, ie. the one
         with the fewest actions.*/
      bool shouldStop = false;
      Solution::Ptr solResult = Solution::create();
      solResult->setValid(false);
      for(int depth = 1; (!shouldStop) && (depth < 5); depth++)
      {
          shouldStop = fp(exStart, exGoals, exFinal, Expression::parseString("()")[0], exPlan, exIStates, depth, availableActions, constructedActions);
      }

      if(shouldStop)
      {
          solResult->bindings()["?p"] = exPlan;
          solResult->bindings()["?f"] = exFinal;
          solResult->bindings()["?h"] = exIStates;
          solResult->setValid(true);
      }

      return solResult;
    }
    
    Solution::Ptr ForwardPlanner::plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext, Solution::Ptr solPrior) {
      problems::PDDL::Ptr prbPDDLProblem = std::dynamic_pointer_cast<problems::PDDL>(prbProblem);
      contexts::PDDL::Ptr ctxPDDLContext = std::dynamic_pointer_cast<contexts::PDDL>(ctxContext);
      
      if(prbPDDLProblem) {
	if(ctxPDDLContext) {
	  std::cout << "Converted request into PDDL compatible data structures" << std::endl;
	  
	  return this->plan(prbPDDLProblem, ctxPDDLContext, solPrior);
	} else {
	  std::cerr << "Error: Non-PDDL context in PDDL planner" << std::endl;
	}
      } else {
	std::cerr << "Error: Non-PDDL problem in PDDL planner" << std::endl;
      }
      
      return nullptr;
    }
  }
}
