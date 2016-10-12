#pragma once
#ifndef __PROLOG_H__
#define __PROLOG_H__


#include <memory>
#include <iosfwd>
#include <map>
#include <vector>
#include <deque>
#include <functional>
#include <fstream>
#include <streambuf>
#include <unistd.h>

#include <gdaplanner/World.h>
#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>
#include <gdaplanner/Solution.h>


namespace gdaplanner {
  class SolutionsExhausted : std::exception {
  public:
    using std::exception::exception;
  };
  
  class Prolog {
  public:
    typedef std::shared_ptr<Prolog> Ptr;
    
    typedef std::function<Solution(Expression, Solution, Solution::Bindings)> LambdaPredicate;
    
  private:
    World::Ptr m_wdWorld;
    std::vector<LambdaPredicate> m_vecLambdaPredicates;
    
  protected:
  public:
    Prolog(World::Ptr wdWorld = nullptr);
    ~Prolog();
    
    World::Ptr world();
    Solution query(std::string const& strExpression, Solution const& solPrior = Solution(), World::Ptr wdWorld = nullptr);
    Solution queryEx(Expression const& exQuery, Solution const& solPrior = Solution(), World::Ptr wdWorld = nullptr);
    
    Solution unify(Expression const& exQuery, Solution solPrior = Solution(), Solution::Bindings const& bdgBindings = {});
    
    Solution matchLambdaPredicates(Expression const& exQuery, Solution const& solPrior, Solution::Bindings const& bdgBindings);
    void addLambdaPredicate(std::string const& strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda);
    void addSimpleLambdaPredicate(std::string const& strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda);
    void addLambdaPredicate(LambdaPredicate const& lpAdd);
    void addLazyListPredicate(std::string const& strPredicate, std::vector<Expression> const& vecList);
    void addCallbackPredicate(std::string const& strPredicate, std::function<Expression(unsigned int)> fncLambda);
    
    LambdaPredicate makeLambdaPredicate(std::string const& strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda);
    LambdaPredicate makeSimpleLambdaPredicate(std::string const& strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda);
    LambdaPredicate makeLazyListPredicate(std::string const& strPredicate, std::vector<Expression> const& vecList);
    LambdaPredicate makeCallbackPredicate(std::string const& strPredicate, std::function<Expression(unsigned int)> fncLambda);
    
    void addDefaultLambdaPredicates();
    
    template<class ... Args>
      void addPredicate(std::string const& strPredicate, std::string const& strArgument, Args ... args) {
      std::vector<std::string> vecArguments = {strArgument};
      this->addPredicate(strPredicate, vecArguments, std::forward<Args>(args)...);
    }
    
    template<class ... Args>
      void addPredicate(std::string const& strPredicate, std::vector<std::string> & vecArguments, std::string const& strArgument, Args ... args) {
      vecArguments.push_back(strArgument);
      this->addPredicate(strPredicate, vecArguments, std::forward<Args>(args)...);
    }
    
    void addPredicate(std::string const& strPredicate, std::vector<std::string> const& vecElements);
    
    void addPredicate(Expression const& exPredicate, std::vector<Expression> const& vecElements);
    
    bool loadFile(std::string strFilepath);
    
    std::string getProgramDirectory();
    bool loadStandardLibrary();
    
    template<class ... Args>
      static Prolog::Ptr create(Args ... args) {
      return std::make_shared<Prolog>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PROLOG_H__ */
