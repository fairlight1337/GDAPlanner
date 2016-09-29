#include <gdaplanner/Expression.h>


namespace gdaplanner {
  Expression::Expression() : m_tpType(List) {
  }
  
  Expression::Expression(std::vector<Expression> vecExpressions) : m_tpType(List), m_vecSubExpressions(vecExpressions) {
  }
  
  Expression::Expression(std::string strExpression) : m_tpType(String), m_vbValue(Value<std::string>::create(strExpression)) {
  }
  
  Expression::Expression(const char* arrcExpression) : m_tpType(String), m_vbValue(Value<std::string>::create(std::string(arrcExpression))) {
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
