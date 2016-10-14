#include <gdaplanner/Action.h>
#include <iostream>


namespace gdaplanner {
  Action::Action(Predicate::Ptr pdPredicate, Expression exPreconditions, Expression exEffects) : m_pdPredicate(pdPredicate), m_exPreconditions(exPreconditions), m_exEffects(exEffects) {
  }
  
  Action::~Action() {
  }
  
  Expression Action::expression() {
    Expression exAction;
    
    Expression exPredicate;
    exPredicate.add("predicate");
    exPredicate.add(m_pdPredicate->expression());
    // TODO: Add the types
    exAction.add(exPredicate);
    
    Expression exPrecondition;
    exPrecondition.add("precondition");
    exPrecondition.add(m_exPreconditions);
    exAction.add(exPrecondition);
    
    Expression exEffect;
    exEffect.add("effect");
    exEffect.add(m_exEffects);
    exAction.add(exEffect);
    
    return exAction;
  }
  
  Predicate::Ptr Action::predicate() {
    return m_pdPredicate;
  }
  
  std::string Action::toString() const {
    std::stringstream sts;
    
    sts << "predicate = " << (*m_pdPredicate).toString() << std::endl;
    sts << "   preconditions = " << m_exPreconditions.toString() << std::endl;
    sts << "   effects = " << m_exEffects.toString();
    
    return sts.str();
  }

  std::string expVec2ConjString(std::vector<Expression> const& expVec)
  {
      unsigned int maxK = expVec.size();

      if(0 == maxK)
          return std::string("()");
      if(1 == maxK)
          return expVec[0].toString();
      std::string retq; retq = "(and ";
      for(unsigned int k = 0; k < maxK; k++)
          if("()" != expVec[k].toString())
              retq += (expVec[k].toString() + " ");
      retq += ")";
      return retq;
  }

  Expression decorateVars(Expression const& exp, int idx)
  {
      Expression expCp = exp;
      std::vector<std::string> varNames; varNames.clear();
      std::map<std::string, Expression> newNames; newNames.clear();

      std::string decStr = std::to_string(idx);

      exp.getVarNames(varNames);
      unsigned int maxK = varNames.size();
      for(unsigned int k = 0; k < maxK; k++)
          newNames.insert(std::pair<std::string, Expression>(varNames[k], Expression::parseString(varNames[k] + decStr)[0]));
      return expCp.parametrize(newNames);
  }

  Action Action::matchAction(std::vector<Action> const& prototypes, Expression const& step, int idx)
  {
      Expression preconditions;
      Expression effects;
      Predicate::Ptr predicatePtr;

      std::cout << "MATCHACTION\nPrototypes:\n";
      for(unsigned int k = 0; k < prototypes.size(); k++)
          std::cout << "  " << prototypes[k].m_pdPredicate->expression().toString().c_str() << " := " << prototypes[k].preconditions().toString().c_str() << "  " << prototypes[k].effects().toString().c_str() << "\n";
      std::cout << "Expression to match:\n  " << step.toString().c_str() << "\n";
      std::cout << "Matching ... \n";

      unsigned int maxK = prototypes.size();
      bool matched = false;
      for(unsigned int k = 0; (!matched) && (k < maxK); k++)
      {
          Expression crPred = prototypes[k].m_pdPredicate->expression();
          Expression stepCp = step;
          crPred.resolve(stepCp, matched);
          if(matched)
          {
              preconditions = prototypes[k].m_exPreconditions;
              effects = prototypes[k].m_exEffects;
              predicatePtr.reset(new Predicate(crPred));
          }
      }
      if(matched && (0 < idx))
      {
          preconditions = decorateVars(preconditions, idx);
          effects = decorateVars(effects, idx);
          predicatePtr.reset(new Predicate(decorateVars(predicatePtr->expression(), idx)));
          std::cout << "Matchedto:\n" << "  " << predicatePtr->expression().toString().c_str() << " " << preconditions.toString().c_str() << " " << effects.toString().c_str() << "\n";
      }
      /*We actually assume a match is found here.*/
      return Action(predicatePtr, preconditions, effects);
  }

  Action Action::parametrize(std::map<std::string, Expression> const& params) const
  {
      Expression pred = m_pdPredicate->expression();
      Expression precs = m_exPreconditions;
      Expression effs = m_exEffects;

      pred = pred.parametrize(params);
      precs = precs.parametrize(params);
      effs = effs.parametrize(params);
      return Action(Predicate::Ptr(new Predicate(pred)), precs, effs);
  }

