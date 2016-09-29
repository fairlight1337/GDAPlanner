#ifndef __CONTEXT_H__
#define __CONTEXT_H__


#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include <gdaplanner/Declaration.h>
#include <gdaplanner/Alias.h>
#include <gdaplanner/Action.h>
#include <gdaplanner/State.h>
#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Context : public Printable {
  public:
    typedef std::shared_ptr<Context> Ptr;
    
  private:
    std::string m_strIdentifier;
    
    std::vector<Declaration::Ptr> m_vecDeclarations;
    std::vector<Alias::Ptr> m_vecAliases;
    std::vector<Action::Ptr> m_vecActions;
    std::vector<State::Ptr> m_vecStates;
    
  protected:
  public:
    Context(std::string strIdentifier = "");
    ~Context();
    
    void declare(std::string strIdentifier, Expression expDeclare);
    void alias(Expression exAlias, std::vector<Expression> vecAliased);
    void action(Expression exAction, Expression exPreconditions, Expression exEffects);
    void state(Expression exState, Expression exPreconditions);
    
    std::vector<State::Ptr> matchingStates(Expression exMatch);    
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static Context::Ptr create(Args ... args) {
      return std::make_shared<Context>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __CONTEXT_H__ */
