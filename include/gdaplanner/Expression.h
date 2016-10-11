#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__


#include <memory>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <functional>

#include <gdaplanner/Printable.h>


namespace gdaplanner {
  /** \brief Base class for storing values anonymously
      
      This class represents an anonymous pointer for polymorphic
      instances of type Value. This class offers a pointer that can be
      stored such that the original template argument of the Value
      class doesn't need to be stored. Before using, this pointer must
      be cast into the appropriate Value<T> class.
      
      \since 0.1.0 */
  class ValueBase {
  public:
    /** Shared pointer to ValueBase class */
    typedef std::shared_ptr<ValueBase> Ptr;
    
    ValueBase() {}
    virtual ~ValueBase() {}
  };
  
  /** \brief Anonymous value holding class
      
      Templated with the type T, instances of this class can hold any
      data type. This is useful if the data type is not known in
      advance, and needs to be stored type-agnostically.
      
      \since 0.1.0 */
  template<typename T>
    class Value : public ValueBase {
  public:
    /** Shared pointer to Value class */
    typedef std::shared_ptr<Value<T>> Ptr;
    typedef std::shared_ptr<Value<T> const> PtrConst;

  private:
    /** \brief The data to store
     
	The data type is based on the class's template parameter T.
    
	\since 0.1.0 */
    T m_tValue;
    
  public:
    Value(T tValue) : m_tValue(tValue) {}
    ~Value() {}
    
    /** \brief Return the currently stored data */
    T get() const { return m_tValue; }
    /** \brief Equivalent to get() */
    T operator*() const { return this->get(); }
    /** \brief Return the currently stored data */
    T get() { return m_tValue; }
    /** \brief Equivalent to get() */
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
      
      \since 0.1.0 */
  class Expression : public Printable {
  public:
    /** Shared pointer to Expression class */
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
    
    bool parseAssertStringValue(std::string strValue);
    
