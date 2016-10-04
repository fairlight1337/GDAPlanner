#ifndef __PDDL_H__
#define __PDDL_H__


#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include <gdaplanner/problems/Problem.h>
#include <gdaplanner/Expression.h>


namespace gdaplanner {
  namespace problems {
    class PDDL : public Problem {
    public:
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
      
      void setIdentifier(std::string strIdentifier);
      void setDomain(std::string strDomain);
      void addObject(std::string strName, std::string strType);
      void addInitExpression(Expression exInit);
      void setInitExpressions(std::vector<Expression> vecInitExpressions);
      void setGoal(Expression exGoal);
      void setMetric(Expression exMetric);
      
      std::string identifier();
      std::string domain();
      std::vector<Object> objects();
      std::vector<Expression> initExpressions();
      Expression goal();
      Expression metric();
      
      virtual std::string toString() override;
      
      template<class ... Args>
	static PDDL::Ptr create(Args ... args) {
	return std::make_shared<PDDL>(std::forward<Args>(args)...);
      }
    };
  }
}


#endif /* __PDDL_H__ */
