#ifndef __STATEFULCONTEXT_H__
#define __STATEFULCONTEXT_H__


#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include <gdaplanner/contexts/Context.h>
#include <gdaplanner/World.h>


namespace gdaplanner {
  class StatefulContext : public Context {
  public:
    typedef std::shared_ptr<StatefulContext> Ptr;
    
  private:
    std::deque<World::Ptr> m_dqWorlds;
    
  protected:
  public:
    StatefulContext(std::string strIdentifier = "");
    ~StatefulContext();
    
    World::Ptr pushWorld();
    bool popWorld();
    World::Ptr currentWorld();
    
    void fact(Expression exFact);
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static StatefulContext::Ptr create(Args ... args) {
      return std::make_shared<StatefulContext>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __STATEFULCONTEXT_H__ */
