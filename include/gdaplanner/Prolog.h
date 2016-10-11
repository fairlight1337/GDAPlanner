#ifndef __PROLOG_H__
#define __PROLOG_H__


#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include <functional>

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
    Solution query(std::string strExpression, Solution solPrior = Solution(), World::Ptr wdWorld = nullptr);
    Solution queryEx(Expression exQuery, Solution solPrior = Solution(), World::Ptr wdWorld = nullptr);
    
    Solution unify(Expression exQuery, Solution solPrior = Solution(), Solution::Bindings bdgBindings = {});
    
    Solution matchLambdaPredicates(Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings);
    void addLambdaPredicate(std::string strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda);
    void addSimpleLambdaPredicate(std::string strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda);
    void addLambdaPredicate(LambdaPredicate lpAdd);
    void addLazyListPredicate(std::string strPredicate, std::vector<Expression> vecList);
    void addCallbackPredicate(std::string strPredicate, std::function<Expression(unsigned int)> fncLambda);
    
    LambdaPredicate makeLambdaPredicate(std::string strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda);
    LambdaPredicate makeSimpleLambdaPredicate(std::string strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda);
    LambdaPredicate makeLazyListPredicate(std::string strPredicate, std::vector<Expression> vecList);
    LambdaPredicate makeCallbackPredicate(std::string strPredicate, std::function<Expression(unsigned int)> fncLambda);
    
    void addDefaultLambdaPredicates();
    
    template<class ... Args>
      void addPredicate(std::string strPredicate, std::string strArgument, Args ... args) {
      std::vector<std::string> vecArguments = {strArgument};
      this->addPredicate(strPredicate, vecArguments, std::forward<Args>(args)...);
    }
    
    template<class ... Args>
      void addPredicate(std::string strPredicate, std::vector<std::string> vecArguments, std::string strArgument, Args ... args) {
      vecArguments.push_back(strArgument);
      this->addPredicate(strPredicate, vecArguments, std::forward<Args>(args)...);
    }
    
    void addPredicate(std::string strPredicate, std::vector<std::string> vecElements);
    
    template<class ... Args>
      static Prolog::Ptr create(Args ... args) {
      return std::make_shared<Prolog>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PROLOG_H__ */
