#pragma once
#ifndef __PDDL_H__
#define __PDDL_H__


#include <memory>
#include <iosfwd>
#include <string>
#include <vector>

#include <gdaplanner/problems/Problem.h>
#include <gdaplanner/Expression.h>


extern template class std::vector<gdaplanner::Expression>;


namespace gdaplanner {
  namespace problems {
    /** \brief PDDL planning problem definition */
    class PDDL : public Problem {
    public:
      /** Shared pointer to PDDL class */
      typedef std::shared_ptr<PDDL> Ptr;
      
      typedef struct {
	std::string strName;
	std::string strType;
      } Object;
      
    private:
      std::string m_strIdentifier;
      std::string m_strDomain;
      std::vector<Object> m_vecObjects;
      std::vector<Expression> m_vecInit;
      Expression m_exGoal;
      Expression m_exMetric;
      
    protected:
    public:
      PDDL();
      ~PDDL();
      
      /** \brief Set problem identifier */
      void setIdentifier(std::string strIdentifier);
      /** \brief Set domain name */
      void setDomain(std::string strDomain);
      /** \brief Add an object */
      void addObject(std::string strName, std::string strType);
      /** \brief Add an initialization expression */
      void addInitExpression(Expression exInit);
      /** \brief Set the list of initialization expressions */
      void setInitExpressions(std::vector<Expression> vecInitExpressions);
      /** \brief Set this problem's goal */
      void setGoal(Expression exGoal);
      /** \brief Set this problem's metric */
      void setMetric(Expression exMetric);
      
      /** \brief Return problem identifier */
      std::string identifier();
      /** \brief Return problem domain */
      std::string domain();
      /** \brief Return list of objects */
      std::vector<Object>& objects();
      std::vector<Object>const& objects() const;
      /** \brief Return list of initialization expressions */
      std::vector<Expression> initExpressions();
      /** \brief Return this problem's goal */
      Expression goal();
      /** \brief Return this problem's metric */
      Expression metric();
      
      std::string objectType(std::string const strName) const;
      
      /** \brief Format stream output */
      virtual std::string toString() const override;
      
      /** \brief Create a new instance of this class */
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __PDDL_H__ */
