#ifndef __DECLARATION_H__
#define __DECLARATION_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Declaration : public Printable {
  public:
    /** Shared pointer to Declaration class */
    typedef std::shared_ptr<Declaration> Ptr;
    
  private:
    std::string m_strIdentifier;
    Expression m_expDeclare;
    
  protected:
  public:
    Declaration(std::string strIdentifier, Expression expDeclare);
    ~Declaration();
    
    virtual std::string toString() const override;
    
    template<class ... Args>
      static Declaration::Ptr create(Args ... args) {
      return std::make_shared<Declaration>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __DECLARATION_H__ */
