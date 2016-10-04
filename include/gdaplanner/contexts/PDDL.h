#ifndef __CONTEXTS_PDDL_H__
#define __CONTEXTS_PDDL_H__


#include <memory>
#include <iostream>
#include <algorithm>

#include <gdaplanner/contexts/StatefulContext.h>
#include <gdaplanner/Predicate.h>
#include <gdaplanner/Action.h>


namespace gdaplanner {
  namespace contexts {
    class PDDL : public StatefulContext {
    public:
      typedef std::shared_ptr<PDDL> Ptr;
      
    private:
      std::string m_strDomain;
      std::vector<std::string> m_vecRequirements;
      std::vector<std::string> m_vecTypes;
      std::vector<Predicate::Ptr> m_vecPredicates;
      
    protected:
    public:
      using StatefulContext::StatefulContext;
      
      void domain(std::string strDomain);
      std::string domain();
      
      void requirement(std::string strRequirement);
      std::vector<std::string> requirements();
      bool requires(std::string strRequirement);
      
      void type(std::string strType);
      std::vector<std::string> types();
      bool hasType(std::string strType);
      
      Predicate::Ptr parsePredicate(Expression exPredicate);
      bool addPredicate(Expression exPredicate);
      
      bool addFunctions(Expression exFunctions);
      bool addAction(Expression exAction);
      
      virtual std::string toString() override;
      
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __CONTEXTS_PDDL_H__ */
