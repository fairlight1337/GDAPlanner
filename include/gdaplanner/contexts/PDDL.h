#ifndef __CONTEXTS_PDDL_H__
#define __CONTEXTS_PDDL_H__


#include <memory>
#include <iostream>
#include <algorithm>

#include <gdaplanner/contexts/StatefulContext.h>
#include <gdaplanner/Predicate.h>


namespace gdaplanner {
  namespace contexts {
    class PDDL : public StatefulContext {
    public:
      typedef std::shared_ptr<PDDL> Ptr;
      
    private:
      std::string m_strDomain;
      std::vector<std::string> m_vecRequirements;
      std::vector<std::string> m_vecTypes;
      std::vector<Predicate> m_vecPredicates;
      
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
      
      bool addPredicate(Expression exPredicate);
      
      virtual std::string toString() override;
      
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __CONTEXTS_PDDL_H__ */