    /** \brief Returns the Expression::Type of this instance 

	\return Expression::Type of this instance */
    Type type() const {
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
      T get() const {
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
    unsigned int size() const {
      return m_vecSubExpressions.size();
    }

    /** \brief Returns a vector containing names of the variables in the expression

    List all variables occurring anywhere in the expression. Variable names begin
    with '?'. The returned vector contains no duplicates. The wildcard variable,
    '?_', is not returned.

    Any content existing in the varNames parameter at the moment the function is
    called is NOT erased.

    \return Nothing*/
    void getVarNames(std::vector<std::string> & varNames) const;

    /** \brief Used to test if an exact copy of an expression occurs anywhere inside the
     * current one.

    Tests all the expression tree. Matching must be exact, no resolution is performed.

    \return bool indicating whether some subexpression exactly matches the given one*/
    bool hasSubExpression(Expression const& what) const;
    
    /** \brief Access individual elements in the list
	
	If this Expression instance is of type Expression::List,
	return its element at the position specified by unIndex.
	
	If the Expression is not of type Expression::List, the return
	value is undefined.
	
	\param unIndex Index of the element to return */
    Expression& operator[](unsigned int unIndex) {
      return m_vecSubExpressions[unIndex];
    }
    Expression const& operator[](unsigned int unIndex) const {
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
    
    std::vector<Expression>const& subExpressions() const{
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
    std::vector<Expression> subSequence(unsigned int unStart, unsigned int unLength = 0) const {
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
    Expression parametrize(std::map<std::string, Expression> const& mapParametrization) const {
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
    
    /** \brief Specifies how this class is printed to output streams */
    virtual std::string toString() const override {
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
    bool isWildcard() const {
      return (m_tpType == String && *this == "?_");
    }
    
    /** \brief Check if this list includes a wildcard
	
	If this Expression instance is of type Expression::List,
	return a boolean value denoting whether it contains a wildcard
	element.
	
	\return Boolean value denoting whether this list contains a wildcard */
    bool hasWildcard() const {
      if(m_tpType == List) {
	for(Expression exTest : m_vecSubExpressions) {
	  if(exTest.isWildcard()) {
	    return true;
	  }
	}
      }
      
      return false;
    }
    
    bool contains(Expression exContained) const {
      if(m_tpType == List) {
	for(Expression exCheck : m_vecSubExpressions) {
	  if(exCheck == exContained) {
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
    bool isVariable() const {
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
    bool isListOf(Type tpType) const {
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
    
    static bool appendToResolution(std::map<std::string, Expression>& mapResolutionA, std::map<std::string, Expression> const& mapResolutionB) {
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
    
    static bool resolveExpressionsIntoResolution(std::map<std::string, Expression>& mapResolution, Expression const& exA, Expression const& exB, bool bExact = false) {
      bool bResolvedSub;
      std::map<std::string, Expression> mapResolutionSub = exA.resolve(exB, bResolvedSub, bExact);
      
      if(bResolvedSub) {
	if(Expression::appendToResolution(mapResolution, mapResolutionSub)) {
	  return true;
	}
      }
      
      return false;
    }
    
    static std::map<std::string, Expression> resolveLists(Expression const& exListA, Expression const& exListB, bool& bResolved, bool bExact = false) {
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
    std::map<std::string, Expression> resolve(Expression const& exOther, bool& bResolved, bool bExact = false) const {
      std::map<std::string, Expression> mapResolution;
      bResolved = false;
      
      if(m_tpType == exOther.type()) {
	bool bAllEqual = true;
	
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
    
    bool matchEx(Expression const& exMatch, std::map<std::string, Expression>& mapResolution) const {
      bool bResolved;
      mapResolution = this->resolve(exMatch, bResolved);
      
      if(!bResolved) {
	mapResolution = {};
      }
      
      return bResolved;
    }
    
    bool match(std::string const& strMatchString, std::map<std::string, Expression>& mapResolution) const {
      Expression exMatch = Expression::parseSingle(strMatchString);
      return this->matchEx(exMatch, mapResolution);
    }
    
    bool operator==(Expression const& exOther) const {
      bool bResolved;
      std::map<std::string, Expression> mapResult = this->resolve(exOther, bResolved);
      
      return bResolved;
    }
    
    bool operator==(Expression::Type tpType) const {
      return m_tpType == tpType;
    }

    bool operator==(const char* arrcValue) const {
      typename Value<std::string>::PtrConst vlValue = std::dynamic_pointer_cast<Value<std::string> const>(m_vbValue);
      
      return (vlValue && vlValue->get() == std::string(arrcValue));
    }
    
    template<typename T>
      bool operator==(T const& tValue) const {
      typename Value<T>::PtrConst vlValue = std::dynamic_pointer_cast<Value<T> const>(m_vbValue);
      
      return (vlValue && vlValue->get() == tValue);
    }
    
    std::string predicateName() const {
      if(m_tpType == List && m_vecSubExpressions.size() > 0 && m_vecSubExpressions[0] == String) {
	return m_vecSubExpressions[0].get<std::string>();
      }
      
      return "";
    }
    
    void add(Expression const& exAdd) {
      if(m_tpType != List) {
	m_vecSubExpressions.push_back(*this);
	m_tpType = List;
      }
      
      m_vecSubExpressions.push_back(exAdd);
    }
    
    bool remove(unsigned int unIndex) {
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
    
    Expression negate() const {
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
    
    std::string stringify() const {
    /** \brief Transforms this Expression into a string
	
	The contents of this Expression and all sub-Expressions will
	be formatted into a proper string.
	
	\brief String denoting the contents of this Expression */
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
    
    size_t hash() const {
    /** \brief Generates a hash value of this Expression instance
	
	\return Hash value for this instance */
      return std::hash<std::string>()(this->stringify());
    }
    
    static std::vector<Expression> parseString(std::string strSource);
    static std::vector<Expression> parseString(std::string& strSource, unsigned int& unPos);
    
    static Expression parseSingle(std::string strSource);
    
    template<class ... Args>
      static Expression::Ptr create(Args ... args) {
      return std::make_shared<Expression>(std::forward<Args>(args)...);
    }
    
    /** \brief Sanitizes all variables recursively
	
	Adds the string strSuffix to all variable names and wildcards
	in this Expression and all sub-Expressions. The main use-case
	for this is the `(= ?a ?b)` Prolog predicate such that
	arbitrary values can be mapped against each other. Without
	this mechanism, the intermediate working variables in the
	Prolog resolver would get inserted into the final bindings of
	the predicate. By sanitizing the original variables, the
	non-sanitized working variables can get filtered out.
	
	For reverting the process on the final bindings in a solution,
	look at Solution::Bindings::desanitize().
	
	\param strSuffix The suffix to append to all variable names and wildcards
	
	\return Sanitized copy of the current Expression */
    Expression sanitize(std::string strSuffix) {
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
    
    /** \brief Check whether this expression is bound to an actual value
        
        Actual values are everything apart from variables and
        wildcards.
	
        \return Boolean value denoting whether this Expression is bound */
    bool isBound() {
      return !(this->isVariable() || this->isWildcard());
    }
    
    bool isNumber() {
      return m_tpType == Float || m_tpType == Double || m_tpType == Integer | m_tpType == UnsignedInteger;
    }
    
    unsigned int transformToUnsignedInteger(bool& bTransformed) {
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
    
    double transformToDouble(bool& bTransformed) {
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
  };
}


namespace std {
  template<> struct hash<gdaplanner::Expression> {
    size_t operator()(gdaplanner::Expression const& exHash) const {
      return ((gdaplanner::Expression)exHash).hash();
    }
  };
}


#endif /* __EXPRESSION_H__ */
