#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__


#include <memory>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>

#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class ValueBase {
  public:
    typedef std::shared_ptr<ValueBase> Ptr;
    
    ValueBase() {}
    virtual ~ValueBase() {}
  };
  
  template<typename T>
    class Value : public ValueBase {
  public:
    typedef std::shared_ptr<Value<T>> Ptr;
    
  private:
    T m_tValue;
    
  public:
    Value(T tValue) : m_tValue(tValue) {}
    ~Value() {}
    
    T get() { return m_tValue; }
    T operator*() { return this->get(); }
    
    template<class ... Args>
      static Value::Ptr create(Args ... args) {
      return std::make_shared<Value<T>>(std::forward<Args>(args)...);
    }
  };
  
  class Expression : public Printable {
  public:
    typedef std::shared_ptr<Expression> Ptr;
    
    typedef enum {
      List = 0,
      String = 1,
      Double = 2,
      Float = 3,
      Integer = 4,
      UnsignedInteger = 5
    } Type;
    
  private:
    Type m_tpType;
    ValueBase::Ptr m_vbValue = nullptr;
    std::vector<Expression> m_vecSubExpressions;
    
  protected:
  public:
    Expression();
    Expression(std::vector<Expression> vecExpressions);
    Expression(std::string strExpression);
    Expression(const char* arrcExpression);
    explicit Expression(double dExpression);
    explicit Expression(float fExpression);
    explicit Expression(int nExpression);
    explicit Expression(unsigned int unExpression);
    
    ~Expression();
    
    Type type() {
      return m_tpType;
    }
    
    template<typename T>
      T get() {
      typename Value<T>::Ptr vlValue = std::dynamic_pointer_cast<Value<T>>(m_vbValue);
      
      if(vlValue) {
	return vlValue->get();
      } else {
	return T();
      }
    }
    
    unsigned int size() {
      return m_vecSubExpressions.size();
    }
    
    Expression operator[](unsigned int unIndex) {
      return m_vecSubExpressions[unIndex];
    }
    
    std::vector<Expression>& subExpressions() {
      return m_vecSubExpressions;
    }
    
    std::vector<Expression> subSequence(unsigned int unStart, unsigned int unLength = 0) {
      std::vector<Expression> vecSequence;
      
      if(unLength == 0) {
	unLength = m_vecSubExpressions.size() - unStart;
      }
      
      for(unsigned int unI = unStart; unI < unStart + unLength; ++unI) {
	vecSequence.push_back(m_vecSubExpressions[unI]);
      }
      
      return vecSequence;
    }
    
    Expression parametrize(std::map<std::string, Expression> mapParametrization) {
      switch(m_tpType) {
      case String: {
	std::string strUs = this->get<std::string>();
	
	if(strUs.size() > 0) {
	  if(strUs[0] == '?') {
	    if(mapParametrization.find(strUs) != mapParametrization.end()) {
	      return mapParametrization[strUs];
	    }
	  }
	}
      } break;
	
      case List: {
	std::vector<Expression> vecSubNew;
	
	for(Expression exSub : m_vecSubExpressions) {
	  vecSubNew.push_back(exSub.parametrize(mapParametrization));
	}
	
	return Expression(vecSubNew);
      } break;
	
      default: {
      } break;
      }
      
      return *this;
    }
    
    virtual std::string toString() override {
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
    
    bool isWildcard() {
      return (m_tpType == String && *this == "?_");
    }
    
    bool hasWildcard() {
      if(m_tpType == List) {
	for(Expression exTest : m_vecSubExpressions) {
	  if(exTest.isWildcard()) {
	    return true;
	  }
	}
      }
      
      return false;
    }
    
    bool isVariable() {
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
    
    bool isListOf(Type tpType) {
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
    
    static bool appendToResolution(std::map<std::string, Expression>& mapResolutionA, std::map<std::string, Expression>& mapResolutionB) {
      bool bSuccess = true;
      
      for(std::map<std::string, Expression>::iterator itPair = mapResolutionB.begin(); itPair != mapResolutionB.end(); ++itPair) {
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
    
    static bool resolveExpressionsIntoResolution(std::map<std::string, Expression>& mapResolution, Expression exA, Expression exB) {
      bool bResolvedSub;
      std::map<std::string, Expression> mapResolutionSub = exA.resolve(exB, bResolvedSub);
      
      if(bResolvedSub) {
	if(Expression::appendToResolution(mapResolution, mapResolutionSub)) {
	  return true;
	}
      }
      
      return false;
    }
    
    static std::map<std::string, Expression> resolveLists(Expression& exListA, Expression& exListB, bool& bResolved) {
      std::map<std::string, Expression> mapResolution;
      bResolved = false;
      
      if(exListA.type() == List && exListB.type() == List) {
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
      
      return mapResolution;
    }
    
    std::map<std::string, Expression> resolve(Expression& exOther, bool& bResolved) {
      std::map<std::string, Expression> mapResolution;
      bResolved = false;
      
      if(m_tpType == exOther.type()) {
	bool bAllEqual = true;
	
	switch(m_tpType) {
	case String: {
	  std::string strA = this->get<std::string>();
	  std::string strB = exOther.get<std::string>();
	  
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
	  mapResolution = Expression::resolveLists(*this, exOther, bResolved);
	  
	  if(bResolved) {
	    std::map<std::string, Expression> mapResolution2 = Expression::resolveLists(exOther, *this, bResolved);
	    
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
	  }
	} break;
	}
      } else if(m_tpType == String) {
	std::string strUs = this->get<std::string>();
	
	if(strUs.size() > 0) {
	  if(strUs[0] == '?') {
	    mapResolution[strUs] = exOther;
	    bResolved = true;
	  }
	}
      } else if(exOther.type() == String) {
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
    
    bool operator==(Expression& exOther) {
      bool bResolved;
      std::map<std::string, Expression> mapResult = this->resolve(exOther, bResolved);
      
      return bResolved;
    }
    
    bool operator==(const char* arrcValue) {
      typename Value<std::string>::Ptr vlValue = std::dynamic_pointer_cast<Value<std::string>>(m_vbValue);
      
      return (vlValue && vlValue->get() == std::string(arrcValue));
    }
    
    template<typename T>
      bool operator==(T& tValue) {
      typename Value<T>::Ptr vlValue = std::dynamic_pointer_cast<Value<T>>(m_vbValue);
      
      return (vlValue && vlValue->get() == tValue);
    }
    
    void add(Expression exAdd) {
      if(m_tpType != List) {
	m_vecSubExpressions.push_back(*this);
	m_tpType = List;
      }
      
      m_vecSubExpressions.push_back(exAdd);
    }
    
    Expression popFront() {
      if(m_tpType == List) {
	if(m_vecSubExpressions.size() > 0) {
	  Expression exReturn = m_vecSubExpressions[0];
	  m_vecSubExpressions.erase(m_vecSubExpressions.begin());
	  
	  return exReturn;
	}
      }
      
      throw std::exception();
    }
    
    void pushFront(Expression exPush) {
      if(m_tpType != List) {
	m_vecSubExpressions.push_back(*this);
	m_tpType = List;
      }
      
      m_vecSubExpressions.insert(m_vecSubExpressions.begin(), exPush);
    }
    
    static std::vector<Expression> parseString(std::string strSource);
    static std::vector<Expression> parseString(std::string& strSource, unsigned int& unPos);
    
    template<class ... Args>
      static Expression::Ptr create(Args ... args) {
      return std::make_shared<Expression>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __EXPRESSION_H__ */
