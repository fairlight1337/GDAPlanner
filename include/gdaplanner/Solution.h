#ifndef __SOLUTION_H__
#define __SOLUTION_H__


#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Solution : public Printable {
  public:
    typedef std::shared_ptr<Solution> Ptr;
    
    class Bindings : public Printable {
    private:
      std::map<std::string, Expression> m_mapBindings;
      
    public:
      Bindings(std::map<std::string, Expression> mapBindings = {}) : m_mapBindings(mapBindings) {}
      ~Bindings() {}
      
      Expression& operator[](std::string strVariable) {
	return m_mapBindings[strVariable];
      }
      
      std::map<std::string, Expression> bindings() {
	return m_mapBindings;
      }
      
      bool bound(std::string strVariable) {
	return m_mapBindings.find(strVariable) != m_mapBindings.end();
      }
      
      virtual std::string toString() override {
	std::stringstream sts;
	
	for(std::pair<std::string, Expression> prBinding : m_mapBindings) {
	  sts << prBinding.first << " = " << prBinding.second << std::endl;
	}
	
	return sts.str();
      }
    };
    
  private:
    std::vector<Solution> m_vecSubSolutions;
    std::vector<int> m_vecIndices;
    Bindings m_bdgBindings;
    bool m_bValid;
    
  public:
    Solution() : m_bValid(true) {}
    ~Solution() {}
    
    virtual std::string toString() override {
      std::stringstream sts;
      
      sts << "Solution" << std::endl;
      sts << m_bdgBindings;
      
      sts << "( ";
      for(int nIndex : m_vecIndices) {
	sts << nIndex << " ";
      }
      sts << ")" << std::endl;
      
      if(m_vecSubSolutions.size() > 0) {
	sts << "[" << std::endl;
	for(Solution solSub : m_vecSubSolutions) {
	  sts << solSub;
	}
	sts << "]" << std::endl;
      }
      
      return sts.str();
    }
    
    Bindings& bindings() {
      return m_bdgBindings;
    }
    
    int& index(unsigned int unIndex = 0) {
      while(unIndex + 1 > m_vecIndices.size()) {
	m_vecIndices.push_back(-1);
      }
      
      return m_vecIndices[unIndex];
    }
    
    void addSubSolution(Solution solSubAdd) {
      m_vecSubSolutions.push_back(solSubAdd);
    }
    
    Solution subSolution(unsigned int unIndex) {
      if(unIndex < m_vecSubSolutions.size()) {
	return m_vecSubSolutions[unIndex];
      } else {
	return Solution();
      }
    }
    
    void setValid(bool bValid) {
      m_bValid = bValid;
    }
    
    bool valid() {
      return m_bValid;
    }
    
    static Solution invalidSolution() {
      Solution solInvalid;
      solInvalid.setValid(false);
      
      return solInvalid;
    }
    
    template<class ... Args>
      static Solution::Ptr create(Args ... args) {
      return std::make_shared<Solution>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __SOLUTION_H__ */
