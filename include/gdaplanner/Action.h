#ifndef __ACTION_H__
#define __ACTION_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class Action : public Printable {
  public:
    typedef std::shared_ptr<Action> Ptr;
    
  private:
    Expression m_exAction;
    Expression m_exPreconditions;
    Expression m_exEffects;
    
  protected:
  public:
    Action(Expression exAction, Expression exPreconditions, Expression exEffects);
    ~Action();
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static Action::Ptr create(Args ... args) {
      return std::make_shared<Action>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __ACTION_H__ */
