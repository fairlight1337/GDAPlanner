#include <gdaplanner/Expression.h>


namespace gdaplanner {
  Expression::Expression() : m_tpType(List) {
  }
  
  Expression::Expression(std::vector<Expression> vecExpressions) : m_tpType(List), m_vecSubExpressions(vecExpressions) {
  }
  
  Expression::Expression(std::string strExpression) : m_tpType(String), m_vbValue(nullptr) {
    this->parseAssertStringValue(strExpression);
  }
  
  Expression::Expression(const char* arrcExpression) : m_tpType(String), m_vbValue(nullptr) {
    this->parseAssertStringValue(std::string(arrcExpression));
  }
  
  Expression::Expression(double dExpression) : m_tpType(Double), m_vbValue(Value<double>::create(dExpression)) {
  }
  
  Expression::Expression(float fExpression) : m_tpType(Float), m_vbValue(Value<float>::create(fExpression)) {
  }
  
  Expression::Expression(int nExpression) : m_tpType(Integer), m_vbValue(Value<int>::create(nExpression)) {
  }
  
  Expression::Expression(unsigned int unExpression) : m_tpType(UnsignedInteger), m_vbValue(Value<unsigned int>::create(unExpression)) {
  }
  
  Expression::~Expression() {
  }
  
  bool Expression::parseAssertStringValue(std::string strValue) {
    // Check for floating point first; we resort to double rather than
    // float per default. If we don't detect a decimal dot, we use
    // integer.
    if(strValue.length() > 0 && strValue != "-") {
      bool bNegative = (strValue[0] == '-');
      bool bDecimalDot = false;
      bool bThisIsAString = false;
      
      if(bNegative) {
	strValue = strValue.substr(1);
      }
      
      for(unsigned int unI = 0; unI < strValue.length(); ++unI) {
	if(strValue[unI] == '.') {
	  if(unI == 0) {
	    // Numbers don't start with a dot. This is a string.
	    bThisIsAString = true;
	    break;
	  } else {
	    bDecimalDot = true;
	  }
	} else if(!isdigit(strValue[unI])) {
	  // There is a non-digit character in here. This is a string,
	  // not a number.
	  bThisIsAString = true;
	  break;
	}
      }
      
      if(!bThisIsAString) {
	if(bDecimalDot) {
	  try {
	    m_vbValue = Value<double>::create((bNegative ? -1 : 1) * std::stod(strValue));
	    m_tpType = Double;
	  } catch(const std::invalid_argument& ia) {
	    // Something went wrong during conversion; this might be a
	    // string after all.
	    m_vbValue = Value<std::string>::create(strValue);
	    m_tpType = String;
	  }
	} else {
	  try {
	    m_vbValue = Value<int>::create((bNegative ? -1 : 1) * std::stoi(strValue));
	    m_tpType = Integer;
	  } catch(const std::invalid_argument& ia) {
	    // Something went wrong during conversion; this might be a
	    // string after all.
	    m_vbValue = Value<std::string>::create(strValue);
	    m_tpType = String;
	  }
	}
      } else {
	m_vbValue = Value<std::string>::create(strValue);
	m_tpType = String;
      }
    } else {
      // Zero length; this is a string.
      m_vbValue = Value<std::string>::create(strValue);
      m_tpType = String;
    }
  }
  
  void Expression::getVarNames(std::vector<std::string> & varNames) const
  {
      if(isVariable())
      {
          bool matched = false;
          int maxK = varNames.size();
          std::string thisName = toString();
          for(int k = 0; (!matched) && (k < maxK); k++)
              matched = (varNames[k] == thisName);
          if(!matched)
              varNames.push_back(thisName);
      }
      else
      {
          int maxK = subExpressions().size();
          for(int k = 0; k < maxK; k++)
              subExpressions()[k].getVarNames(varNames);
      }
  }

  bool Expression::hasSubExpression(Expression const& what) const
  {
      if(this->toString() == what.toString())
          return true;
      int maxK = subExpressions().size();
      bool retq = false;
      for(int k = 0; (!retq) && (k < maxK); k++)
          retq = subExpressions()[k].hasSubExpression(what);
      return retq;
  }
  
  Expression Expression::parseSingle(std::string strSource) {
    std::vector<Expression> vecParsed = Expression::parseString(strSource);
    
    if(vecParsed.size() > 0) {
      return vecParsed[0];
    }
    
    throw std::exception();
  }
  
  std::vector<Expression> Expression::parseString(std::string strSource) {
    unsigned int unPos = 0;
    strSource = "(" + strSource + ")";
    
    return Expression::parseString(strSource, unPos)[0].subExpressions();
  }
  
  std::vector<Expression> Expression::parseString(std::string& strSource, unsigned int& unPos) {
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
	  vecSegments.push_back(Expression(Expression::parseString(strSource, unPos)));
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
}
