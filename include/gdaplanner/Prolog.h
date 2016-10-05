#ifndef __PROLOG_H__
#define __PROLOG_H__


#include <memory>
#include <iostream>
#include <map>
#include <vector>

#include <gdaplanner/World.h>
#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Prolog {
  public:
    typedef std::shared_ptr<Prolog> Ptr;
    
    class Solution : public Printable {
    public:
      typedef std::shared_ptr<Solution> Ptr;
      
    private:
      std::map<std::string, Expression> m_mapBindings;
      std::vector<Solution::Ptr> m_vecSubSolutions;
      std::vector<unsigned int> m_vecIndex;
      
    public:
      Solution();
      ~Solution();
      
      virtual std::string toString() override;
      
      bool isBound(std::string strVariable);
      bool isBoundEx(Expression exVariable);
      std::vector<std::string> boundVariables();
      Expression value(std::string strVariable);
      std::map<std::string, Expression> bindings(bool bRecursive = true);
      
      void setIndex(unsigned int unPosition, unsigned int unIndex) {
	if(m_vecIndex.size() < unPosition + 1) {
	  m_vecIndex.resize(unPosition + 1);
	}
	
	m_vecIndex[unPosition] = unIndex;
      }
      
      unsigned int index(unsigned int unPosition) {
	return m_vecIndex[unPosition];
      }
      
      Solution::Ptr pushSubSolution() {
	Solution::Ptr solNew = Solution::create();
	solNew->m_mapBindings = this->bindings();
	
	this->addSubSolution(solNew);
	
	return solNew;
      }
      
      void addSubSolution(Solution::Ptr solSub) {
	m_vecSubSolutions.push_back(solSub);
        
      }
      
      Solution::Ptr subSolution(unsigned int unIndex) {
	return m_vecSubSolutions[unIndex];
      }
      
      Expression& operator[](std::string strVariable) {
	return m_mapBindings[strVariable];
      }
      
      template<class ... Args>
	static Solution::Ptr create(Args ... args) {
	return std::make_shared<Solution>(std::forward<Args>(args)...);
      }
    };
    
  private:
    World::Ptr m_wdWorld;
    
  protected:
  public:
    Prolog(World::Ptr wdWorld = nullptr);
    ~Prolog();
    
    World::Ptr world();
    Solution::Ptr query(std::string strExpression, Solution::Ptr solPrior = nullptr, World::Ptr wdWorld = nullptr);
    Solution::Ptr queryEx(Expression exQuery, Solution::Ptr solPrior = nullptr, World::Ptr wdWorld = nullptr);
    
    bool evaluateBuiltinFunction(Expression exQuery, Solution::Ptr solSolution, Solution::Ptr solPrior);
    bool populateSolution(Expression exQuery, Solution::Ptr solSolution, Solution::Ptr solPrior);
    
    template<class ... Args>
      static Prolog::Ptr create(Args ... args) {
      return std::make_shared<Prolog>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PROLOG_H__ */