  void Action::initFromSequence(std::string const& name, std::vector<Action> const& prototypes, std::vector<Expression> const& steps)
  {
      unsigned int maxK = steps.size();

      std::cout << "Step 0." << std::endl;
      /*Step 0: create a new list of prototypes where all variable names are unique.*/
      /*List also contains one prototype for each step in the sequence; there may be
        duplicate prototypes therefore.*/
      std::vector<Action> auxPrototypes; auxPrototypes.clear();
      auxPrototypes.reserve(maxK);

      for(unsigned int k = 0; k < maxK; k++)
          auxPrototypes.push_back(matchAction(prototypes, steps[k], k));

      std::cout << "AUXPROTOTYPES:\n";
      for(unsigned int k = 0; k < maxK; k++)
          std::cout << "  " << auxPrototypes[k].m_pdPredicate->expression().toString().c_str() << " := " << auxPrototypes[k].preconditions().toString().c_str() << "  " << auxPrototypes[k].effects().toString().c_str() << "\n";

      std::cout << "Step 0." << std::endl;
      /*Step 1: gather parameters.*/
      /*The purpose here is to see how many of the parameter slots in the steps sequence
        are occupied by the same value, and equate the parameter variables associated
        to those slots*/
      std::map<std::string, Expression> param2ValueMap;
      std::map<std::string, Expression> param2ParamMap;
      std::vector<std::string> params;

      /* LOOP over steps.*/
      for(unsigned int k = 0; k < maxK; k++)
      {
          std::vector<Expression> subExpProto = auxPrototypes[k].m_pdPredicate->expression().subExpressions();
          std::vector<Expression> subExpStep = steps[k].subExpressions();
          /* We assume here that subExpProto and subExpStep have the same size.*/
          /* Other assumptions: subExpProto only contains the predicate name and variable names,
             whereas subExpStep contains only grounded atoms.*/
          int maxJ = subExpStep.size();
          /* LOOP over subexpressions in a step.*/
          /* Skip index 0, as it containts the predicate name which we don't care about here
             when just looking for variables.*/
          for(int j = 1; (j < maxJ); j++)
          {
              bool matched = false;
              std::string matchingKey;
              /* LOOP over previously established param-name -> value matchings.*/
              for(std::map<std::string, Expression>::iterator it = param2ValueMap.begin();
                  (!matched) && (it != param2ValueMap.end()); it++)
              {
                  matched = (it->second.toString() == subExpStep[j].toString());
                  matchingKey = it->first;
              }
              if(!matched)
                  param2ValueMap.insert(std::pair<std::string, Expression>(subExpProto[j].toString(), subExpStep[j]));
              else
                  param2ParamMap.insert(std::pair<std::string, Expression>(subExpProto[j].toString(), Expression::parseString(matchingKey)[0]));
          }
      }

      std::cout << "PARAM2VALUE:\n";
      for(std::map<std::string, Expression>::const_iterator it = param2ValueMap.begin();
          it != param2ValueMap.end(); it++)
          std::cout << "  " << it->first << " : " << it->second.toString().c_str() << "\n";
      std::cout << "PARAM2PARAM:\n";
      for(std::map<std::string, Expression>::const_iterator it = param2ParamMap.begin();
          it != param2ParamMap.end(); it++)
          std::cout << "  " << it->first << " : " << it->second.toString().c_str() << "\n";

      std::cout << "Step 0." << std::endl;
      /*Step 2: create a new list of prototypes where parameters associated to the same value
        in steps will have the same name.*/
      for(unsigned int k = 0; k < maxK; k++)
          auxPrototypes[k] = auxPrototypes[k].parametrize(param2ParamMap);

      std::cout << "AUXPROTOTYPES:\n";
      for(unsigned int k = 0; k < maxK; k++)
          std::cout << "  " << auxPrototypes[k].m_pdPredicate->expression().toString().c_str() << " := " << auxPrototypes[k].preconditions().toString().c_str() << "  " << auxPrototypes[k].effects().toString().c_str() << "\n";

      /*Step 2.5: handle side-effects.
        Side-effects are expressions that
            - appear as subxepressions of an action's preconditions or effects,
            - appear as the parameter values of some OTHER action in the sequence.
        Handling means replacing, in auxPrototypes, the parameter variable matched to such a side-effect
        expression by the side-effect expression and appropriately updating the params vector.

        In other words, this step is meant to answer whether an action's parameter value in steps
        must remain as is, or whether it can be replaced by a variable.*/
      std::map<std::string, Expression> sideEffects; sideEffects.clear();

      /*LOOP over param->vaue matchings*/
      for(std::map<std::string, Expression>::iterator it = param2ValueMap.begin();
          it != param2ValueMap.end(); it++)
          /*LOOP over auxPrototypes*/
          for(unsigned int k = 0; k < maxK; k++)
              if((!auxPrototypes[k].m_pdPredicate->expression().hasSubExpression(Expression::parseString(it->first)[0])) &&
                 (auxPrototypes[k].preconditions().hasSubExpression(it->second) || auxPrototypes[k].effects().hasSubExpression(it->second)))
                  sideEffects.insert(std::pair<std::string, Expression>(it->first, it->second));

      for(unsigned int k = 0; k < maxK; k++)
          auxPrototypes[k] = auxPrototypes[k].parametrize(sideEffects);

      for(std::map<std::string, Expression>::iterator it = param2ValueMap.begin();
          it != param2ValueMap.end(); it++)
          if(sideEffects.end() == sideEffects.find(it->first))
              params.push_back(it->first);

      std::cout << "AUXPROTOTYPES:\n";
      for(unsigned int k = 0; k < maxK; k++)
          std::cout << "  " << auxPrototypes[k].m_pdPredicate->expression().toString().c_str() << " := " << auxPrototypes[k].preconditions().toString().c_str() << "  " << auxPrototypes[k].effects().toString().c_str() << "\n";

      std::cout << "Step 0." << std::endl;
      /*Step 3: gather pre- and postconditions.*/
      /*This is done in tandem, so as to check if intermediate effects satisfy intermediate
        preconditions, even if the intermediate effects get clobbered later.*/
      std::vector<Expression> preconditions; preconditions.clear();
      std::vector<Expression> effects; effects.clear();

      /*LOOP over steps (or rather, auxPrototypes, which has the same length as steps)*/
      for(unsigned int k = 0; k < maxK; k++)
      {
          Expression aux = auxPrototypes[k].m_exPreconditions;
          /*Regularization: ensure that aux is of the form (AND <expressions>)*/
          std::string str;
          if("and" != aux.subExpressions()[0].toString())
              aux = Expression::parseString(std::string("(and ") + aux.toString() + ")")[0];
          int maxJ = aux.subExpressions().size();
          /*LOOP over preconditions in the current step*/
          /*Skip first index (it should contain only the AND).*/
          for(int j = 1; (j < maxJ); j++)
          {
              int maxL = effects.size();
              bool matched = false;
              /*LOOP over previously established effects that are still active.*/
              for(int l = 0; (!matched) && (l < maxL); l++)
                  matched = (effects[l].toString() == aux.subExpressions()[j].toString());
              if(!matched)
                  preconditions.push_back(aux.subExpressions()[j]);
          }
          aux = auxPrototypes[k].m_exEffects;
          /*Regularization: ensure that aux is of the form (AND <expressions>)*/
          if("and" != aux.subExpressions()[0].toString())
              aux = Expression::parseString(std::string("(and ") + aux.toString() + ")")[0];
          maxJ = aux.subExpressions().size();
          /*LOOP over effects in the current step*/
          /*Skip first index (it should contain only the AND).*/
          for(int j = 1; (j < maxJ); j++)
          {
              int maxL = effects.size();
              /*LOOP over previously established effects that are still active.*/
              for(int l = 0; l < maxL; l++)
                  if((effects[l].negate().toString() == aux.subExpressions()[j].toString())
                          || (effects[l].toString() == aux.subExpressions()[j].negate().toString()))
                      effects[l] = Expression::parseString("()")[0];
              std::cout << "PUSH_BACK " << aux.subExpressions()[j] << "\n";
              effects.push_back(aux.subExpressions()[j]);
          }
      }

      std::cout << "Step 0." << std::endl;
      /*Step 4: store result in this object*/
      m_exPreconditions = Expression::parseString(expVec2ConjString(preconditions))[0];
      m_exEffects = Expression::parseString(expVec2ConjString(effects))[0];
      maxK = params.size();
      std::string predString = "(" + name;
      for(unsigned int k = 0; k < maxK; k++)
          predString += (" " + params[k]);
      predString += ")";
      m_pdPredicate.reset(new Predicate(Expression::parseString(predString)[0]));
  }

}
