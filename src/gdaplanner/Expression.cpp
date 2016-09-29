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
}
