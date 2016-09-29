#include <gdaplanner/GDAPlanner.h>


namespace gdaplanner {
  GDAPlanner::GDAPlanner() {
  }
  
  GDAPlanner::~GDAPlanner() {
  }
  
  void GDAPlanner::popContext() {
    if(m_dqContexts.size() > 0) {
      m_dqContexts.pop_front();
    } else {
      throw std::exception();
    }
  }
  
  Context::Ptr GDAPlanner::currentContext() {
    if(m_dqContexts.size() > 0) {
      return m_dqContexts.front();
    }
    
    return nullptr;
  }
  
  bool GDAPlanner::readFile(std::string strFilepath) {
    std::ifstream ifFile(strFilepath, std::ios::in);
    
    if(ifFile.good()) {
      std::string strContents((std::istreambuf_iterator<char>(ifFile)),
			      std::istreambuf_iterator<char>());
      
      return this->parseString(strContents);
    }
    
    return false;
  }
  
  bool GDAPlanner::parseString(std::string strSource) {
    try {
      unsigned int unPos = 0;
      strSource = "(" + strSource + ")";
      
      std::vector<Expression> vecSegments = this->parseSegments(strSource, unPos);
      bool bAllGood = true;
      
      for(Expression exExpression : vecSegments[0].subExpressions()) {
	if(!this->processExpression(exExpression)) {
	  bAllGood = false;
	  
	  break;
	}
      }
      
      return bAllGood;
    } catch(std::exception& exCaught) {
      return false;
    }
  }
  
  bool GDAPlanner::processExpression(Expression exProcess) {
    bool bResult = false;
    
    if(exProcess.size() > 0) {
      std::string strCommand = exProcess[0].get<std::string>();
      
      if(strCommand == "declare") {
	bool bDeclareGood = true;
	
	for(unsigned int unI = 1; unI < exProcess.size(); ++unI) {
	  if(exProcess[unI].size() == 2) {
	    std::string strVariable = exProcess[unI][0].get<std::string>();
	    
	    if(strVariable != "") {
	      this->currentContext()->declare(strVariable, exProcess[unI][1]);
	    } else {
	      bDeclareGood = false;
	      
	      break;
	    }
	  } else {
	    bDeclareGood = false;
	    
	    break;
	  }
	}
	
	bResult = bDeclareGood;
      } else if(strCommand == "alias") {
	if(exProcess.size() > 2) {
	  this->currentContext()->alias(exProcess[1], exProcess.subSequence(2));
	  
	  bResult = true;
	}
      } else if(strCommand == "action") {
	if(exProcess.size() == 4) {
	  this->currentContext()->action(exProcess[1], exProcess[2], exProcess[3]);
	  
	  bResult = true;
	}
      } else if(strCommand == "state") {
	if(exProcess.size() == 3) {
	  this->currentContext()->state(exProcess[1], exProcess[2]);
	  
	  bResult = true;
	}
      } else if(strCommand == "fact") {
	if(exProcess.size() == 2) {
	  this->currentContext()->fact(exProcess[1]);
	  
	  bResult = true;
	}
      }
    }
    
    return bResult;
  }
  
  std::vector<Expression> GDAPlanner::parseSegments(std::string& strSource, unsigned int& unPos) {
    std::vector<Expression> vecSegments;
    char cQuoteMode = 0;
    std::string strFragment = "";
    bool bSkipUntilNewline = false;
    
    for(; unPos < strSource.size(); ++unPos) {
      char cCurrent = strSource[unPos];
      
      if(cQuoteMode == 0) {
	if(cCurrent == '(' && !bSkipUntilNewline) {
	  if(strFragment != "") {
	    vecSegments.push_back(strFragment);
	    strFragment = "";
	  }
	  
	  unPos++;
	  vecSegments.push_back(Expression(this->parseSegments(strSource, unPos)));
	} else if(cCurrent == ')' && !bSkipUntilNewline) {
	  if(strFragment != "") {
	    vecSegments.push_back(strFragment);
	    strFragment = "";
	  }
	  
	  break;
	} else if((cCurrent == '"' || cCurrent == '\'') && !bSkipUntilNewline) {
	  cQuoteMode = cCurrent;
	} else {
	  if(cCurrent == ' ' || cCurrent == '\t' || cCurrent == '\n' || cCurrent == '\r') {
	    if(bSkipUntilNewline) {
	      if(cCurrent == '\n') {
		bSkipUntilNewline = false;
	      }
	    } else {
	      if(strFragment != "" && !bSkipUntilNewline) {
		vecSegments.push_back(strFragment);
		strFragment = "";
	      }
	    }
	  } else if(cCurrent == ';') {
	    bSkipUntilNewline = true;
	  } else if(!bSkipUntilNewline) {
	    strFragment += cCurrent;
	  }
	}
      } else {
	if(cCurrent == cQuoteMode) {
	  cQuoteMode = 0;
	  vecSegments.push_back(Expression(strFragment));
	  strFragment = "";
	} else {
	  strFragment += cCurrent;
	}
      }
    }
    
    return vecSegments;
  }
  
  Expression GDAPlanner::simpleExpression(std::string strSource) {
    unsigned int unPos = 0;
    return Expression(this->parseSegments(strSource, unPos)[0]);
  }
}
