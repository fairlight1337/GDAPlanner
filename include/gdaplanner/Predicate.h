#ifndef __PREDICATE_H__
#define __PREDICATE_H__


#include <memory>
#include <iostream>
#include <algorithm>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Predicate : public Printable {
  public:
    typedef std::shared_ptr<Predicate> Ptr;
    
  private:
    Expression m_exExpression;
    std::map<std::string, std::string> m_mapTypes;
    
    static std::vector<std::string> variables(Expression& exExpression);
    
  protected:
  public:
    Predicate(Expression exExpression);
    ~Predicate();
    
    Expression& expression();
    std::string& type(std::string strVariable);
    void setType(std::string strVariable, std::string strType);
    bool isTyped(std::string strVariable);
    std::vector<std::string> variables();
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static Predicate::Ptr create(Args ... args) {
      return std::make_shared<Predicate>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PREDICATE_H__ */
