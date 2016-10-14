#pragma once
#ifndef __SOLUTION_H__
#define __SOLUTION_H__


#include <memory>
#include <iosfwd>
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
      Expression const& operator[](std::string strVariable) const{
	return m_mapBindings.find(strVariable)->second;
      }
      
      std::map<std::string, Expression>& bindings() {
	return m_mapBindings;
      }
      std::map<std::string, Expression>const& bindings() const{
	return m_mapBindings;
      }
      
      Bindings sanitize(std::string strSuffix);      

      bool bound(std::string strVariable) const {
	return m_mapBindings.find(strVariable) != m_mapBindings.end();
      }
      
      bool superImpose(Bindings bdgOther) {
	for(std::pair<std::string, Expression> prBinding : bdgOther.bindings()) {
	  if(this->bound(prBinding.first)) {
	    if(!((*this)[prBinding.first] == prBinding.second)) {
	      return false;
	    }
	  } else {
	    (*this)[prBinding.first] = prBinding.second;
	  }
	}
	
	return true;
      }
      
      Bindings desanitize(std::string strSuffix) {
	Bindings bdgDesanitized;
	
	for(std::pair<std::string, Expression> prBinding : m_mapBindings) {
	  if(prBinding.first.length() >= strSuffix.length()) {
	    if(prBinding.first.compare(prBinding.first.length() - strSuffix.length(), strSuffix.length(), strSuffix) == 0) {
	      bdgDesanitized[prBinding.first.substr(0, prBinding.first.length() - strSuffix.length())] = prBinding.second;
	    }
	  }
	}
	
	return bdgDesanitized;
      }
      
      virtual std::string toString() const override {
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
    unsigned int m_nextFactToTry;
    bool m_bValid;
    
  public:
    Solution() : m_nextFactToTry(0), m_bValid(true) {}
    ~Solution() {}
    
    virtual std::string toString() const override {
      std::stringstream sts;
      
      if(m_bValid) {
	sts << "Valid Solution" << std::endl;
    sts << m_bdgBindings.toString();
      
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
      } else {
	sts << "Invalid Solution" << std::endl;
      }
      
      return sts.str();
    }
    
    unsigned int& nextFactToTry(void)
    {
        return m_nextFactToTry;
    }
    unsigned int const& nextFactToTry(void) const
    {
        return m_nextFactToTry;
    }

    Bindings& bindings() {
        unsigned int maxK = m_vecSubSolutions.size();
        if(!maxK)
            return m_bdgBindings;
        else
            return m_vecSubSolutions[maxK - 1].bindings();
    }
    Bindings const& bindings() const {
        unsigned int maxK = m_vecSubSolutions.size();
        if(!maxK)
            return m_bdgBindings;
        else
            return m_vecSubSolutions[maxK - 1].bindings();
    }

    unsigned int indexCount() const {
      return m_vecIndices.size();
    }
    
    int& index(unsigned int unIndex = 0) {
      while(unIndex + 1 > m_vecIndices.size()) {
	m_vecIndices.push_back(-1);
      }
      
      return m_vecIndices[unIndex];
    }
    int const& index(unsigned int unIndex = 0) const{

      return m_vecIndices[unIndex];
    }

    void resetIndices(int nValue = -1) {
      std::fill(m_vecIndices.begin(), m_vecIndices.end(), nValue);
      
      if(m_vecIndices.size() > 0) {
	//std::cout << m_vecIndices[0] << std::endl;
      }
      for(std::vector<Solution>::iterator itSub = m_vecSubSolutions.begin();
	  itSub != m_vecSubSolutions.end(); ++itSub) {
	itSub->resetIndices(nValue);
      }
    }
    
    void addSubSolution(Solution solSubAdd) {
      m_vecSubSolutions.push_back(solSubAdd);
    }
    
    std::vector<Solution>& subSolutions() {
      return m_vecSubSolutions;
    }
    std::vector<Solution>const& subSolutions() const {
      return m_vecSubSolutions;
    }

    Solution& subSolution(unsigned int unIndex) {
      while(unIndex + 1 > m_vecSubSolutions.size()) {
	this->addSubSolution(Solution());
      }
      
      return m_vecSubSolutions[unIndex];
    }
    Solution const& subSolution(unsigned int unIndex) const{

      return m_vecSubSolutions[unIndex];
    }

    void setValid(bool bValid) {
      m_bValid = bValid;
    }
    
    bool valid() const {
      return m_bValid;
    }
    
    std::map<std::string, Expression> finalBindings() {
      if(m_vecSubSolutions.size() == 0) {
	return m_bdgBindings.bindings();
      } else {
	return m_vecSubSolutions.back().finalBindings();//.bindings();
      }
    }
    
    void setBindings(Bindings bdgSet);
    
    void clearBindings();
    
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
