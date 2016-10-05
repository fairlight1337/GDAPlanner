#ifndef __ACTION_H__
#define __ACTION_H__


#include <memory>
#include <iostream>

#include <gdaplanner/Expression.h>
#include <gdaplanner/Predicate.h>
#include <gdaplanner/Printable.h>


namespace gdaplanner {
  /** \brief Class representing a first-order Action
      
      \since 0.1.0 */
  class Action : public Printable {
  public:
    /** Shared pointer to Action class */
    typedef std::shared_ptr<Action> Ptr;
    
  private:
    Predicate::Ptr m_pdPredicate;
    Expression m_exPreconditions;
    Expression m_exEffects;
    
  protected:
  public:
    Action(Predicate::Ptr pdPredicate, Expression exPreconditions, Expression exEffects);
    ~Action();
    
    virtual std::string toString() override;
    
    template<class ... Args>
      static Action::Ptr create(Args ... args) {
      return std::make_shared<Action>(std::forward<Args>(args)...);
    }
  };
}


#endif /* __ACTION_H__ */
