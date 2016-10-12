#pragma once
#ifndef __CONTEXT_H__
#define __CONTEXT_H__


#include <memory>
#include <iosfwd>
#include <string>
#include <vector>
#include <deque>

#include <gdaplanner/Declaration.h>
#include <gdaplanner/Alias.h>
#include <gdaplanner/Action.h>
#include <gdaplanner/State.h>
#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


extern template class std::vector<gdaplanner::Expression>;


namespace gdaplanner {
  namespace contexts {
    class Context : public Printable {
    public:
      /** Shared pointer to Context class */
      typedef std::shared_ptr<Context> Ptr;
    
    private:
      std::string m_strIdentifier;
    
      std::vector<Declaration::Ptr> m_vecDeclarations;
      std::vector<Alias::Ptr> m_vecAliases;
      std::vector<State::Ptr> m_vecStates;
    
    protected:
      std::vector<Action::Ptr> m_vecActions;
    
    public:
      Context(std::string strIdentifier = "");
      ~Context();
    
      void declare(std::string strIdentifier, Expression expDeclare);
      void alias(Expression exAlias, std::vector<Expression> vecAliased);
      void action(Expression exPreconditions, Expression exEffects);
      void state(Expression exState, Expression exPreconditions);
    
      std::vector<State::Ptr> matchingStates(Expression exMatch);    
    
      virtual std::string toString() const override;
      
      unsigned int actionCount();
      Action::Ptr action(unsigned int unIndex);
      
    
      template<class ... Args>
	static Context::Ptr create(Args ... args) {
	return std::make_shared<Context>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __CONTEXT_H__ */
