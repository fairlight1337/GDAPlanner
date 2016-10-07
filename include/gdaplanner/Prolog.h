#ifndef __PROLOG_H__
#define __PROLOG_H__


#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <deque>

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
    
  private:
    World::Ptr m_wdWorld;
    
  protected:
  public:
    Prolog(World::Ptr wdWorld = nullptr);
    ~Prolog();
    
    World::Ptr world();
    Solution query(std::string strExpression, Solution solPrior, World::Ptr wdWorld = nullptr);
    Solution queryEx(Expression exQuery, Solution solPrior, World::Ptr wdWorld = nullptr);
    
    Solution unify(Expression exQuery, Solution solPrior, Solution::Bindings bdgBindings = {});
    
    template<class ... Args>
      static Prolog::Ptr create(Args ... args) {
      return std::make_shared<Prolog>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __PROLOG_H__ */
