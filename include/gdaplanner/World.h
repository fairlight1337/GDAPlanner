#pragma once
#ifndef __WORLD_H__
#define __WORLD_H__


#include <memory>
#include <iosfwd>
#include <list>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  /** \brief State management class holding a (mutable) reasoning/planning state
      
      This class holds the state of all asserted facts, allows
      assertion of new, retraction of existing ones, and presents a
      mechanism to check whether a certain condition holds, and what
      its bindings are.
  
      \since 0.1.0 */
  class World : public Printable {
  public:
    /** Shared pointer to World class */
    typedef std::shared_ptr<World> Ptr;
    typedef std::shared_ptr<const World> PtrConst;

  private:
    std::list<Expression> m_lstFacts;
    
  protected:
  public:
    World();
    ~World();
    
    bool assertFact(Expression exFact);
    std::vector<std::map<std::string, Expression>> retractFact(Expression exFact, unsigned int unLimit = 0, bool bExact = false);
    
    std::vector<std::map<std::string, Expression>> holds(Expression exFact, bool bExact = false);
    
    World::Ptr copy();
    
    virtual std::string toString() const override;
    
    template<class ... Args>
      static World::Ptr create(Args ... args) {
      return std::make_shared<World>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __WORLD_H__ */
