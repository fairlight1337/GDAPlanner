#include <gdaplanner/loaders/PDDL.h>


namespace gdaplanner {
  namespace loaders {
    PDDL::PDDL() {
    }
    
    PDDL::~PDDL() {
    }
    
    bool PDDL::processExpression(Expression exProcess, Context::Ptr ctxContext) {
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
		  bResult = true;
		} else if(strSection == ":action") {
		  bResult = true;
		}
	      }
	    }
	  }
	}
      }
      
      // if(exProcess.size() > 0) {
      // 	contexts::PDDL::Ptr pcxCtx = std::dynamic_pointer_cast<contexts::PDDL>(ctxContext);
      //   std::string strCommand = exProcess[0].get<std::string>();

      //   if(strCommand == "declare") {
      // 	  bool bDeclareGood = true;

      // 	  for(unsigned int unI = 1; unI < exProcess.size(); ++unI) {
      // 	    if(exProcess[unI].size() == 2) {
      // 	      std::string strVariable = exProcess[unI][0].get<std::string>();

      // 	      if(strVariable != "") {
      // 		pcxCtx->declare(strVariable, exProcess[unI][1]);
      // 	      } else {
      // 		bDeclareGood = false;

      // 		break;
      // 	      }
      // 	    } else {
      // 	      bDeclareGood = false;

      // 	      break;
      // 	    }
      // 	  }

      // 	  bResult = bDeclareGood;
      // 	} else if(strCommand == "alias") {
      // 	  if(exProcess.size() > 2) {
      // 	    pcxCtx->alias(exProcess[1], exProcess.subSequence(2));
	    
      // 	    bResult = true;
      // 	  }
      //   } else if(strCommand == "action") {
      // 	  if(exProcess.size() == 4) {
      // 	    pcxCtx->action(exProcess[1], exProcess[2], exProcess[3]);
	    
      // 	    bResult = true;
      // 	  }
      //   } else if(strCommand == "state") {
      // 	  if(exProcess.size() == 3) {
      //       pcxCtx->state(exProcess[1], exProcess[2]);
	    
      // 	    bResult = true;
      // 	  }
      //   } else if(strCommand == "fact") {
      // 	  if(exProcess.size() == 2) {
      // 	    pcxCtx->fact(exProcess[1]);
	    
      // 	    bResult = true;
      // 	  }
      //   }
      // }
      
      return bResult;
    }
    
    Context::Ptr PDDL::makeContext() {
      return contexts::PDDL::create();
    }
  }
}
