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
    /** \brief PDDL planning context */
    class PDDL : public StatefulContext {
    public:
      typedef std::shared_ptr<PDDL> Ptr;
      
      typedef struct {
	Predicate::Ptr pdPredicate;  /**< Predicate identifying the Function's signature */
	std::string strType;         /**< Return type of this Function */
      } Function;
      
    private:
      std::string m_strDomain;
      std::vector<std::string> m_vecRequirements;
      std::vector<std::string> m_vecTypes;
      std::map<std::string, std::string> m_mapConstants;
      std::vector<Predicate::Ptr> m_vecPredicates;
      std::vector<Function> m_vecFunctions;
      
    protected:
    public:
      using StatefulContext::StatefulContext;
      
      /** \brief Set the domain of this context*/
      void domain(std::string strDomain);
      /** \brief Return the domain of this context */
      std::string domain();
      
      /** \brief Add a requirement for this context */
      void requirement(std::string strRequirement);
      /** \brief Return all requirements for this context */
      std::vector<std::string> requirements();
      /** \brief Check if a certain requirement is valid for this context */
      bool requires(std::string strRequirement);
      
      /** \brief Add a type for this context */
      void type(std::string strType);
      /** \brief Return all types for this context */
      std::vector<std::string> types();
      /** \brief Check if a certain type is available in this context */
      bool hasType(std::string strType);
      
      /** \brief Parse an expression, forming a predicate instance */
      Predicate::Ptr parsePredicate(Expression exPredicate);
      /** \brief Add an expression as a predicate to this context */
      bool addPredicate(Expression exPredicate);
      
      /** \brief Add a function to this context */
      bool addFunctions(Expression exFunctions);
      /** \brief Add an action to this context */
      bool addAction(Expression exAction);
      /** \brief Add a constant to this context */
      bool addConstants(Expression exConstants);
      
      /** \brief Return the type of a constant */
      std::string constantType(std::string strConstant);
      
      /** \brief Format stream output */
      virtual std::string toString() override;
      
      /** \brief Create a new instance of this class */
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __CONTEXTS_PDDL_H__ */
