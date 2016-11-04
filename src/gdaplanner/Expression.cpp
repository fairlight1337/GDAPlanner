#include <gdaplanner/Expression.h>
#include <iostream>


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
  
  Expression::Type Expression::type() const {
    return m_tpType;
  }
  
  unsigned int Expression::size() const {
    return m_vecSubExpressions.size();
  }
  
  Expression& Expression::operator[](unsigned int unIndex) {
    return m_vecSubExpressions[unIndex];
  }
  
  Expression const& Expression::operator[](unsigned int unIndex) const {
    return m_vecSubExpressions[unIndex];
  }
  
  std::vector<Expression>& Expression::subExpressions() {
    return m_vecSubExpressions;
  }
  
  std::vector<Expression> const& Expression::subExpressions() const {
    return m_vecSubExpressions;
  }
  
  std::vector<Expression> Expression::subSequence(unsigned int unStart, unsigned int unLength) const {
    std::vector<Expression> vecSequence;
    
    if(unLength == 0) {
      unLength = m_vecSubExpressions.size() - unStart;
    }
    
    for(unsigned int unI = unStart; unI < unStart + unLength; ++unI) {
      vecSequence.push_back(m_vecSubExpressions[unI]);
    }
    
    return vecSequence;
  }
  
  Expression Expression::parametrize(std::map<std::string, Expression> const& mapParametrization) const {
    Expression exParametrized = *this;
      
    switch(m_tpType) {
    case String: {
      std::string strUs = this->get<std::string>();
	
      if(strUs.size() > 0) {
	if(strUs[0] == '?') {
	  std::map<std::string, Expression>::const_iterator it = mapParametrization.find(strUs);
	  if(it != mapParametrization.end()) {
	    exParametrized = it->second;
	  }
	}
      }
    } break;
	
    case List: {
      std::vector<Expression> vecSubNew;
	
      for(Expression exSub : m_vecSubExpressions) {
	vecSubNew.push_back(exSub.parametrize(mapParametrization));
      }
	
      exParametrized = Expression(vecSubNew);
    } break;
	
    default: {
    } break;
    }
      
    return exParametrized;
  }

  std::string Expression::toString() const {
    std::stringstream sts;
      
    switch(m_tpType) {
    case List: {
      sts << "(";
      bool bFirst = true;
	
      for(Expression exSub : m_vecSubExpressions) {
	if(bFirst) {
	  bFirst = false;
	} else {
	  sts << " ";
	}
	  
	sts << exSub;
      }
      sts << ")";
    } break;
	
    case String: {
      std::string strUs = this->get<std::string>();
      bool bSpace = false;
	
      for(unsigned int unI = 0; unI < strUs.size(); ++unI) {
	if(isspace(strUs[unI])) {
	  bSpace = true;
	  break;
	}
      }
	
      if(bSpace) sts << "\"";
      sts << strUs;
      if(bSpace) sts << "\"";
    } break;
	
    case Double: sts << this->get<double>(); break;
    case Float: sts << this->get<float>(); break;
    case Integer: sts << this->get<int>(); break;
    case UnsignedInteger: sts << this->get<unsigned int>(); break;
    }
      
    return sts.str();
  }
  
  bool Expression::isWildcard() const {
    return (m_tpType == String && *this == "?_");
  }
  
  bool Expression::hasWildcard() const {
    if(m_tpType == List) {
      for(Expression exTest : m_vecSubExpressions) {
	if(exTest.isWildcard()) {
	  return true;
	}
      }
    }
      
    return false;
  }
  
  bool Expression::contains(Expression exContained) const {
    if(m_tpType == List) {
      for(Expression exCheck : m_vecSubExpressions) {
	if(exCheck == exContained) {
	  return true;
	}
      }
    }
      
    return false;
  }
  
  bool Expression::isVariable() const {
    if(!this->isWildcard()) {
      if(this->type() == String) {
	std::string strString = this->get<std::string>();
	  
	if(strString.size() > 0) {
	  if(strString[0] == '?') {
	    return true;
	  }
	}
      }
    }
      
    return false;
  }

  bool Expression::isListOf(Type tpType) const {
    if(m_tpType == List) {
      bool bAllOfType = true;
	
      for(Expression exTest : m_vecSubExpressions) {
	if(exTest.type() != tpType) {
	  bAllOfType = false;
	  break;
	}
      }
	
      return bAllOfType;
    }
      
    return false;
  }
    
  bool Expression::appendToResolution(std::map<std::string, Expression>& mapResolutionA, std::map<std::string, Expression> const& mapResolutionB) {
    bool bSuccess = true;
      
    for(std::map<std::string, Expression>::const_iterator itPair = mapResolutionB.begin(); itPair != mapResolutionB.end(); ++itPair) {
      bSuccess = false;
	
      if(mapResolutionA.find(itPair->first) != mapResolutionA.end()) {
	if(mapResolutionA[itPair->first] == itPair->second) {
	  bSuccess = true;
	}
      } else {
	mapResolutionA[itPair->first] = itPair->second;
	bSuccess = true;
      }
	
      if(!bSuccess) {
	break;
      }
    }
      
    return bSuccess;
  }
    
  bool Expression::resolveExpressionsIntoResolution(std::map<std::string, Expression>& mapResolution, Expression const& exA, Expression const& exB, bool bExact) {
    bool bResolvedSub;
    std::map<std::string, Expression> mapResolutionSub = exA.resolve(exB, bResolvedSub, bExact);
      
    if(bResolvedSub) {
      if(Expression::appendToResolution(mapResolution, mapResolutionSub)) {
	return true;
      }
    }
      
    return false;
  }
    
  std::map<std::string, Expression> Expression::resolveLists(Expression const& exListA, Expression const& exListB, bool& bResolved, bool bExact) {
    std::map<std::string, Expression> mapResolution;
    bResolved = false;
      
    if(exListA.type() == List && exListB.type() == List) {
      if(bExact) {
	if(exListA.size() == exListB.size()) {
	  bResolved = true;
	    
	  for(unsigned int unI = 0; unI < exListA.size(); ++unI) {
	    bResolved = Expression::resolveExpressionsIntoResolution(mapResolution, exListA[unI], exListB[unI], bExact);
	      
	    if(!bResolved) {
	      break;
	    }
	  }
	}
      } else {
	bool bWCA = exListA.hasWildcard();
	bool bWCB = exListB.hasWildcard();
	bResolved = true;
	
	std::vector<Expression> vecA = exListA.subExpressions();
	std::vector<Expression> vecB = exListB.subExpressions();
	
	std::vector<Expression>::iterator itA = vecA.begin();
	std::vector<Expression>::iterator itB = vecB.begin();
	
	if(bWCA) {
	  bResolved = true;
	  
	  // B should have any items A has, in any order, or more
	  for(; itA != vecA.end(); ++itA) {
	    if(!itA->isWildcard()) {
	      bResolved = false;
	      
	      for(; itB != vecB.end(); ++itB) {
		if(!itB->isWildcard()) {
		  bResolved = Expression::resolveExpressionsIntoResolution(mapResolution, *itA, *itB);
		  
		  if(bResolved) {
		    // Remove item from B as it was `used up` to
		    // validate an item in A.
		    vecB.erase(itB);
		    break;
		  }
		}
	      }
	      
	      if(!bResolved) {
		if(bWCB) {
		  bResolved = true;
		} else {
		  break;
		}
	      }
	    }
	  }
	} else {
	  bResolved = true;
	  
	  // B should have all items A has, and not more (except for
	  // wildcards), in the exact same order
	  unsigned int unResolved = 0;
	  
	  for(; (itA != vecA.end()) && (itB != vecB.end()); ++itA, ++itB) {
	    while(itA != vecA.end() && itA->isWildcard()) {
	      ++itA;
	    }
	    
	    if(itA != vecA.end()) {
	      unResolved++;
	      
	      while(itB != vecB.end() && itB->isWildcard()) {
		++itB;
	      }
	      
	      if(itB != vecB.end()) {
		if(Expression::resolveExpressionsIntoResolution(mapResolution, *itA, *itB)) {
		  unResolved--;
		}
	      }
	    }
	    
	    if(itA == vecA.end()) {
	      --itA;
	    }
	    
	    if(itB == vecB.end()) {
	      --itB;
	    }
	  }
	  
	  while(itA != vecA.end() && itA->isWildcard()) {
	    itA++;
	  }
	  
	  while(itB != vecB.end() && itB->isWildcard()) {
	    itB++;
	  }
	  
	  bResolved = ((itA == vecA.end() || bWCB) && (itB == vecB.end() || bWCA) && ((unResolved == 0) || (bWCB && unResolved > 0)));
	}
      }
    }
      
    return mapResolution;
  }
  
  std::map<std::string, Expression> Expression::resolve(Expression const& exOther, bool& bResolved, bool bExact) const {
    std::map<std::string, Expression> mapResolution;
    bResolved = false;
      
    if(m_tpType == exOther.type()) {
      switch(m_tpType) {
      case String: {
	std::string strA = this->get<std::string>();
	std::string strB = exOther.get<std::string>();
	  
	if(bExact) {
	  if((this->isVariable() && exOther.isVariable()) ||
	     (strA == strB)) {
	    bResolved = true;
	  }
	} else {
	  if(strA.size() > 0 && strB.size() > 0) {
	    if(strA[0] == '?') {
	      if(strB[0] == '?') {
		bResolved = true;
		
		if(strA != "?_") {
		  mapResolution[strA] = exOther;
		}
		
		if(strA != strB && strB != "?_") {
		  mapResolution[strB] = *this;
		}
	      } else {
		bResolved = true;
		
		if(strA != "?_") {
		  mapResolution[strA] = exOther;
		}
	      }
	    } else {
	      if(strB[0] == '?') {
		bResolved = true;
		
		if(strB != "?_") {
		  mapResolution[strB] = *this;
		}
	      } else {
		if(strA == strB) {
		  bResolved = true;
		}
	      }
	    }
	  }
	}
      } break;
	  
      case Double: {
	bResolved = (this->get<double>() == exOther.get<double>());
      } break;
	  
      case Float: {
	bResolved = (this->get<float>() == exOther.get<float>());
      } break;
	  
      case Integer: {
	bResolved = (this->get<int>() == exOther.get<int>());
      } break;
	  
      case UnsignedInteger: {
	bResolved = (this->get<unsigned int>() == exOther.get<unsigned int>());
      } break;
	  
      case List: {
	mapResolution = Expression::resolveLists(*this, exOther, bResolved, bExact);
	  
	if(bResolved) {
	  std::map<std::string, Expression> mapResolution2 = Expression::resolveLists(exOther, *this, bResolved, bExact);
	    
	  if(bResolved) {
	    for(std::map<std::string, Expression>::iterator itR = mapResolution2.begin(); itR != mapResolution2.end(); ++itR) {
	      if(mapResolution.find(itR->first) != mapResolution.end()) {
		if(!(mapResolution[itR->first] == itR->second)) {
		  bResolved = false;
		  break;
		}
	      } else {
		mapResolution[itR->first] = itR->second;
	      }
	    }
	  }
	} break;
      }
      }
    } else if(m_tpType == String && !bExact) {
      std::string strUs = this->get<std::string>();
	
      if(strUs.size() > 0) {
	if(strUs[0] == '?') {
	  mapResolution[strUs] = exOther;
	  bResolved = true;
	}
      }
    } else if(exOther.type() == String && !bExact) {
      std::string strThem = exOther.get<std::string>();
	
      if(strThem.size() > 0) {
	if(strThem[0] == '?') {
	  mapResolution[strThem] = *this;
	  bResolved = true;
	}
      }
    }
      
    return mapResolution;
  }

  std::map<std::string, Expression> Expression::resolveTo(Expression const& exOther, bool& bResolved) const
  {
      std::map<std::string, Expression> retq = this->resolve(exOther, bResolved);

      if(bResolved)
      {
          std::vector<std::string> varNames;
          exOther.getVarNames(varNames);
          unsigned int maxK = varNames.size();
          for(unsigned int k = 0; k < maxK; k++)
              retq.erase(varNames[k]);
          Expression goal = this->parametrize(retq);
          goal.resolve(exOther, bResolved, true);
          if(!bResolved)
              retq.clear();
      }

      return retq;
  }

  bool Expression::matchEx(Expression const& exMatch, std::map<std::string, Expression>& mapResolution) const {
    bool bResolved;
    mapResolution = this->resolve(exMatch, bResolved);
      
    if(!bResolved) {
      mapResolution = {};
    }
      
    return bResolved;
  }
    
  bool Expression::match(std::string const& strMatchString, std::map<std::string, Expression>& mapResolution) const {
    Expression exMatch = Expression::parseSingle(strMatchString);
    return this->matchEx(exMatch, mapResolution);
  }
    
  bool Expression::operator==(Expression const& exOther) const {
    bool bResolved;
    std::map<std::string, Expression> mapResult = this->resolve(exOther, bResolved);
      
    return bResolved;
  }
    
  bool Expression::operator==(Expression::Type tpType) const {
    return m_tpType == tpType;
  }

  bool Expression::operator==(const char* arrcValue) const {
    typename Value<std::string>::PtrConst vlValue = std::dynamic_pointer_cast<Value<std::string> const>(m_vbValue);
      
    return (vlValue && vlValue->get() == std::string(arrcValue));
  }
  
  std::string Expression::predicateName() const {
    if(m_tpType == List && m_vecSubExpressions.size() > 0 && m_vecSubExpressions[0] == String) {
      return m_vecSubExpressions[0].get<std::string>();
    }
      
    return "";
  }
    
  void Expression::add(Expression const& exAdd) {
    if(m_tpType != List) {
      m_vecSubExpressions.push_back(*this);
      m_tpType = List;
    }
      
    m_vecSubExpressions.push_back(exAdd);
  }
    
  bool Expression::remove(unsigned int unIndex) {
    if(m_tpType == List) {
      if(m_vecSubExpressions.size() > unIndex) {
	std::vector<Expression>::iterator itSub = m_vecSubExpressions.begin();
	std::advance(itSub, unIndex);
	  
	m_vecSubExpressions.erase(itSub);
	  
	return true;
      }
    }
      
    return false;
  }
    
  Expression Expression::popFront() {
    if(m_tpType == List) {
      if(m_vecSubExpressions.size() > 0) {
	Expression exReturn = m_vecSubExpressions[0];
	m_vecSubExpressions.erase(m_vecSubExpressions.begin());
	  
	return exReturn;
      }
    }
      
    throw std::exception();
  }
    
  Expression Expression::negate() const {
    if(m_tpType == List) {
      if(this->size() == 2 && m_vecSubExpressions[0] == "not") {
	return m_vecSubExpressions[1];
      }
    }
      
    Expression exNot;
    exNot.add(Expression::parseString("not")[0]);
    exNot.add(*this);
      
    return exNot;
  }
    
  void Expression::pushFront(Expression exPush) {
    if(m_tpType != List) {
      m_vecSubExpressions.push_back(*this);
      m_tpType = List;
    }
      
    m_vecSubExpressions.insert(m_vecSubExpressions.begin(), exPush);
  }
    
  std::string Expression::stringify() const {
    std::stringstream sts;
      
    switch(m_tpType) {
    case Float: { sts << this->get<float>(); } break;
    case Double: { sts << this->get<double>(); } break;
    case Integer: { sts << this->get<int>(); } break;
    case UnsignedInteger: { sts << this->get<unsigned int>(); } break;
	
    case String: {
      if(this->isWildcard() || this->isVariable()) {
	sts << "?";
      } else {
	sts << this->get<std::string>();
      }
    } break;
	
    case List: {
      sts << "(";
	
      bool bFirst = true;
      for(Expression exSub : m_vecSubExpressions) {
	if(bFirst) {
	  bFirst = false;
	} else {
	  sts << " ";
	}
	  
	sts << exSub.stringify();
      }
	
      sts << ")";
    } break;
    }
      
    return sts.str();
  }
    
  size_t Expression::hash() const {
    return std::hash<std::string>()(this->stringify());
  }
  
  Expression Expression::sanitize(std::string strSuffix) {
    if(this->isVariable() || this->isWildcard()) {
      return Expression(this->get<std::string>() + strSuffix);
    } else if(m_vecSubExpressions.size() > 0) {
      std::vector<Expression> vecSanitized;
	
      for(Expression exSub : m_vecSubExpressions) {
	vecSanitized.push_back(exSub.sanitize(strSuffix));
      }
	
      return Expression(vecSanitized);
    }
      
    return *this;
  }
  
  bool Expression::isBound() const {
    return !(this->isVariable() || this->isWildcard());
  }
    
  bool Expression::isNumber() {
    return (m_tpType == Float) || (m_tpType == Double) || (m_tpType == Integer) || (m_tpType == UnsignedInteger);
  }
    
  unsigned int Expression::transformToUnsignedInteger(bool& bTransformed) {
    unsigned int unValue = 0;
      
    switch(m_tpType) {
    case String: {
      try {
	unValue = std::stoul(this->get<std::string>());
	bTransformed = true;
      } catch(const std::invalid_argument& ia) {
	bTransformed = false;
      }
    } break;
	
    case Float: {
      unValue = (unsigned int)this->get<float>();
      bTransformed = true;
    } break;
	
    case Double: {
      unValue = (unsigned int)this->get<double>();
      bTransformed = true;
    } break;
	
    case Integer: {
      unValue = (unsigned int)this->get<int>();
      bTransformed = true;
    } break;
	
    case UnsignedInteger: {
      unValue = this->get<unsigned int>();
      bTransformed = true;
    } break;
	
    default: {
      // For example List
      bTransformed = false;
    } break;
    }
      
    return unValue;
  }
    
  double Expression::transformToDouble(bool& bTransformed) {
    double dValue = 0;
      
    switch(m_tpType) {
    case String: {
      try {
	dValue = std::stod(this->get<std::string>());
	bTransformed = true;
      } catch(const std::invalid_argument& ia) {
	bTransformed = false;
      }
    } break;
	
    case Float: {
      dValue = (double)this->get<float>();
      bTransformed = true;
    } break;
	
    case Double: {
      dValue = this->get<double>();
      bTransformed = true;
    } break;
	
    case Integer: {
      dValue = (double)this->get<int>();
      bTransformed = true;
    } break;
	
    case UnsignedInteger: {
      dValue = (double)this->get<unsigned int>();
      bTransformed = true;
    } break;
	
    default: {
      // For example List
      bTransformed = false;
    } break;
    }
      
    return dValue;
  }
  
  void Expression::parseAssertStringValue(std::string strValue) {
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
  
  Expression Expression::conjunctionToList(void) const
  {
      unsigned int maxE = this->subExpressions().size();
      Expression retq;
      std::string aux = "(";
      if((maxE <= 1) || (this->subExpressions()[0].toString() != "and"))
          return Expression::parseString(aux + this->toString() + ")")[0];
      for(unsigned int e = 1; e < maxE; e++)
          retq.add(this->subExpressions()[e]);
      return retq;
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
