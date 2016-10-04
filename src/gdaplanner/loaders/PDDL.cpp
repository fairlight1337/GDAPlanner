#include <gdaplanner/loaders/PDDL.h>


namespace gdaplanner {
  namespace loaders {
    PDDL::PDDL() {
    }
    
    PDDL::~PDDL() {
    }
    
    bool PDDL::processExpression(Expression exProcess, contexts::Context::Ptr ctxContext) {
      bool bResult = false;
      
      if(exProcess.type() == Expression::List && exProcess.size() > 0) {
	contexts::PDDL::Ptr pcxCtx = std::dynamic_pointer_cast<contexts::PDDL>(ctxContext);
	
	if(pcxCtx) {
	  if(exProcess[0] == "define") {
	    for(unsigned int unI = 1; unI < exProcess.size(); ++unI) {
	      Expression exDetail = exProcess[unI];
	      
	      if(exDetail.type() == Expression::List && exDetail.size() > 0) {
		std::string strSection = exDetail[0].get<std::string>();
		
		if(strSection == "domain") {
		  if(exDetail.size() > 1) {
		    pcxCtx->domain(exDetail[1].get<std::string>());
		    
		    bResult = true;
		  }
		} else if(strSection == ":requirements") {
		  for(unsigned int unR = 1; unR < exDetail.size(); ++unR) {
		    pcxCtx->requirement(exDetail[unR].get<std::string>());
		  }
		  
		  bResult = true;
		} else if(strSection == ":types") {
		  for(unsigned int unT = 1; unT < exDetail.size(); ++unT) {
		    pcxCtx->type(exDetail[unT].get<std::string>());
		  }
		  
		  bResult = true;
		} else if(strSection == ":predicates") {
		  bool bOK = true;
		  
		  for(Expression exPredicate : exDetail.subSequence(1)) {
		    if(!pcxCtx->addPredicate(exPredicate)) {
		      bOK = false;
		      break;
		    }
		  }
		  
		  bResult = bOK;
		} else if(strSection == ":functions") {
		  bResult = pcxCtx->addFunctions(exDetail.subSequence(1));
		} else if(strSection == ":constants") {
		  bResult = pcxCtx->addConstants(exDetail.subSequence(1));
		} else if(strSection == ":action") {
		  bResult = pcxCtx->addAction(exDetail.subSequence(1));
		}
	      }
	      
	      if(!bResult) {
		break;
	      }
	    }
	  }
	}
      }
      std::cout << "Return from ctx" << std::endl;
      return bResult;
    }
    
    contexts::Context::Ptr PDDL::makeContext() {
      return contexts::PDDL::create();
    }
    
    bool PDDL::processExpression(Expression exProcess, problems::Problem::Ptr prbProblem) {
      bool bResult = false;
      
      if(exProcess.type() == Expression::List && exProcess.size() > 0) {
	problems::PDDL::Ptr prbPrb = std::dynamic_pointer_cast<problems::PDDL>(prbProblem);
	
	if(prbPrb) {
	  if(exProcess[0] == "define") {
	    if(exProcess.size() > 1 && exProcess[1].size() == 2) {
	      if(exProcess[1][0] == "problem") {
		bResult = true;
		prbPrb->setIdentifier(exProcess[1][1].get<std::string>());
		
		Expression exDetails = exProcess.subSequence(2);
		std::vector<Expression> vecSubExpressions = exDetails.subExpressions();
		
		for(Expression exDetail : vecSubExpressions) {
		  if(exDetail.type() == Expression::List && exDetail.size() > 1) {
		    if(exDetail[0] == ":domain") {
		      if(exDetail.size() == 2) {
			prbPrb->setDomain(exDetail[1].get<std::string>());
		      } else {
			bResult = false;
		      }
		    } else if(exDetail[0] == ":objects") {
		      exDetail.popFront();
		      
		      while(exDetail.size() > 0) {
			std::vector<Expression> vecObject;
			
			while(!(exDetail[0] == "-") && exDetail.size() > 0) {
			  vecObject.push_back(exDetail[0]);
			  exDetail.popFront();
			}
			
			if(exDetail.size() > 1) {
			  exDetail.popFront();
			  
			  if(exDetail.size() > 0) {
			    std::string strType = exDetail[0].get<std::string>();
			    for(Expression exObject : vecObject) {
			      prbPrb->addObject(exObject.get<std::string>(), strType);
			    }
			  }
			} else {
			  break;
			}
		      }
		    } else if(exDetail[0] == ":init") {
		      prbPrb->setInitExpressions(exDetail.subSequence(1));
		    } else if(exDetail[0] == ":goal") {
		      prbPrb->setGoal(exDetail[1]);
		    } else if(exDetail[0] == ":metric") {
		      prbPrb->setMetric(exDetail.subSequence(1));
		    }
		  }
		  
		  if(!bResult) {
		    break;
		  }
		}
	      }
	    }
	  }
	}
      }
      
      std::cout << "Return from prb" << std::endl;
      
      return bResult;
    }
    
    problems::Problem::Ptr PDDL::makeProblem() {
      return problems::PDDL::create();
    }
  }
}
