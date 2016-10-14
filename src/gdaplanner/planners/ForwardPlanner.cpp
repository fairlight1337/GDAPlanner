#include <gdaplanner/planners/ForwardPlanner.h>
#include <gdaplanner/problems/PDDL.h>

#include <iostream>


namespace gdaplanner {
  namespace planners {
    ForwardPlanner::ForwardPlanner() {
    }
    
    ForwardPlanner::~ForwardPlanner() {
    }
    
    void ForwardPlanner::plan(problems::PDDL::Ptr prbProblem, contexts::PDDL::Ptr ctxContext) {
      World::Ptr wdWorld = World::create();
      Prolog::Ptr plProlog = Prolog::create(wdWorld);
      
      /*
      plProlog->addLazyListPredicate("(init-predicate ?predicate)", prbProblem->initExpressions());
      plProlog->addCallbackPredicate("(action ?action)", [ctxContext](unsigned int unIndex) -> Expression {
	  if(unIndex < ctxContext->actionCount()) {
	    return ctxContext->action(unIndex)->expression();
	  } else {
	    throw SolutionsExhausted();
	  }
	});
      
      Solution solInitPredicates = plProlog->query("(foreach ?a (init-predicate ?a) ?predicates)");
      
      if(solInitPredicates.valid()) {
          std::cout << solInitPredicates;
      } else {
          std::cerr << "Invalid: Init predicates" << std::endl;
      }
      
      Solution solActions = plProlog->query("(foreach ?a (action ?a) ?actions)");
      
      if(solActions.valid()) {
          std::cout << solActions;
      } else {
          std::cerr << "Invalid: Actions" << std::endl;
      }
      */

      /*
      std::vector<Expression> initExpressions = prbProblem->initExpressions();
      unsigned int maxK = initExpressions.size();
      std::cout << "Init expressions: ";
      for(unsigned int k = 0; k < maxK; k++)
      {
          plProlog->addFact(initExpressions[k]);
          std::cout << " " << initExpressions[k];
      }
      std::cout << "\n";
      Solution solPredicateAux;
      while(solPredicateAux.valid())
      {
          solPredicateAux = plProlog->query("(not (found ?x))", solPredicateAux);
          if(solPredicateAux.valid())
              std::cout << solPredicateAux;
      }
      */

      /* TODO: incorporate object type constraints into action parametrization*/
      std::vector<problems::PDDL::Object> objects = prbProblem->objects();

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
	      
	      if(ctxContext->predicateArgumentsValid(exPrExp.parametrize(mapBdgs), prbProblem)) {
		std::string factString = "(available-action ";
		factString += exPrExp.parametrize(mapBdgs).toString() + " ";
		factString += exPrecs.parametrize(mapBdgs).toString() + " ";
		factString += exEffs.parametrize(mapBdgs).toString() + " ";
		factString += ")";
		std::cout << factString << "\n";
		plProlog->addFact(factString);
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

      /* Define the fp predicate*/
      plProlog->addPredicate("(fp ?s ?g ?f ?cp ?p ?d)",
                             "(holds ?g ?s)",
                             "(= ?s ?f)",
                             "(= ?cp ?p)");
      plProlog->addPredicate("(fp ?s ?g ?f ?cp ?p ?d)",
                             "(1- ?d ?nd)",
                             "(available-action ?act ?prec ?eff)",
                             "(holds ?prec ?s)",
                             "(apply ?eff ?s ?ns)",
                             "(append ?act ?cp ?ncp)",
                             "(fp ?ns ?g ?f ?ncp ?p ?nd)");
      plProlog->addPredicate("(fp ?s ?g ?f ?p ?d)",
                             "(fp ?s ?g ?f () ?p ?d)");

      /* Run fp queries*/

      /*
      std::cout << "\n\n";
      Expression f = Expression::parseString("(not (found ?x))")[0];
      Expression nf = Expression::parseString("(not (found plate0))")[0];
      bool b;
      std::map<std::string, Expression> mapBdgs = f.resolve(nf, b);
      for(std::map<std::string, Expression>::const_iterator it = mapBdgs.begin();
          it != mapBdgs.end(); it++)
          std::cout << b << " " << it->first << " " << it->second.toString().c_str() << "\n";
      */
      
      /*
      plProlog->addPredicate("(do-something ?a ?b)",
                             "(format \"This: ~a, ~a~%\" ?a ?b)",
                             "(format \"This again: ~a, ~a~%\" ?a ?b)",
                             "(member ?a ?b)",
                             "(format \"Got through!~%\")");
      
      Solution solPredicate = plProlog->query("(do-something 2 (1 4 2))");
      std::cout << solPredicate;
      */
    }
    
    void ForwardPlanner::plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext) {
      this->plan(std::dynamic_pointer_cast<problems::PDDL>(prbProblem), std::dynamic_pointer_cast<contexts::PDDL>(ctxContext));
    }
  }
}
