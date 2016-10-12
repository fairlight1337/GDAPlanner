#include <gdaplanner/planners/ForwardPlanner.h>
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
      
      plProlog->addPredicate("(do-something ?a ?b)",
			     "(format \"This: ~a, ~a~%\" ?a ?b)",
			     "(format \"This again: ~a, ~a~%\" ?a ?b)",
			     "(member ?a ?b)",
			     "(format \"Got through!~%\")");
      
      Solution solPredicate = plProlog->query("(do-something 2 (1 4 2))");
      std::cout << solPredicate;
    }
    
    void ForwardPlanner::plan(problems::Problem::Ptr prbProblem, contexts::Context::Ptr ctxContext) {
      this->plan(std::dynamic_pointer_cast<problems::PDDL>(prbProblem), std::dynamic_pointer_cast<contexts::PDDL>(ctxContext));
    }
  }
}
