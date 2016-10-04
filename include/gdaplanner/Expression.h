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
    /** Shared pointer to ValueBase class **/
    typedef std::shared_ptr<ValueBase> Ptr;
    
    ValueBase() {}
    virtual ~ValueBase() {}
  };
  
  template<typename T>
    class Value : public ValueBase {
  public:
    /** Shared pointer to Value class **/
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
  
  /** \brief Container class for holding arbitrary first-order logic expressions
      
      The Expression class can contain:
      
      * Other Expressions (as a list structure)
      * Atoms (currently everything defined in Expression::Type)
      
   */
  class Expression : public Printable {
  public:
    /** Shared pointer to Expression class **/
    typedef std::shared_ptr<Expression> Ptr;
    
    typedef enum {
      List = 0,            /**< A list, containing other Expression instances */
      String = 1,          /**< A zero-terminated character string; equivalent to \code{.cpp}std::string\endcode */
      Float = 2,           /**< A single-precision floating point number */
      Double = 3,          /**< A double-precision floating point number */
      Integer = 4,         /**< A signed integer */
      UnsignedInteger = 5  /**< An unsigned integer */
    } Type;
    
  private:
    Type m_tpType;
    ValueBase::Ptr m_vbValue = nullptr;
    std::vector<Expression> m_vecSubExpressions;
    
  protected:
  public:
    /** \brief Default constructor
	
	Initialized the Expression instance as an Expression::List. */
    Expression();
    /** \brief List constructor
	
	Initialized the Expression instance as an
	Expression::List. All elements from vecExpressions will be
	copiedinto the new instance as list elements.
    
	\param vecExpressions The elements to initialize the Expression instance with */
    Expression(std::vector<Expression> vecExpressions);
    /** \brief String constructor
	
	Initialized the Expression instance as an
	Expression::String. strExpression will be copied as the
	content string.
	
	\param strExpression String character sequence to use as content for the Expression instance */
    Expression(std::string strExpression);
    /** \brief Character array constructor
	
	Initialized the Expression instance as an
	Expression::String. arrcExpression will be copied as the
	content string. This is effectively mirroring the behavior of
	the string constructor. The character sequence given in
	arrcExpression must be zero-terminated. Failing to do so will
	result in undefined behavior.
	
	\param arrcExpression Character sequence to use as content for the Expression instance. This will be transformed into a std::string */
    Expression(const char* arrcExpression);
    /** \brief Double-precision floating point number constructor
	
	Initialized the Expression instance as an
	Expression::Double. dExpression will be copied as the
	content string.
	
	\param dExpression Double-precision floating point number to use as content for the Expression instance */
    explicit Expression(double dExpression);
    /** \brief Single-precision floating point number constructor
	
	Initialized the Expression instance as an
	Expression::Float. fExpression will be copied as the
	content string.
	
	\param fExpression Single-precision floating point number to use as content for the Expression instance */
    explicit Expression(float fExpression);
    /** \brief Signed integer constructor
	
	Initialized the Expression instance as an
	Expression::Integer. nExpression will be copied as the
	content string.
	
	\param nExpression Signed integer to use as content for the Expression instance */
    explicit Expression(int nExpression);
    /** \brief Unsigned integer constructor
	
	Initialized the Expression instance as an
	Expression::UnsignedInteger. unExpression will be copied as the
	content string.
	
	\param unExpression Unsigned integer to use as content for the Expression instance */
    explicit Expression(unsigned int unExpression);
    
    /** \brief Default destructor */
    ~Expression();
    
    /** \brief Returns the Expression::Type of this instance 

	\return Expression::Type of this instance */
    Type type() {
      return m_tpType;
    }
    
    /** \brief Returns the Value stored in this instance
	
	If the Expression instance is of a type other than
	Expression::List (e.g. it is an atom), this templated function
	will return the value stored in the atom. The template
	argument must match the stored value type. Refer to type() to
	validate the instance's contained value type.
	
	If the Expression instance is of type Expression::List or if
	the type of the contained Value doesn't match the template
	parameter, this function returns the default constructor of
	the template type.
	
	Usage example:
	\code{.cpp}
	Expression exValue("Hello World");
	std::cout << exValue.get<std::string>() << std::endl; // Will print `"Hello World"`
	std::cout << exValue.get<int>() << std::endl; // Will print `0`
	\endcode
	
	\return Value contained in the Expression instance, or T's default constructor */
    template<typename T>
      T get() {
      if(m_tpType != List) {
	typename Value<T>::Ptr vlValue = std::dynamic_pointer_cast<Value<T>>(m_vbValue);
	
	if(vlValue) {
	  return vlValue->get();
	}
      }
      
      return T();
    }
    
    /** \brief Returns the Expression's list length
	
	If this Expression instance is of type Expression::List,
	return its list length.
	
	\return Length of list stored in this Expression */
    unsigned int size() {
      return m_vecSubExpressions.size();
    }
    
    /** \brief Access individual elements in the list
	
	If this Expression instance is of type Expression::List,
	return its element at the position specified by unIndex.
	
	If the Expression is not of type Expression::List, the return
	value is undefined.
	
	\param unIndex Index of the element to return */
    Expression operator[](unsigned int unIndex) {
      return m_vecSubExpressions[unIndex];
    }
    
    /** \brief Return vector of this list's sub-expressions
	
	If this Expression instance is of type Expression::List,
	return its sub-expressions in order.
	
	If the Expression is not of type Expression::List, the return
	value is undefined.
	
	\return std::vector of Expression instances stored in this Expression instance */
    std::vector<Expression>& subExpressions() {
      return m_vecSubExpressions;
    }
    
    /** \brief Return a subset of sub-expressions from this list
	
	If this Expression instance is of type Expression::List,
	return a subset of its sub-expressions in order. The subset
	starts at index unStart of the original set of sub-expressions
	and has at most unLength elements. If either unStart or
	unLength are greater than the elements available in the
	original list of elements, less elements will be returned in
	the subset.
	
	Usage example:
	\code{.cpp}
	Expression exList({"Hello", "World", 1, 2, 3, 4.0});
	Expression exIntegers = exList.subSequence(2, 3); // Will contain the elements `{1, 2, 3}`.
	\endcode
	
	If the Expression is not of type Expression::List, the return
	value is undefined.
	
	\param unStart Index of the first element to include in the sub-set
	\param unLength Desired length of the sub-et
	
	\return Expression of type Expression::List holding the subset of Expression instances from the original list */
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
    
    /** \brief Parametrize an expression
	
	If an expression contains variables (starting with an `?`, not
	including the wildcard `?_`), this function will replace all
	their occurrences in the tree of Expression instances, based
	on the variable values given in mapParametrization.
	
	Usage example:
	\code{.cpp}
	Expression exPrototype = Expression::parseString("(an object ((type ?type) (color ?color) (size ?size)))")[0];
	Expression exInstance = exPrototype.parametrize({{"?type", "Cup"}, {"?color", "red"}});
	
	std::cout << exInstance << std::endl; // Will print `(an object ((type Cup) (color red) (size ?size)))`
	\endcode
	
	If variables are defined in the Expression that are not
	defined in the mapParametrization map, their original symbol
	is returned. If variables are defined in the map that are not
	part of the original Expression, their value is ignored.
	
	The replacement can be whole Expression instances:
	\code{.cpp}
	Expression exPrototype = Expression::parseString("(an action-sequence ?do))")[0];
	Expression exDo = Expression::parseString("((pick ?object) (place ?object ?location))")[0];
	Expression exInstance = exPrototype.parametrize({{"?do", exDo}});
	
	std::cout << exInstance << std::endl; // Will print `(an action-sequence ((pick ?object) (place ?object ?location)))
	\endcode
	
	\param mapParametrization The set of variables to replace in the original Expression
	
	\return Expression instance obeying the above formulation */
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
    
    /** \brief Specifies how this class is printed to output streams */
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
    
    /** \brief Check if this instance is a wildcard
	
	Wildcards are first-order logic elements denoted as `?_`. This
	function returns a boolean value denoting whether this
	instance is such a wildcard.
	
	\return Boolean value denoting whether this instance is a wildcard */
    bool isWildcard() {
      return (m_tpType == String && *this == "?_");
    }
    
    /** \brief Check if this list includes a wildcard
	
	If this Expression instance is of type Expression::List,
	return a boolean value denoting whether it contains a wildcard
	element.
	
	\return Boolean value denoting whether this list contains a wildcard */
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
    
    /** \brief Check if this instance is a variable
	
	Variables are first-order logic elements starting with a `?`
	(not including the wildcard element being `?_`). This function
	returns a boolean value denoting whether this instance is such
	a variable.
	
	\return Boolean value denoting whether this instance is a variable */
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
    
    /** \brief Check if this instance is of type Expression::List
	
	\return Boolean value denoting whether this instance is of type Expression::List */
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
    
    /** \brief Resolves this instance against another instance
	
	When resolving, two things happen:
	
	* Two instances of type Expression are checked for structural equality
	
	* When structurally equal, all variables are resolved against their counterpart
	
	Usage example:
	\code{.cpp}
	Expression exPrototype = Expression::parseString("(place-object ?object ?location)")[0];
	
	Expression exSituationA = Expression::parseString("(place-object Cup Table)")[0];
	std::map<std::string, Expression> mapResolutionA = exPrototype.resolve(exSituationA);
	// mapResolutionA is now `{{"?object", "Cup"}, {"?location", "Table"}}`
	
	Expression exSituationB = Expression::parseString("(place-object (an object) Table)")[0];
	std::map<std::string, Expression> mapResolutionB = exPrototype.resolve(exSituationA);
	// mapResolutionA is now `{{"?object", Expression<"(an object)">}, {"?location", "Table"}}`
	\endcode
	
	Variables on both sides:
	\code{.cpp}
	Expression exProtoA = Expression::parseString("(predicate ?a 2)")[0];
	Expression exProtoB = Expression::parseString("(predicate 1 ?b)")[0];
	
	std::map<std::string, Expression> mapResolutionA = exProtoA.resolve(exProtoB);
	// mapResolutionA is now `{{"?a", 1}, {"?b", 2}}`
	\endcode
	
	After calling the function, the parameter bResolved passed by
	reference denotes whether the two terms are structurally
	equivalent and could be resolved.
	
	\param exOther The Expression instance to resolve against
	\param bResolved Boolean flag denoting whether the resolution was successful
	
	\return Map of variable to value bindings */
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
    
    Expression negate() {
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
