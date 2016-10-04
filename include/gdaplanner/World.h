#ifndef __WORLD_H__
#define __WORLD_H__


#include <memory>
#include <iostream>
#include <list>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class World : public Printable {
  public:
    typedef std::shared_ptr<World> Ptr;
    
  private:
    std::list<Expression> m_lstFacts;
    
  protected:
  public:
    World();
    ~World();
    
    bool assertFact(Expression exFact);
    std::vector<std::map<std::string, Expression>> retractFact(Expression exFact, unsigned int unLimit = 0);
    
    std::vector<std::map<std::string, Expression>> holds(Expression exFact, bool bExact = false);
    
    World::Ptr copy();
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static World::Ptr create(Args ... args) {
      return std::make_shared<World>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __WORLD_H__ */
