#ifndef __STATE_H__
#define __STATE_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  class State : public Printable {
  public:
    /** Shared pointer to State class */
    typedef std::shared_ptr<State> Ptr;
    
  private:
    Expression m_exState;
    Expression m_exPreconditions;
    
  protected:
  public:
    State(Expression exState, Expression exPreconditions);
    ~State();
    
    virtual std::string toString() const override;
    
    Expression state();
    Expression preconditions();
    
    State::Ptr parametrize(Expression exTarget);
    
    template<class ... Args>
      static State::Ptr create(Args ... args) {
      return std::make_shared<State>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __STATE_H__ */
