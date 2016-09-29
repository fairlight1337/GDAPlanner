#ifndef __ALIAS_H__
#define __ALIAS_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Alias : public Printable {
  public:
    typedef std::shared_ptr<Alias> Ptr;
    
  private:
    Expression m_exAlias;
    std::vector<Expression> m_vecAliased;
    
  protected:
  public:
    Alias(Expression exAlias, std::vector<Expression> vecAliased);
    ~Alias();
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static Alias::Ptr create(Args ... args) {
      return std::make_shared<Alias>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __ALIAS_H__ */
