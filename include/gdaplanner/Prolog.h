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
    Solution query(std::string strExpression, Solution solPrior, World::Ptr wdWorld = nullptr);
    Solution queryEx(Expression exQuery, Solution solPrior, World::Ptr wdWorld = nullptr);
    
    Solution unify(Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings = {});
    
    Solution matchLambdaPredicates(Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings);
    void addLambdaPredicate(std::string strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda);
    void addSimpleLambdaPredicate(std::string strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda);
    void addLambdaPredicate(LambdaPredicate lpAdd);
    void addLazyListPredicate(std::string strPredicate, std::vector<Expression> vecList);
    
    LambdaPredicate makeLambdaPredicate(std::string strPredicate, std::function<bool(std::map<std::string, Expression>)> fncLambda);
    LambdaPredicate makeSimpleLambdaPredicate(std::string strPredicate, std::function<void(std::map<std::string, Expression>)> fncLambda);
    Prolog::LambdaPredicate makeLazyListPredicate(std::string strPredicate, std::vector<Expression> vecList);
    
    void addDefaultLambdaPredicates();
    
    template<class ... Args>
      static Prolog::Ptr create(Args ... args) {
      return std::make_shared<Prolog>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PROLOG_H__ */
